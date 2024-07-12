/*
Authored 2022. Pei Mu.
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
*	Redistributions of source code must retain the above
	copyright notice, this list of conditions and the following
	disclaimer.
*	Redistributions in binary form must reproduce the above
	copyright notice, this list of conditions and the following
	disclaimer in the documentation and/or other materials
	provided with the distribution.
*	Neither the name of the author nor the names of its
	contributors may be used to endorse or promote products
	derived from this software without specific prior written
	permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include "newton-irPass-LLVMIR-quantization.h"
#include <unordered_map>
using namespace llvm;

#define FRAC_Q 16
#define FRAC_BASE (1 << FRAC_Q)
#define BIT_WIDTH 32

extern "C" {

// check if the type is a floating type
bool
isValidFloatingType(Type * type)
{
	return type->isFloatTy() || type->isDoubleTy();
}

// Set the quantized type for a given value
void
setQuantizedType(Value * inValue, Type * quantizedType)
{
	llvm::errs() << "Entering setQuantizedType\n";
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
			isPointer = true;
			// If the value is a pointer, get the address space and element type
			pointerAddr = valueType->getPointerAddressSpace();
			valueType   = valueType->getPointerElementType();
		}

		// 跳过整数类型和结构体类型
		if (valueType->isIntegerTy() || valueType->isStructTy())
		{
			llvm::errs() << "Skipping quantization for type: " << *valueType << "\n";
			return;
		}

		if (isValidFloatingType(valueType) || valueType->isArrayTy())
		{
			// Print the original and new types for debugging
			llvm::errs() << "Original type: " << *valueType << "\n";
			llvm::errs() << "New quantized type: " << *quantizedType << "\n";
			// If the value is a pointer, get the address space and element type
			if (isPointer)
			{
				inValue->mutateType(quantizedType->getPointerTo(pointerAddr));
			}
			else
			{
				// Otherwise, directly set the type to the quantized type
				inValue->mutateType(quantizedType);
			}
		}
		else
		{
			llvm::errs() << "Unsupported type for quantization: " << *valueType << "\n";
		}
	}
	else
	{
		llvm::errs() << "Value type is nullptr\n";
	}
}
// Quantize constants within an instruction
std::unordered_map<ConstantFP *, Value *> quantizedValueCache;

// Ensure load and store instructions have matching types
void
handleLoadStoreInstructions(Function & llvmIrFunction, Type * quantizedType)
{
	llvm::errs() << "Entering handleLoadStoreInstructions\n";
	for (BasicBlock & llvmIrBasicBlock : llvmIrFunction)
	{
		for (BasicBlock::iterator itBB = llvmIrBasicBlock.begin(); itBB != llvmIrBasicBlock.end();)
		{
			Instruction * llvmIrInstruction = &*itBB++;
			if (llvmIrInstruction->getOpcode() == Instruction::Load)
			{
				if (auto loadInst = dyn_cast<LoadInst>(llvmIrInstruction))
				{
					auto ptr = loadInst->getPointerOperand();
					if (ptr->getType()->getPointerElementType()->isFloatTy() ||
					    ptr->getType()->getPointerElementType()->isDoubleTy())
					{
						ptr->mutateType(quantizedType->getPointerTo());
					}
				}
			}
			else if (llvmIrInstruction->getOpcode() == Instruction::Store)
			{
				if (auto storeInst = dyn_cast<StoreInst>(llvmIrInstruction))
				{
					auto ptr = storeInst->getPointerOperand();
					if (ptr->getType()->getPointerElementType()->isFloatTy() ||
					    ptr->getType()->getPointerElementType()->isDoubleTy())
					{
						ptr->mutateType(quantizedType->getPointerTo());
					}
				}
			}
		}
	}
}

// Create a quantized function with the same signature as the original function
Function *
createQuantizedFunction(Function & llvmIrFunction, Type * quantizedType)
{
	std::vector<Type *> params;
	for (auto & arg : llvmIrFunction.args())
	{
		// If the argument type is float or double, use the quantized type
		if (arg.getType()->isFloatTy() || arg.getType()->isDoubleTy())
		{
			params.push_back(quantizedType);
			llvm::errs() << "Quantizing parameter: " << arg.getName() << " from " << *arg.getType() << " to " << *quantizedType << "\n";
		}
		else
		{
			params.push_back(arg.getType());
		}
	}

	// Determine the return type: if it's float or double, use the quantized type
	Type * returnType = llvmIrFunction.getReturnType()->isFloatTy() || llvmIrFunction.getReturnType()->isDoubleTy()
				? quantizedType
				: llvmIrFunction.getReturnType();
	llvm::errs() << "Original Function: " << llvmIrFunction.getName() << "\n";
	llvm::errs() << "Original Return Type: " << *llvmIrFunction.getReturnType() << "\n";
	for (auto & arg : llvmIrFunction.args())
	{
		llvm::errs() << "Original Arg: " << *arg.getType() << "\n";
	}
	llvm::errs() << "New Function: " << llvmIrFunction.getName() + "_quantized" << "\n";
	llvm::errs() << "New Return Type: " << *returnType << "\n";
	for (auto & param : params)
	{
		llvm::errs() << "New Arg: " << *param << "\n";
	}

	// Create a new function type with the modified parameters and return type
	FunctionType * newFuncType = FunctionType::get(returnType, params, false);
	// Create the new function in the same module with a modified name
	Function * newFunc = Function::Create(newFuncType, llvmIrFunction.getLinkage(), llvmIrFunction.getName() + "_quantized", llvmIrFunction.getParent());
	return newFunc;
}

// Clone the function body from the original function to the new quantized function
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
	// Clone the function body into the new function
	SmallVector<ReturnInst *, 8> returns;
	llvm::CloneFunctionInto(newFunc, &oldFunc, vmap, CloneFunctionChangeType::LocalChangesOnly, returns);
}

// Replace all uses of the original function with the new quantized function
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
			// Create a new call instruction to the new function
			CallInst * newCall = CallInst::Create(newFunc, args, "", callInst);
			newCall->setCallingConv(callInst->getCallingConv());
			newCall->setDebugLoc(callInst->getDebugLoc());
			llvm::errs() << "Replacing call: " << *callInst << " with " << *newCall << "\n";
			callInst->replaceAllUsesWith(newCall);
			callInst->eraseFromParent();
		}
	}
}

bool
isQuantizedFunctionName(const std::string & functionName)
{
	return functionName.find("_quantized") != std::string::npos;
}

// A list of functions to erase after processing
std::vector<Function *> functionsToErase;

// Track processed functions to avoid duplicate processing
std::set<std::string> processedFunctions;

// Handle the function signature change for quantization
void
handleFunctionSignature(Function & llvmIrFunction, Type * quantizedType)
{
	llvm::errs() << "Calling handleFunctionSignature for function: " << llvmIrFunction.getName() << "\n";
	// Skip certain functions
	std::string functionName = llvmIrFunction.getName().str();
	if (functionName == "llvm.dbg.declare" || functionName == "llvm.dbg.value" || functionName == "llvm.dbg.label" || functionName == "fixmul" || functionName == "floatIntMul")
	// if (functionName == "llvm.dbg.declare" || functionName == "llvm.dbg.value" || functionName == "llvm.dbg.label" || functionName == "fixmul")
	{
		llvm::errs() << "Skipping function signature handling for: " << functionName << "\n";
		return;
	}

	if (processedFunctions.find(functionName) != processedFunctions.end())
	{
		llvm::errs() << "Function already processed: " << functionName << "\n";
		return;
	}

	if (isQuantizedFunctionName(functionName))
	{
		llvm::errs() << "Skipping already quantized function: " << functionName << "\n";
		return;
	}

	processedFunctions.insert(functionName);

	Function * newFunc = createQuantizedFunction(llvmIrFunction, quantizedType);
	cloneFunctionBody(llvmIrFunction, newFunc);
	replaceFunctionUses(llvmIrFunction, newFunc);

	// Add the old function to the list of functions to erase
	functionsToErase.push_back(&llvmIrFunction);
	llvmIrFunction.replaceAllUsesWith(UndefValue::get(llvmIrFunction.getType()));
	llvm::errs() << "Finished handling function signature for: " << newFunc->getName() << "\n";
}

// Function to actually erase functions after processing
void
eraseOldFunctions()
{
	llvm::errs() << "Entering eraseOldFunctions\n";
	for (auto * func : functionsToErase)
	{
		llvm::errs() << "Erasing old function: " << func->getName() << "\n";
		func->eraseFromParent();
	}
	functionsToErase.clear();
	llvm::errs() << "Exiting eraseOldFunctions\n";
}

// Quantize constants within an instruction

void
quantizeConstant(Instruction * inInstruction, Type * quantizedType)
{
	llvm::errs() << "Entering quantizeConstant\n";
	for (size_t idx = 0; idx < inInstruction->getNumOperands(); idx++)
	{
		Value * inValue = inInstruction->getOperand(idx);

		// Skip if the operand is not a floating-point constant
		if (!isa<llvm::ConstantFP>(inValue))
		{
			continue;
		}

		auto * constFp = llvm::dyn_cast<ConstantFP>(inValue);

		// Check the cache for the quantized value
		auto cachedValue = quantizedValueCache.find(constFp);

		if (cachedValue != quantizedValueCache.end())
		{
			inInstruction->replaceUsesOfWith(inValue, cachedValue->second);
			continue;
		}

		// Compute the quantized value if not found in cache
		Value * newValue = nullptr;
		if (inValue->getType()->isFloatTy())
		{
			// Convert float constant to fixed-point
			float constValue = constFp->getValueAPF().convertToFloat();
			constValue *= FRAC_BASE;
			int32_t fixedPointValue = round(constValue);
			// newValue = ConstantInt::get(quantizedType, round(constValue), true);
			newValue = ConstantFP::get(inValue->getType(), (float)fixedPointValue / FRAC_BASE);
		}
		else if (inValue->getType()->isDoubleTy())
		{
			// Convert double constant to fixed-point and back to double
			double constValue = constFp->getValueAPF().convertToDouble();
			constValue *= FRAC_BASE;
			int64_t fixedPointValue = round(constValue);
			// newValue = ConstantInt::get(quantizedType, round(constValue), true);
			newValue = ConstantFP::get(inValue->getType(), (double)fixedPointValue / FRAC_BASE);
		}
		else
		{
			llvm::errs() << "Unknown floating type: " << *inValue->getType() << "\n";
			// assert(false && "unknown floating type");
			continue;
		}

		// Cache the quantized value
		quantizedValueCache[constFp] = newValue;

		// Replace all uses of the original value with the new quantized value
		inInstruction->replaceUsesOfWith(inValue, newValue);
	}
}

void
handleFloatIntMul(Instruction * llvmIrInstruction, Type * intType, Type * floatType, Function * floatIntMul)
{
	llvm::errs() << "Handling FloatIntMul\n";
	llvm::errs() << "Original Instruction: " << *llvmIrInstruction << "\n";
	IRBuilder<> Builder(llvmIrInstruction);

	// 获取操作数
	Value * lhs = llvmIrInstruction->getOperand(0);
	Value * rhs = llvmIrInstruction->getOperand(1);

	llvm::errs() << "LHS: " << *lhs << "\n";
	llvm::errs() << "RHS: " << *rhs << "\n";

	// 确保左操作数是浮点数，右操作数是整数
	if (!lhs->getType()->isFloatTy() && !lhs->getType()->isDoubleTy())
	{
		std::swap(lhs, rhs);
	}

	if (lhs->getType()->isFloatTy() || lhs->getType()->isDoubleTy())
	{
		if (rhs->getType()->isIntegerTy())
		{
			llvm::CallInst * callInst = Builder.CreateCall(floatIntMul, {lhs, rhs});
			llvmIrInstruction->replaceAllUsesWith(callInst);
			llvmIrInstruction->eraseFromParent();
			llvm::errs() << "Replaced with call to floatIntMul\n";
		}
		else
		{
			llvm::errs() << "RHS is not an integer\n";
		}
	}
	else
	{
		llvm::errs() << "LHS is not a float\n";
	}

	llvm::errs() << "Finished handling FloatIntMul\n";
}

void
simplifyConstant(Instruction * inInstruction, Type * quantizedType, Function * floatIntMul)

{
	llvm::errs() << "Entering simplifyConstant\n";
	auto checkDecimal = [](float decimalNum) {
		int digits = 0;
		while (fabs(round(decimalNum) - decimalNum) > 0.001 && digits < 4)
		{
			decimalNum *= 10;  // Scale decimal to avoid precision issues
			digits++;
		}
		return decimalNum;
	};



	auto compensateFP = [inInstruction, quantizedType, floatIntMul](float quantizedNum, float decimalNum) {
		float compensateNum = quantizedNum / decimalNum;
		//		Value *	 constOperand, *nonConstOperand;
		//
		//		unsigned constIdx, nonConstIdx;
		Value *	 constOperand	 = nullptr;
		Value *	 nonConstOperand = nullptr;
		unsigned constIdx	 = 0;
		unsigned nonConstIdx	 = 0;

		if (isa<llvm::Constant>(inInstruction->getOperand(0)))
		{
			constIdx	= 0;
			nonConstIdx	= 1;
			constOperand	= inInstruction->getOperand(0);
			nonConstOperand = inInstruction->getOperand(1);
		}
		else
		{
			constIdx	= 1;
			nonConstIdx	= 0;
			constOperand	= inInstruction->getOperand(1);
			nonConstOperand = inInstruction->getOperand(0);
		}

		// auto quantizeNumValue = ConstantInt::get(quantizedType, round(quantizedNum), true);
		// auto quantizeNumValue = ConstantFP::get(nonConstOperand->getType(), quantizedNum);
		auto quantizeNumValue = ConstantInt::get(quantizedType, round(quantizedNum), true);

		if (compensateNum == 1)
		{
			llvm::errs() << "Compensation factor is 1, directly setting the quantized value\n";
			llvm::errs() << "Original Instruction: " << *inInstruction << "\n";
			llvm::errs() << "Quantized value: " << *quantizeNumValue << "\n";
			inInstruction->setOperand(constIdx, quantizeNumValue);


			IRBuilder<> Builder(inInstruction);

			if (nonConstOperand->getType()->isFloatTy() || nonConstOperand->getType()->isDoubleTy())
			{
				// 调用 handleFloatIntMul 处理浮点数和整数相乘
				llvm::errs() << "Calling handleFloatIntMul\n";
				llvm::CallInst * callInst = Builder.CreateCall(floatIntMul, {nonConstOperand, quantizeNumValue});
				inInstruction->replaceAllUsesWith(callInst);
			}
			else
			{
				// Replace the original fmul instruction with integer mul
				llvm::errs() << "Replacing original fmul instruction with integer mul\n";
				Value *	    newMulValue = Builder.CreateMul(nonConstOperand, quantizeNumValue);
				inInstruction->replaceAllUsesWith(newMulValue);
			}
			inInstruction->eraseFromParent();
		}
		else{
			llvm::errs() << "Applying compensation to the fixed-point arithmetic\n";
			auto	      compensateNumValue = ConstantInt::get(quantizedType, round(compensateNum), true);
			IRBuilder<>   Builder(inInstruction);
			Instruction * insertPoint = inInstruction->getNextNode();
			Builder.SetInsertPoint(insertPoint);

			Value * newFirstInst  = nullptr;
			Value * newSecondInst = nullptr;
			auto	instOpCode    = inInstruction->getOpcode();

			if (instOpCode == Instruction::FMul)
			{
				newFirstInst  = Builder.CreateMul(nonConstOperand, quantizeNumValue);
				newSecondInst = Builder.CreateSDiv(newFirstInst, compensateNumValue);
			}
			else if (instOpCode == Instruction::FDiv && isa<llvm::Constant>(inInstruction->getOperand(1)))
			{
				newFirstInst  = Builder.CreateMul(nonConstOperand, compensateNumValue);
				newSecondInst = Builder.CreateSDiv(newFirstInst, quantizeNumValue);
			}
			else if (instOpCode == Instruction::FDiv && isa<llvm::Constant>(inInstruction->getOperand(0)))
			{
				newFirstInst  = Builder.CreateSDiv(quantizeNumValue, nonConstOperand);
				newSecondInst = Builder.CreateSDiv(newFirstInst, compensateNumValue);
			}

			if (newSecondInst)
			{
				inInstruction->replaceAllUsesWith(newSecondInst);
				inInstruction->eraseFromParent();
			}
			else
			{
				llvm::errs() << "Failed to create new compensated instruction\n";
			}
		}
	};

	for (size_t idx = 0; idx < inInstruction->getNumOperands(); idx++)
	{
		Value * inValue = inInstruction->getOperand(idx);
		if (!isa<llvm::ConstantFP>(inValue))
		{
			continue;
		}

		ConstantFP * constFp  = dyn_cast<ConstantFP>(inValue);
		Value *	     newValue = nullptr;

		if (inValue->getType()->isFloatTy())
		{
			float constValue = constFp->getValueAPF().convertToFloat();
			compensateFP(checkDecimal(constValue), constValue);
		}
		else if (inValue->getType()->isDoubleTy())
		{
			double constValue = constFp->getValueAPF().convertToDouble();
			compensateFP(checkDecimal(constValue), constValue);
		}
		else
		{
			assert(false && "unknown floating type");
		}
	}
	llvm::errs() << "Exiting simplifyConstant\n";
}

void
substituteHardcodeFunc(Instruction * inInstruction, Type * quantizedType, llvm::Function * func)
{
	llvm::errs() << "Entering substituteHardcodeFunc\n";
	llvm::errs() << "Original Instruction: " << *inInstruction << "\n";
	IRBuilder<>   Builder(inInstruction);
	Instruction * insertPoint = inInstruction->getNextNode();
	Builder.SetInsertPoint(insertPoint);

	Value * lhs = inInstruction->getOperand(0);
	Value * rhs = inInstruction->getOperand(1);

	llvm::errs() << "LHS: " << *lhs << "\n";
	llvm::errs() << "RHS: " << *rhs << "\n";

	llvm::CallInst * callInst = Builder.CreateCall(func, {lhs, rhs});
	llvm::errs() << "Created call instruction: " << *callInst << "\n";

	inInstruction->replaceAllUsesWith(callInst);
	inInstruction->removeFromParent();
	llvm::errs() << "Exiting substituteHardcodeFunc\n";
}

llvm::Function *
createFloatIntMul(Module * irModule, Type * intType, Type * floatType, std::vector<llvm::Function *> & functionsToInsert)
{
	llvm::errs() << "Entering createFloatIntMul\n";

	// 检查 irModule 是否有效
	if (!irModule)
	{
		llvm::errs() << "Error: irModule is nullptr\n";
		return nullptr;
	}

	std::string floatIntMulFuncName = "floatIntMul";
	for (auto & function : *irModule)
	{
		if (function.getName() == floatIntMulFuncName)
		{
			llvm::errs() << "floatIntMul already exists\n";
			return &function;
		}
	}

	llvm::FunctionType * funcType = llvm::FunctionType::get(intType, {floatType, intType}, false);
	llvm::Function *     func     = llvm::Function::Create(funcType, llvm::Function::PrivateLinkage, floatIntMulFuncName, irModule);

	llvm::BasicBlock * entryBB = llvm::BasicBlock::Create(irModule->getContext(), "entry", func);
	llvm::IRBuilder<>  builder(entryBB);
	builder.SetInsertPoint(entryBB);

	// Get function arguments

	llvm::Function::arg_iterator arg1     = &*(func->arg_begin());
	llvm::Value *		     floatArg = &*arg1;
	llvm::Function::arg_iterator arg2     = &*(++arg1);
	llvm::Value *		     intArg   = &*arg2;

	// Generate float * int multiplication instruction
	llvm::Value * intToFloat = builder.CreateSIToFP(intArg, floatType);
	llvm::Value * mulInst	 = builder.CreateFMul(floatArg, intToFloat);
	llvm::Value * floatToInt = builder.CreateFPToSI(mulInst, intType);

	builder.CreateRet(floatToInt);

	functionsToInsert.emplace_back(func);
	llvm::errs() << "Created floatIntMul function: " << func->getName() << "\n";
	return func;
}

// Create a fixed-point multiplication function
llvm::Function *
createFixMul(Module * irModule, Type * quantizedType, std::vector<llvm::Function *> & functionsToInsert)
{
	llvm::errs() << "Entering createFixMul\n";

	// 检查 irModule 是否有效
	if (!irModule)
	{
		llvm::errs() << "Error: irModule is nullptr\n";
		return nullptr;
	}

	std::string fixmulFuncName = "fixmul";
	for (auto & function : *irModule)
	{
		if (function.getName() == fixmulFuncName)
		{
			llvm::errs() << "fixmul already exists\n";
			return &function;
		}
	}

	llvm::FunctionType * funcType = llvm::FunctionType::get(quantizedType, {quantizedType, quantizedType}, false);
	llvm::Function *     func     = llvm::Function::Create(funcType, llvm::Function::PrivateLinkage, fixmulFuncName, irModule);

	llvm::BasicBlock * entryBB = llvm::BasicBlock::Create(irModule->getContext(), "entry", func);
	llvm::IRBuilder<>  builder(entryBB);
	builder.SetInsertPoint(entryBB);

	// 生成 fixed-point 乘法指令
	Type * higherQuantizedType;
	switch (BIT_WIDTH)
	{
		case 8:
			higherQuantizedType = Type::getInt16Ty(irModule->getContext());
			break;
		case 16:
			higherQuantizedType = Type::getInt32Ty(irModule->getContext());
			break;
		default:
			higherQuantizedType = Type::getInt64Ty(irModule->getContext());
			break;
	}

	llvm::Function::arg_iterator arg1      = &*(func->arg_begin());
	llvm::Value *		     sext1     = builder.CreateSExt(arg1, higherQuantizedType);
	llvm::Function::arg_iterator arg2      = &*(++arg1);
	llvm::Value *		     sext2     = builder.CreateSExt(arg2, higherQuantizedType);
	llvm::Value *		     mulInst   = builder.CreateMul(sext1, sext2);
	llvm::Value *		     ashrInst  = builder.CreateAShr(mulInst, ConstantInt::get(higherQuantizedType, FRAC_Q));
	llvm::Value *		     truncInst = builder.CreateTrunc(ashrInst, quantizedType);
	builder.CreateRet(truncInst);

	functionsToInsert.emplace_back(func);
	llvm::errs() << "Created fixmul function: " << func->getName() << "\n";
	return func;
}

// Quantize simple floating-point instructions
CmpInst::Predicate
quantizePredict(CmpInst::Predicate predict)
{
	llvm::errs() << "Entering quantizePredict with predicate: " << predict << "\n";
	switch (predict)
	{
		case FCmpInst::FCMP_OEQ:  // equal
		case FCmpInst::FCMP_UEQ:
			return ICmpInst::ICMP_EQ;  // greater than
		case FCmpInst::FCMP_OGT:
		case FCmpInst::FCMP_UGT:
			return ICmpInst::ICMP_SGT;
		case FCmpInst::FCMP_OGE:  // greater than or equal
		case FCmpInst::FCMP_UGE:
			return ICmpInst::ICMP_SGE;
		case FCmpInst::FCMP_OLT:  // less than
		case FCmpInst::FCMP_ULT:
			return ICmpInst::ICMP_SLT;
		case FCmpInst::FCMP_OLE:  // less than or equal
		case FCmpInst::FCMP_ULE:
			return ICmpInst::ICMP_SLE;
		case FCmpInst::FCMP_ONE:  // not equal
		case FCmpInst::FCMP_UNE:
			return ICmpInst::ICMP_NE;

			/*
				case FCmpInst::FCMP_ORD:  // ordered (no NaNs)
					// For ordered, we map it to ICMP_NE, assuming integers cannot be NaN
					return ICmpInst::ICMP_NE;
				case FCmpInst::FCMP_UNO:  // unordered (at least one NaN)
					// For unordered, there is no direct integer equivalent, map to ICMP_EQ for safety
					return ICmpInst::ICMP_EQ;*/

		default:
			llvm::errs() << "Unhandled floating point predicate\n";
			return ICmpInst::ICMP_EQ;  // Default to equal for safety
	}
}

