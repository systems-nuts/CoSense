#include "newton-irPass-LLVMIR-dequantization.h"
#include <unordered_map>
using namespace llvm;

#define FRAC_Q 16
#define FRAC_BASE (1 << FRAC_Q)
#define BIT_WIDTH 32

extern "C" {

// Create a dequantized function with the same signature as the original function
Function *
createDequantizedFunction(Function & llvmIrFunction, Type * floatType)
{
	std::vector<Type *> params;
	for (auto & arg : llvmIrFunction.args())
	{
		if (isQuantizedType(arg.getType()))
		{
			params.push_back(floatType);
			llvm::errs() << "Dequantizing parameter: " << arg.getName() << " from " << *arg.getType() << " to " << *floatType << "\n";
		}
		else
		{
			params.push_back(arg.getType());
		}
	}

	Type *	       returnType  = isQuantizedType(llvmIrFunction.getReturnType()) ? floatType : llvmIrFunction.getReturnType();
	FunctionType * newFuncType = FunctionType::get(returnType, params, false);
	Function *     newFunc	   = Function::Create(newFuncType, llvmIrFunction.getLinkage(), llvmIrFunction.getName() + "_dequantized", llvmIrFunction.getParent());
	return newFunc;
}

// Clone the function body from the original function to the new dequantized function
void
cloneFunctionBody(Function & oldFunc, Function * newFunc)
{
	ValueToValueMapTy      vmap;
	Function::arg_iterator newArgIt = newFunc->arg_begin();
	for (auto & oldArg : oldFunc.args())
	{
		newArgIt->setName(oldArg.getName());
		vmap[&oldArg] = &*newArgIt++;
	}
	SmallVector<ReturnInst *, 8> returns;
	llvm::CloneFunctionInto(newFunc, &oldFunc, vmap, CloneFunctionChangeType::LocalChangesOnly, returns);
}

// Replace all uses of the original function with the new dequantized function
void
replaceFunctionUses(Function & oldFunc, Function * newFunc)
{
	std::vector<User *> users(oldFunc.user_begin(), oldFunc.user_end());
	for (auto * U : users)
	{
		if (CallInst * callInst = dyn_cast<CallInst>(U))
		{
			std::vector<Value *> args;
			for (auto & arg : callInst->args())
			{
				args.push_back(arg);
			}
			CallInst * newCall = CallInst::Create(newFunc, args, "", callInst);
			newCall->setCallingConv(callInst->getCallingConv());
			newCall->setDebugLoc(callInst->getDebugLoc());
			llvm::errs() << "Replacing call: " << *callInst << " with " << *newCall << "\n";
			callInst->replaceAllUsesWith(newCall);
			callInst->eraseFromParent();
		}
	}
}

// Set the dequantized type for a given value
void
setDequantizedType(Value * inValue, Type * floatType)
{
	llvm::errs() << "Entering setDequantizedType\n";
	if (inValue == nullptr)
	{
		llvm::errs() << "inValue is nullptr\n";
		return;
	}
	auto	 valueType = inValue->getType();
	unsigned pointerAddr;
	bool	 isPointer = false;

	if (valueType != nullptr)
	{
		if (valueType->isPointerTy())
		{
			isPointer   = true;
			pointerAddr = valueType->getPointerAddressSpace();
			valueType   = valueType->getPointerElementType();
		}

		if (isQuantizedType(valueType) || valueType->isArrayTy())
		{
			llvm::errs() << "Original type: " << *valueType << "\n";
			llvm::errs() << "New dequantized type: " << *floatType << "\n";
			if (isPointer)
			{
				inValue->mutateType(floatType->getPointerTo(pointerAddr));
			}
			else
			{
				inValue->mutateType(floatType);
			}
		}
		else
		{
			llvm::errs() << "Unsupported type for dequantization: " << *valueType << "\n";
		}
	}
	else
	{
		llvm::errs() << "Value type is nullptr\n";
	}
}
}