void
quantizeSimpleFPInstruction(Instruction * inInstruction, Type * quantizedType)
{
	llvm::errs() << "Entering quantizeSimpleFPInstruction\n";
	if (!inInstruction)
	{
		llvm::errs() << "inInstruction is nullptr\n";
		return;
	}
	IRBuilder<>   Builder(inInstruction);
	Instruction * insertPoint = inInstruction->getNextNode();
	Builder.SetInsertPoint(insertPoint);
	Value * newInst = nullptr;

	// Ensure the quantized type is not null
	if (!quantizedType)
	{
		llvm::errs() << "Error: Quantized type is nullptr\n";
		return;
	}

	llvm::errs() << "Instruction Opcode: " << inInstruction->getOpcodeName() << "\n";
	switch (inInstruction->getOpcode())
	{
		case Instruction::FAdd:
		{
			llvm::errs() << "Handling FAdd\n";
			// Replace floating-point addition with integer addition
			newInst = Builder.CreateAdd(inInstruction->getOperand(0), inInstruction->getOperand(1));
			break;
		}
//		case Instruction::FSub:
//		{
//			llvm::errs() << "Handling FSub\n";
//			Value * op0 = inInstruction->getOperand(0);
//			Value * op1 = inInstruction->getOperand(1);
//			if (op0->getType()->isFloatTy() || op0->getType()->isDoubleTy())
//			{
//				op0 = Builder.CreateFPToSI(op0, quantizedType);
//			}
//			if (op1->getType()->isFloatTy() || op1->getType()->isDoubleTy())
//			{
//				op1 = Builder.CreateFPToSI(op1, quantizedType);
//			}
//			newInst = Builder.CreateSub(op0, op1);
//			break;
//		}

		case Instruction::FSub:
		{
			llvm::errs() << "Handling FSub\n";
			Value * op0 = inInstruction->getOperand(0);
			Value * op1 = inInstruction->getOperand(1);
			llvm::errs() << "Original Operand 0: " << *op0 << "\n";
			llvm::errs() << "Original Operand 1: " << *op1 << "\n";
			if (op0->getType()->isFloatTy() || op0->getType()->isDoubleTy())
			{
				op0 = Builder.CreateFPToSI(op0, quantizedType);
				llvm::errs() << "Converted Operand 0 to fixed-point: " << *op0 << "\n";
			}
			if (op1->getType()->isFloatTy() || op1->getType()->isDoubleTy())
			{
				op1 = Builder.CreateFPToSI(op1, quantizedType);
				llvm::errs() << "Converted Operand 1 to fixed-point: " << *op1 << "\n";
			}
			newInst = Builder.CreateSub(op0, op1);
			llvm::errs() << "Created Sub instruction: " << *newInst << "\n";
			break;
		}



		case Instruction::FRem:
		{
			llvm::errs() << "Handling FRem\n";
			// Replace floating-point remainder with integer remainder
			newInst = Builder.CreateSRem(inInstruction->getOperand(0), inInstruction->getOperand(1));
			break;
		}

		case Instruction::FCmp:
			// Replace floating-point comparison with integer comparisonm
			llvm::errs() << "Handling FCmp\n";
			if (auto fcmp_inst = dyn_cast<FCmpInst>(inInstruction))
			{
				CmpInst::Predicate pred = quantizePredict(fcmp_inst->getPredicate());
				if (fcmp_inst->getPredicate() == FCmpInst::FCMP_TRUE)
				{
					newInst = ConstantInt::getTrue(quantizedType);
				}
				else if (fcmp_inst->getPredicate() == FCmpInst::FCMP_FALSE)
				{
					newInst = ConstantInt::getFalse(quantizedType);
				}
				else
				{
					// Convert floating-point operands to integer operands
					Value * fpOp0 = fcmp_inst->getOperand(0);
					Value * fpOp1 = fcmp_inst->getOperand(1);

					// Debug output to check types
					llvm::errs() << "Entering FCmp case\n";
					llvm::errs() << "Original Operand 0 type: " << *fpOp0->getType() << "\n";
					llvm::errs() << "Original Operand 1 type: " << *fpOp1->getType() << "\n";

					Value * intOp0 = Builder.CreateFPToSI(fpOp0, quantizedType);
					Value * intOp1 = Builder.CreateFPToSI(fpOp1, quantizedType);

					// Debug output to check types after conversion
					llvm::errs() << "Converted Operand 0 type: " << *intOp0->getType() << "\n";
					llvm::errs() << "Converted Operand 1 type: " << *intOp1->getType() << "\n";
					llvm::errs() << "Expected quantizedType: " << *quantizedType << "\n";

					// Assert to ensure types are correct
					assert(intOp0->getType() == intOp1->getType() && "Both operands to ICmp instruction are not of the same type!");

					newInst = Builder.CreateICmp(pred, intOp0, intOp1);
				}
			}
			break;

			/*
			 * Change fneg(a) to `0-a`.
			 * */
		case Instruction::FNeg:
		{
			llvm::errs() << "Handling FNeg\n";
			// Replace floating-point negation with integer negation
			Value * op = inInstruction->getOperand(0);
			if (op->getType()->isFloatTy() || op->getType()->isDoubleTy())
			{
				op = Builder.CreateFPToSI(op, quantizedType);
			}
			auto constZero = ConstantInt::get(quantizedType, 0, true);
			newInst = Builder.CreateSub(constZero, op);
			break;


		}
		default:
			llvm::errs() << "Unhandled floating point instruction\n";
			break;
	}

	if (newInst)
	{
		llvm::errs() << "Replacing instruction with newInst\n";
		inInstruction->replaceAllUsesWith(newInst);
		inInstruction->removeFromParent();
	}
	else
	{
		llvm::errs() << "No new instruction created\n";
	}
}

// Adapt type casts to the quantized type
void
adaptTypeCast(llvm::Function & llvmIrFunction, Type * quantizedType)
{
	llvm::errs() << "Entering adaptTypeCast\n";
	for (BasicBlock & llvmIrBasicBlock : llvmIrFunction)
	{
		for (BasicBlock::iterator itBB = llvmIrBasicBlock.begin(); itBB != llvmIrBasicBlock.end();)
		{
			Instruction * llvmIrInstruction = &*itBB++;
			switch (llvmIrInstruction->getOpcode())
			{
				case Instruction::FPToUI:
				case Instruction::FPToSI:
				case Instruction::SIToFP:
//				case Instruction::UIToFP:
//				{
//					auto sourceOp = llvmIrInstruction->getOperand(0);
//					IRBuilder<> Builder(llvmIrInstruction);
//
//					// 如果源操作数类型和指令类型相同，直接替换
//					if (sourceOp->getType() == llvmIrInstruction->getType())
//					{
//						llvmIrInstruction->replaceAllUsesWith(sourceOp);
//						llvmIrInstruction->removeFromParent();
//					}
//					else
//					{
//						llvm::errs() << "Handling UIToFP: Source type does not match instruction type, adjusting types\n";
//						if (sourceOp->getType()->isIntegerTy())
//						{
//							// 如果源操作数是整数类型，转换为浮点类型
//							sourceOp = Builder.CreateSIToFP(sourceOp, llvmIrInstruction->getType());
//						}
//						else if (sourceOp->getType()->isFloatTy() || sourceOp->getType()->isDoubleTy())
//						{
//							// 如果源操作数是浮点类型，转换为整数类型
//							sourceOp = Builder.CreateFPToSI(sourceOp, quantizedType);
//						}
//
//						// 创建新的 UIToFP 指令
//						Value * newInst = Builder.CreateUIToFP(sourceOp, llvmIrInstruction->getType());
//						llvmIrInstruction->replaceAllUsesWith(newInst);
//						llvmIrInstruction->removeFromParent();
//					}
//					break;
//				}


				case Instruction::UIToFP:
				{
					auto sourceOp = llvmIrInstruction->getOperand(0);
					IRBuilder<> Builder(llvmIrInstruction);

					if (sourceOp->getType()->isIntegerTy())
					{
						// 将整数类型转换为浮点类型
						Value * newInst = Builder.CreateSIToFP(sourceOp, llvmIrInstruction->getType());
						llvmIrInstruction->replaceAllUsesWith(newInst);
						llvmIrInstruction->removeFromParent();
					}
					else if (sourceOp->getType()->isFloatTy() || sourceOp->getType()->isDoubleTy())
					{
						// 将浮点类型转换为整数类型
						Value * newInst = Builder.CreateFPToSI(sourceOp, quantizedType);
						llvmIrInstruction->replaceAllUsesWith(newInst);
						llvmIrInstruction->removeFromParent();
					}
					break;
				}


				break;
					//				case Instruction::ZExt:
					//				case Instruction::SExt:
					//				case Instruction::Trunc:
					/*
					 * since the src type changed, adapt the new instruction
					 * */
				case Instruction::FPExt:
				case Instruction::FPTrunc:
				{
					IRBuilder<>   Builder(llvmIrInstruction);
					Instruction * insertPoint = llvmIrInstruction->getNextNode();
					Builder.SetInsertPoint(insertPoint);
					Value * newInst = nullptr;
					if (llvmIrInstruction->getOperand(0)->getType()->isIntegerTy())
					{
						// Handle integer to float cast
						newInst = Builder.CreateSIToFP(
						    llvmIrInstruction->getOperand(0), llvmIrInstruction->getType());
					}
					else
					{
						// Handle float cast
						newInst = Builder.CreateFPCast(
						    llvmIrInstruction->getOperand(0), llvmIrInstruction->getType());
					}
					llvmIrInstruction->replaceAllUsesWith(newInst);
					llvmIrInstruction->removeFromParent();
					break;
				}
				case Instruction::BitCast:
				{
					IRBuilder<>   Builder(llvmIrInstruction);
					Instruction * insertPoint = llvmIrInstruction->getNextNode();
					Builder.SetInsertPoint(insertPoint);
					Value * newInst = Builder.CreateBitCast(
					    llvmIrInstruction->getOperand(0), llvmIrInstruction->getType());
					llvmIrInstruction->replaceAllUsesWith(newInst);
					llvmIrInstruction->removeFromParent();
					break;
				}
			}
		}
	}
	llvm::errs() << "Exiting adaptTypeCast\n";
}
}