// Adapt type casts to the dequantized type
void adaptTypeCast(Function &llvmIrFunction, Type *floatType) {
	llvm::errs() << "Entering adaptTypeCast\n";
	for (BasicBlock &llvmIrBasicBlock : llvmIrFunction) {
		for (BasicBlock::iterator itBB = llvmIrBasicBlock.begin(); itBB != llvmIrBasicBlock.end();) {
			Instruction *llvmIrInstruction = &*itBB++;
			switch (llvmIrInstruction->getOpcode()) {
				case Instruction::FPToUI:
				case Instruction::FPToSI:
				case Instruction::UIToFP:
				case Instruction::SIToFP:
				{
					IRBuilder<> Builder(llvmIrInstruction);
					Value *operand = llvmIrInstruction->getOperand(0);
					Value *newInst = nullptr;

					if (llvmIrInstruction->getOpcode() == Instruction::FPToUI || llvmIrInstruction->getOpcode() == Instruction::FPToSI) {
						if (isQuantizedType(operand->getType())) {
							newInst = Builder.CreateSIToFP(operand, floatType);
						}
					} else if (llvmIrInstruction->getOpcode() == Instruction::UIToFP || llvmIrInstruction->getOpcode() == Instruction::SIToFP) {
						if (isQuantizedType(llvmIrInstruction->getType())) {
							newInst = Builder.CreateSIToFP(operand, floatType);
						}
					}

					if (newInst) {
						llvmIrInstruction->replaceAllUsesWith(newInst);
						llvmIrInstruction->eraseFromParent();
					}
					break;
				}
				case Instruction::FPExt:
				case Instruction::FPTrunc:
				{
					IRBuilder<> Builder(llvmIrInstruction);
					Value *operand = llvmIrInstruction->getOperand(0);
					Value *newInst = nullptr;

					if (isQuantizedType(operand->getType())) {
						newInst = Builder.CreateSIToFP(operand, floatType);
					} else {
						newInst = Builder.CreateFPCast(operand, floatType);
					}

					if (newInst) {
						llvmIrInstruction->replaceAllUsesWith(newInst);
						llvmIrInstruction->eraseFromParent();
					}
					break;
				}
				case Instruction::BitCast:
				{
					IRBuilder<> Builder(llvmIrInstruction);
					Value *operand = llvmIrInstruction->getOperand(0);
					Value *newInst = Builder.CreateBitCast(operand, floatType);

					if (newInst) {
						llvmIrInstruction->replaceAllUsesWith(newInst);
						llvmIrInstruction->eraseFromParent();
					}
					break;
				}
				default:
					break;
			}
		}
	}
	llvm::errs() << "Exiting adaptTypeCast\n";
}

// Main function to perform LLVM IR auto dequantization
void
irPassLLVMIRAutoDequantization(State * N, llvm::Function & llvmIrFunction, std::vector<llvm::Function *> & functionsToInsert)
{
	flexprint(N->Fe, N->Fm, N->Fpinfo, "\tauto dequantization.\n");
	llvm::errs() << "Entering irPassLLVMIRAutoDequantization\n";

	std::string functionName = llvmIrFunction.getName().str();
	if (functionName == "llvm.dbg.declare" || functionName == "llvm.dbg.value" || functionName == "llvm.dbg.label" || functionName == "fixmul" || functionName == "floatIntMul" || functionName == "sqrt")
	{
		llvm::errs() << "Skipping function: " << functionName << "\n";
		return;
	}

	Type * floatType = Type::getFloatTy(llvmIrFunction.getContext());

	Function * newFunc = createDequantizedFunction(llvmIrFunction, floatType);
	cloneFunctionBody(llvmIrFunction, newFunc);
	replaceFunctionUses(llvmIrFunction, newFunc);

	llvm::errs() << "Finished handling function signature for: " << newFunc->getName() << "\n";

	handleLoadStoreInstructions(llvmIrFunction, floatType);
	llvm::errs() << "Exiting irPassLLVMIRAutoDequantization\n";
}