void
handleFMul(Instruction * llvmIrInstruction, Type * quantizedType, Function * fixmul, Function * floatIntMul)
{
	llvm::errs() << "Handling FMul\n";
	llvm::errs() << "Original Instruction: " << *llvmIrInstruction << "\n";
	IRBuilder<> Builder(llvmIrInstruction);

	// Ensure operands are correctly converted to fixed-point integers
	Value * lhs = llvmIrInstruction->getOperand(0);
	Value * rhs = llvmIrInstruction->getOperand(1);

	llvm::errs() << "LHS: " << *lhs << "\n";
	llvm::errs() << "RHS: " << *rhs << "\n";

	bool lhsIsFloat = lhs->getType()->isFloatTy() || lhs->getType()->isDoubleTy();
	bool rhsIsFloat = rhs->getType()->isFloatTy() || rhs->getType()->isDoubleTy();

	// If either operand is a float, convert both to fixed-point
	if (lhsIsFloat)
	{
		lhs = Builder.CreateFPToSI(lhs, quantizedType);
		llvm::errs() << "Converted LHS to fixed-point: " << *lhs << "\n";
	}
	if (rhsIsFloat)
	{
		rhs = Builder.CreateFPToSI(rhs, quantizedType);
		llvm::errs() << "Converted RHS to fixed-point: " << *rhs << "\n";
	}

	// Ensure both operands are now integers
	bool lhsIsInteger = lhs->getType()->isIntegerTy();
	bool rhsIsInteger = rhs->getType()->isIntegerTy();

	if (lhsIsInteger && rhsIsInteger)
	{
		// Handle constant simplification if one of the operands is a constant
		if (isa<llvm::Constant>(lhs) || isa<llvm::Constant>(rhs))
		{
			llvm::errs() << "One of the operands is a constant, simplifying...\n";
			simplifyConstant(llvmIrInstruction, quantizedType, floatIntMul);
		}
		else
		{
			llvm::errs() << "Both operands are integers, substituting with fixmul function...\n";
			llvm::CallInst * callInst = Builder.CreateCall(fixmul, {lhs, rhs});
			llvmIrInstruction->replaceAllUsesWith(callInst);
			llvmIrInstruction->eraseFromParent();
		}
	}
	else
	{
		llvm::errs() << "Operands are not both integers, cannot handle FMul\n";
	}

	llvm::errs() << "Finished handling FMul\n";
}

// Main function to perform LLVM IR auto quantization
void
irPassLLVMIRAutoQuantization(State * N, llvm::Function & llvmIrFunction, std::vector<llvm::Function *> & functionsToInsert)
{
	flexprint(N->Fe, N->Fm, N->Fpinfo, "\tauto quantization.\n");
	llvm::errs() << "Entering irPassLLVMIRAutoQuantization\n";

	// Skip certain functions
	std::string functionName = llvmIrFunction.getName().str();
	if (functionName == "llvm.dbg.declare" || functionName == "llvm.dbg.value" || functionName == "llvm.dbg.label" || functionName == "fixmul" || functionName == "floatIntMul")
	// if (functionName == "llvm.dbg.declare" || functionName == "llvm.dbg.value" || functionName == "llvm.dbg.label" || functionName == "fixmul" )
	{
		llvm::errs() << "Skipping function: " << functionName << "\n";
		return;
	}

	Type * quantizedType;
	switch (BIT_WIDTH)
	{
		case 8:
			quantizedType = Type::getInt8Ty(llvmIrFunction.getContext());
			break;
		case 16:
			quantizedType = Type::getInt16Ty(llvmIrFunction.getContext());
			break;
		case 32:
			quantizedType = Type::getInt32Ty(llvmIrFunction.getContext());
			break;
		case 64:
			quantizedType = Type::getInt64Ty(llvmIrFunction.getContext());
			break;
		default:
			flexprint(N->Fe, N->Fm, N->Fperr, "\tunknown int type.\n");
			llvm_unreachable("Unknown bit width for quantization");
			return;
	}

	// Save the parent module
	Module * module = llvmIrFunction.getParent();
	if (!module)
	{
		llvm::errs() << "Error: Function does not have a parent module.\n";
		return;
	}

	// Ensure types are correctly defined
	Type* floatType = Type::getFloatTy(llvmIrFunction.getContext());
	Type* intType = Type::getInt32Ty(llvmIrFunction.getContext());

	// Deal with function signature
	llvm::errs() << "Calling handleFunctionSignature for function: " << llvmIrFunction.getName() << "\n";
	handleFunctionSignature(llvmIrFunction, quantizedType);
	llvm::errs() << "Finished calling handleFunctionSignature for function: " << llvmIrFunction.getName() << "\n";

	/*
	 * generate hardcode function - fixmul and fixdiv
	 * */

	llvm::errs() << "Calling createFixMul for function: " << llvmIrFunction.getName() << "\n";
	llvm::errs() << "inFunction address: " << &llvmIrFunction << "\n";
	llvm::Function * fixmul = createFixMul(module, quantizedType, functionsToInsert);
	llvm::errs() << "Created fixmul function: " << (fixmul ? "Success" : "Failed") << "\n";

	// Create floatIntMul function
	llvm::errs() << "Calling createFloatIntMul for function: " << llvmIrFunction.getName() << "\n";
	//llvm::Function * floatIntMul = createFloatIntMul(llvmIrFunction.getParent(), intType, floatType, functionsToInsert);
	llvm::Function *floatIntMul = createFloatIntMul(module, quantizedType, Type::getFloatTy(llvmIrFunction.getContext()), functionsToInsert);

	llvm::errs()
	    << "Created floatIntMul function: " << (floatIntMul ? "Success" : "Failed") << "\n";

	/*
	 * quantize the arguments type
	 * */

	llvm::errs() << "Quantizing arguments for function: " << llvmIrFunction.getName() << "\n";
	for (size_t idx = 0; idx < llvmIrFunction.arg_size(); idx++)
	{
		llvm::errs() << "Quantizing parameter at index " << idx << "\n";
		llvm::Function::arg_iterator argIt = llvmIrFunction.arg_begin();
		std::advance(argIt, idx);

		if (argIt == llvmIrFunction.arg_end())
		{
			llvm::errs() << "Error: Reached end of arguments while trying to access index " << idx << "\n";
			continue;
		}

		llvm::Argument * paramOp = &*argIt;
		llvm::errs() << "Processing parameter: " << paramOp->getName() << "\n";
		if (!paramOp)
		{
			llvm::errs() << "Error: paramOp is nullptr at index " << idx << "\n";
			continue;
		}

		if (paramOp->getType()->isFloatTy() || paramOp->getType()->isDoubleTy())
		{
			llvm::errs() << "Quantizing parameter from " << *paramOp->getType() << " to " << *quantizedType << "\n";
			setQuantizedType(paramOp, quantizedType);
		}
		else
		{
			llvm::errs() << "Parameter at index " << idx << " is not a floating-point type, skipping\n";
		}
	}

	for (BasicBlock & llvmIrBasicBlock : llvmIrFunction)
	{
		for (BasicBlock::iterator itBB = llvmIrBasicBlock.begin(); itBB != llvmIrBasicBlock.end();)
		{
			Instruction * llvmIrInstruction = &*itBB++;
			llvm::errs() << "Processing instruction in adaptTypeCast: " << *llvmIrInstruction << "\n";

			IRBuilder<> Builder(llvmIrInstruction);
			switch (llvmIrInstruction->getOpcode())
			{
				case Instruction::Alloca:
					if (auto llvmIrAllocaInstruction = dyn_cast<AllocaInst>(llvmIrInstruction))
					{
						auto allocaType = llvmIrAllocaInstruction->getAllocatedType();
						auto newType	= quantizedType;
						if (allocaType->getTypeID() == Type::ArrayTyID)
						{
							newType	   = ArrayType::get(quantizedType,
										    allocaType->getArrayNumElements());
							allocaType = allocaType->getArrayElementType();
						}
						if (allocaType->isDoubleTy() || allocaType->isFloatTy())
						{
							llvmIrAllocaInstruction->setAllocatedType(newType);
						}
						setQuantizedType(llvmIrAllocaInstruction, newType);
					}
					break;
				case Instruction::Call:
					if (auto llvmIrCallInstruction = dyn_cast<CallInst>(llvmIrInstruction))
					{
						Function * calledFunction = llvmIrCallInstruction->getCalledFunction();
						if (calledFunction == nullptr || !calledFunction->hasName() || calledFunction->getName().empty())
							break;
						if (!calledFunction->getName().startswith("llvm.dbg.value") &&
						    !calledFunction->getName().startswith("llvm.dbg.declare") &&
						    !calledFunction->getName().startswith("llvm.dbg.label"))
						{
							if (calledFunction->isDeclaration())
							{
								// For library functions
								IRBuilder<>   Builder(llvmIrCallInstruction);
								Instruction * insertPoint = llvmIrCallInstruction->getNextNode();
								Builder.SetInsertPoint(insertPoint);
								Value * newInst = nullptr;
								if (calledFunction->getName().str() == "sqrt")
								{
									/*
									 * if the arg's type is int, convert to fp,
									 * after the call node, convert to int and shl FRAC_Q/2
									 *
									 * int32_t res = (int32_t)sqrt(x)<<(FRAC_Q/2);
									 * if (FRAC_Q%2)
									 *   return res*1.414213562;
									 * else
									 *   return res;
									 *
									 * %25 = sitofp i32 %0 to double
									 * %26 = call double @sqrt(double %25) #3
									 * %27 = fptosi double %26 to i32
									 * %28 = shl i32 %27, 4
									 * */
									auto operand = llvmIrCallInstruction->getOperand(0);
									if (operand->getType()->isIntegerTy())
									{
										Value * newOperand = Builder.CreateSIToFP(
										    operand, llvmIrCallInstruction->getType());
										llvmIrCallInstruction->setOperand(0, newOperand);
									}
									auto	cloneInst  = llvmIrCallInstruction->clone();
									Value * fptosiInst = Builder.CreateFPToSI(
									    cloneInst, quantizedType);
									Value * shlInst = Builder.CreateShl(fptosiInst, FRAC_Q / 2);
									Value * resInst = nullptr;
									/*
									 * if (FRAC_Q%2) then multiply with 1.414213562;
									 * */
									if (FRAC_Q % 2)
									{
										Value * lhsCompensateInst = Builder.CreateSIToFP(
										    shlInst, llvmIrCallInstruction->getType());
										auto	compensateNum = ConstantFP::get(llvmIrCallInstruction->getType(),
															1.414213562);
										Value * mulInst	      = Builder.CreateFMul(lhsCompensateInst, compensateNum);
										resInst		      = Builder.CreateFPToSI(mulInst, quantizedType);
									}
									else
									{
										resInst = shlInst;
									}
									llvmIrCallInstruction->replaceAllUsesWith(resInst);
									ReplaceInstWithInst(llvmIrCallInstruction, cloneInst);
								}
								else
								{
									/*
									 * for other lib functions, de-quantize the arguments and quantize the return value
									 * */
								}
							}
							else
							{
								/*
								 * for user-defined function, quantize the arguments
								 * */
								for (size_t idx = 0; idx < llvmIrCallInstruction->getNumOperands() - 1; idx++)
								{
									setQuantizedType(llvmIrCallInstruction->getOperand(idx), quantizedType);
								}
								quantizeConstant(llvmIrCallInstruction, quantizedType);
								/*
								 * then quantize the return type
								 * */
								setQuantizedType(llvmIrCallInstruction, quantizedType);
							}
						}
					}
					break;
				case Instruction::GetElementPtr:
					if (auto gepInst = dyn_cast<GetElementPtrInst>(llvmIrInstruction))
					{
						auto gepType	= gepInst->getType();
						auto sourceType = quantizedType;
						//                        bool isPointer = false;
						//                        unsigned pointerAddr = 0;
						//                        if (gepType->isPointerTy()) {
						//                            isPointer = true;
						//                            pointerAddr = gepType->getPointerAddressSpace();
						//                            valueType = gepType->getPointerElementType();
						//                        }
						if (gepInst->getSourceElementType()->getTypeID() == Type::ArrayTyID)
						{
							sourceType = ArrayType::get(quantizedType,
										    gepInst->getSourceElementType()->getArrayNumElements());
						}
						//                        if (isPointer) {
						//                            inValue->mutateType(quantizedType->getPointerTo(pointerAddr));
						//                        }
						//                        if (gepType->isDoubleTy() || gepType->isFloatTy()) {
						gepInst->setSourceElementType(sourceType);
						gepInst->setResultElementType(quantizedType);
						//                        }
					}
					break;
				case Instruction::Load:
				case Instruction::PHI:
				{
					setQuantizedType(llvmIrInstruction, quantizedType);
				}
				break;

				case Instruction::Store:
				{
					/*
					 * If either of the operands is constant, change it to a int value
					 * */
					setQuantizedType(llvmIrInstruction->getOperand(0), quantizedType);
					quantizeConstant(llvmIrInstruction, quantizedType);
				}
				break;

				/*
				 * For fmul/fdiv,
				 *
				 * if either one of the operands is a constant value, simplify it by multiplying with 10^n,
				 * then replace the instruction to mul/div;
				 *
				 * else substitute this instruction to a pre-implemented function: mulfix/divfix.
				 * */
				case Instruction::FMul:
					llvm::errs() << "Found FMul instruction.\n";
					handleFMul(llvmIrInstruction, quantizedType, fixmul, floatIntMul);
					break;

				case Instruction::FDiv:
				{
					if (isa<llvm::Constant>(llvmIrInstruction->getOperand(0)) ||
					    isa<llvm::Constant>(llvmIrInstruction->getOperand(1)))
					{
						simplifyConstant(llvmIrInstruction, quantizedType, floatIntMul);
					}
					else
					{
						substituteHardcodeFunc(llvmIrInstruction, quantizedType, fixmul);
					}
					break;
				}

				/*
				 * If either one of the operands is a constant value, quantize it,
				 * then replace the instruction to the int version.
				 * */
				case Instruction::FCmp:
				case Instruction::FAdd:
				case Instruction::FSub:
				case Instruction::FRem:
				{
					quantizeConstant(llvmIrInstruction, quantizedType);
				}
				case Instruction::FNeg:
				{
					quantizeSimpleFPInstruction(llvmIrInstruction, quantizedType);
					break;
				}

					//                case Instruction::Add:
					//                case Instruction::Sub:
					//                case Instruction::Mul:
					//                case Instruction::UDiv:
					//				case Instruction::SDiv:
					//                case Instruction::URem:
					//                case Instruction::SRem:
					//
					//				case Instruction::Shl:
					//				case Instruction::LShr:
					//				case Instruction::AShr:
					//				case Instruction::And:
					//				case Instruction::Or:
					//				case Instruction::Xor:
					//
					//                case Instruction::ICmp:

				case Instruction::FPToUI:
				case Instruction::FPToSI:
				case Instruction::SIToFP:
				case Instruction::UIToFP:
				case Instruction::ZExt:
				case Instruction::SExt:
				case Instruction::Trunc:
				case Instruction::FPExt:
				case Instruction::FPTrunc:
				case Instruction::BitCast:
					break;

				case Instruction::Ret:
				case Instruction::Switch:
				case Instruction::Br:
				case Instruction::Select:
				case Instruction::IndirectBr:
				case Instruction::Invoke:
				case Instruction::Resume:
				case Instruction::Unreachable:
				case Instruction::CleanupRet:
				case Instruction::CatchRet:
				case Instruction::CatchSwitch:
				case Instruction::CallBr:
				case Instruction::Fence:
				case Instruction::AtomicCmpXchg:
				case Instruction::AtomicRMW:
				case Instruction::PtrToInt:
				case Instruction::IntToPtr:
				case Instruction::AddrSpaceCast:
				case Instruction::CleanupPad:
				case Instruction::CatchPad:
				case Instruction::UserOp1:
				case Instruction::UserOp2:
				case Instruction::VAArg:
				case Instruction::ExtractElement:
				case Instruction::InsertElement:
				case Instruction::ShuffleVector:
				case Instruction::ExtractValue:
				case Instruction::InsertValue:
				case Instruction::LandingPad:
				case Instruction::Freeze:
					break;
				default:
					break;
			}
		}
	}
	handleLoadStoreInstructions(llvmIrFunction, quantizedType);
	adaptTypeCast(llvmIrFunction, quantizedType);

	return;
}