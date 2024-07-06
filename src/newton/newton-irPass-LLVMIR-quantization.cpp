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
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/ValueMapper.h"
#include "llvm/ADT/SmallVector.h"
#include <unordered_map>
using namespace llvm;

#define FRAC_Q 16
#define FRAC_BASE (1 << FRAC_Q)
#define BIT_WIDTH 32

extern "C" {

//check if the type is a floating type
bool isValidFloatingType(Type *type) {
	return type->isFloatTy() || type->isDoubleTy();
}



// Set the quantized type for a given value
void setQuantizedType(Value * inValue, Type * quantizedType) {
	auto valueType = inValue->getType();
	unsigned pointerAddr;
	bool	 isPointer = false;
	if (valueType != nullptr)
	{
		if (valueType->isPointerTy()) {
			isPointer = true;
			pointerAddr = valueType->getPointerAddressSpace();
			valueType = valueType->getPointerElementType();
		}
		if (valueType->isDoubleTy() || valueType->isFloatTy() || valueType->isArrayTy())
		{
			if (isPointer) {
				inValue->mutateType(quantizedType->getPointerTo(pointerAddr));
			} else {
				inValue->mutateType(quantizedType);
			}
		}
	}
}

// Quantize constants within an instruction
std::unordered_map<ConstantFP *, Value *> quantizedValueCache;

// Ensure load and store instructions have matching types
void handleLoadStoreInstructions(Function &llvmIrFunction, Type *quantizedType) {
	llvm::errs() << "Entering handleLoadStoreInstructions\n";
	for (BasicBlock &llvmIrBasicBlock : llvmIrFunction) {
		for (BasicBlock::iterator itBB = llvmIrBasicBlock.begin(); itBB != llvmIrBasicBlock.end();) {
			Instruction *llvmIrInstruction = &*itBB++;
			if (llvmIrInstruction->getOpcode() == Instruction::Load) {
				if (auto loadInst = dyn_cast<LoadInst>(llvmIrInstruction)) {
					auto ptr = loadInst->getPointerOperand();
					if (ptr->getType()->getPointerElementType()->isFloatTy() ||
					    ptr->getType()->getPointerElementType()->isDoubleTy()) {
						ptr->mutateType(quantizedType->getPointerTo());
					}
				}
			} else if (llvmIrInstruction->getOpcode() == Instruction::Store) {
				if (auto storeInst = dyn_cast<StoreInst>(llvmIrInstruction)) {
					auto ptr = storeInst->getPointerOperand();
					if (ptr->getType()->getPointerElementType()->isFloatTy() ||
					    ptr->getType()->getPointerElementType()->isDoubleTy()) {
						ptr->mutateType(quantizedType->getPointerTo());
					}
				}
			}
		}
	}
}

/*Function *replaceFunctionSignature(Function &llvmIrFunction, Type *quantizedType) {
	auto &context = llvmIrFunction.getContext();
	std::vector<Type*> paramTypes;
	for (auto &arg : llvmIrFunction.args()) {
		if (arg.getType()->isFloatTy() || arg.getType()->isDoubleTy()) {
			paramTypes.push_back(quantizedType);
		} else {
			paramTypes.push_back(arg.getType());
		}
	}

	Type *returnType = llvmIrFunction.getReturnType()->isFloatTy() || llvmIrFunction.getReturnType()->isDoubleTy()
				? quantizedType
				: llvmIrFunction.getReturnType();

	FunctionType *newFuncType = FunctionType::get(returnType, paramTypes, llvmIrFunction.isVarArg());
	Function *newFunction = Function::Create(newFuncType, llvmIrFunction.getLinkage(), llvmIrFunction.getName(), llvmIrFunction.getParent());

	// Copy the function body to the new function
	newFunction->getBasicBlockList().splice(newFunction->begin(), llvmIrFunction.getBasicBlockList());

	// Update the arguments mapping
	auto newArgIter = newFunction->arg_begin();
	for (auto &arg : llvmIrFunction.args()) {
		arg.replaceAllUsesWith(&*newArgIter);
		newArgIter->takeName(&arg);
		++newArgIter;
	}

	return newFunction;
}*/



// Function to create a new function with quantized return type and parameters
llvm::Function* createNewFunctionWithQuantizedType(llvm::Function &oldFunction, Type *quantizedType) {
	// Create a new function type with quantized arguments and return type
	std::vector<Type*> paramTypes;
	for (auto &arg : oldFunction.args()) {
		if (arg.getType()->isFloatTy() || arg.getType()->isDoubleTy()) {
			paramTypes.push_back(quantizedType);
		} else {
			paramTypes.push_back(arg.getType());
		}
	}

	// Determine the return type for the new function
	Type *returnType = oldFunction.getReturnType();
	if (returnType->isFloatTy() || returnType->isDoubleTy()) {
		returnType = quantizedType;
	}
	// Create a new function type with the quantized return type and parameters
	llvm::FunctionType *newFuncType = llvm::FunctionType::get(returnType, paramTypes, oldFunction.isVarArg());
	// Create a new function with the new function type in the same module as the old function
	llvm::Function *newFunction = llvm::Function::Create(newFuncType, oldFunction.getLinkage(), oldFunction.getName(), oldFunction.getParent());


	return newFunction;
}


// Function to rename the old function to avoid name conflict
void renameOldFunction(llvm::Function &oldFunction) {
	std::string oldFunctionName = oldFunction.getName().str();
	oldFunction.setName(oldFunctionName + "_old");
}

// Function to replace all calls to the old function with calls to the new function
void replaceCallsToOldFunction(llvm::Function &oldFunction, llvm::Function &newFunction) {
	std::vector<llvm::CallInst*> callsToReplace;

	for (auto &use : oldFunction.uses()) {
		if (auto *call = llvm::dyn_cast<llvm::CallInst>(use.getUser())) {
			callsToReplace.push_back(call);
		}
	}

	// Replace each call to the old function with a call to the new function
	for (auto *call : callsToReplace) {
		llvm::SmallVector<llvm::Value*, 8> args(call->arg_begin(), call->arg_end());
		llvm::IRBuilder<> builder(call);
		llvm::CallInst *newCall = builder.CreateCall(&newFunction, args);
		call->replaceAllUsesWith(newCall);
		call->eraseFromParent();
	}
}



// Quantize constants within an instruction

void quantizeConstant(Instruction *inInstruction, Type *quantizedType) {
	llvm::errs() << "Entering quantizeConstant\n";
	for (size_t idx = 0; idx < inInstruction->getNumOperands(); idx++) {
		Value *inValue = inInstruction->getOperand(idx);


		// Skip if the operand is not a floating-point constant
		if (!isa<llvm::ConstantFP>(inValue)) {
			continue;
		}


		auto *constFp = llvm::dyn_cast<ConstantFP>(inValue);


		// Check the cache for the quantized value
		auto cachedValue = quantizedValueCache.find(constFp);

		if (cachedValue != quantizedValueCache.end()) {
			inInstruction->replaceUsesOfWith(inValue, cachedValue->second);
			continue;
		}

		// Compute the quantized value if not found in cache
		Value *newValue = nullptr;
		if (inValue->getType()->isFloatTy()) {
			// Convert float constant to fixed-point
			float constValue = constFp->getValueAPF().convertToFloat();
			constValue *= FRAC_BASE;
			int32_t fixedPointValue = round(constValue);
			//newValue = ConstantInt::get(quantizedType, round(constValue), true);
			newValue = ConstantFP::get(inValue->getType(), (float)fixedPointValue / FRAC_BASE);
		} else if (inValue->getType()->isDoubleTy()) {
			// Convert double constant to fixed-point and back to double
			double constValue = constFp->getValueAPF().convertToDouble();
			constValue *= FRAC_BASE;
			int64_t fixedPointValue = round(constValue);
			//newValue = ConstantInt::get(quantizedType, round(constValue), true);
			newValue = ConstantFP::get(inValue->getType(), (double)fixedPointValue / FRAC_BASE);
		} else {
			llvm::errs() << "Unknown floating type: " << *inValue->getType() << "\n";
			//assert(false && "unknown floating type");
			continue;
		}

		// Cache the quantized value
		quantizedValueCache[constFp] = newValue;

		// Replace all uses of the original value with the new quantized value
		inInstruction->replaceUsesOfWith(inValue, newValue);
	}
}
/*void
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

		ConstantFP * constFp = llvm::dyn_cast<llvm::ConstantFP>(inValue);


		Value * newValue = nullptr;
		if (inValue->getType()->isFloatTy())
		{
			// Convert float constant to fixed-point
			float constValue = constFp->getValueAPF().convertToFloat();
			constValue *= FRAC_BASE;
			newValue = ConstantInt::get(quantizedType, round(constValue), true);
		}
		else if (inValue->getType()->isDoubleTy())
		{
			// Convert double constant to fixed-point
			double constValue = constFp->getValueAPF().convertToDouble();
			constValue *= FRAC_BASE;
			newValue = ConstantInt::get(quantizedType, round(constValue), true);
		}
		else
		{
			// assert(false && "unknown floating type");
			llvm::errs() << "Unknown floating type\n";
			continue;
		}

		// Replace all uses of the original value with the new quantized value
		inInstruction->replaceUsesOfWith(inValue, newValue);
	}
}*/

// Simplify constants in the instruction
void
simplifyConstant(Instruction * inInstruction, Type * quantizedType)
{
	llvm::errs() << "Entering simplifyConstant\n";
	auto checkDecimal = [](float decimalNum) {
		int digits = 0;
		/*
		 * Since the max value of `int16` is 32767,
		 * we maximum multiply with 1,000 to make sure it won't exceed max_int16
		 * */

		// Ensure the decimal number can be represented within the fixed-point range
		while (fabs(round(decimalNum) - decimalNum) > 0.001 && digits < 4)
		{
			decimalNum *= 10;  // Scale decimal to avoid precision issues
			digits++;
		}
		return decimalNum;
	};

	auto compensateFP = [inInstruction, quantizedType](float quantizedNum, float decimalNum) {
		/*
		 * 3333.3 / 3.3333 = 1000
		 * ===>
		 * Example 1:
		 * a * 3.3333 ~= a * (3333 / 1000) ~= (int)a * 3333 / 1000
		 *
		 * Example 2:
		 * a / 3.3333 ~= a / (3333 / 1000) ~= (int)a * 1000 / 3333
		 *
		 * Example 3:
		 * 3.3333 / a ~= (3333 / 1000) / a ~= 3333 / (int)a / 1000
		 * */
		float compensateNum = quantizedNum / decimalNum;

		Value *	 constOperand, *nonConstOperand;
		unsigned constIdx, nonConstIdx;
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

		auto quantizeNumValue = ConstantInt::get(quantizedType, round(quantizedNum), true);

		if (compensateNum == 1)
		{
			// If the compensation factor is 1, directly set the quantized value
			inInstruction->setOperand(constIdx, quantizeNumValue);
		}
		else
		{
			// Otherwise, apply compensation to the fixed-point arithmetic
			auto compensateNumValue = ConstantInt::get(quantizedType, round(compensateNum), true);

			IRBuilder<>   Builder(inInstruction);
			Instruction * insertPoint = inInstruction->getNextNode();
			Builder.SetInsertPoint(insertPoint);
			Value * newFisrtInst  = nullptr;
			Value * newSecondInst = nullptr;
			auto	instOpCode    = inInstruction->getOpcode();
			if (instOpCode == Instruction::FMul)
			{
				// Replace floating-point multiplication with fixed-point equivalent
				newFisrtInst  = Builder.CreateMul(nonConstOperand, quantizeNumValue);
				newSecondInst = Builder.CreateSDiv(newFisrtInst, compensateNumValue);
			}
			else if (instOpCode == Instruction::FDiv && isa<llvm::Constant>(inInstruction->getOperand(1)))
			{
				// Replace floating-point division with fixed-point equivalent (divisor is constant)
				newFisrtInst  = Builder.CreateMul(nonConstOperand, compensateNumValue);
				newSecondInst = Builder.CreateSDiv(newFisrtInst, quantizeNumValue);
			}
			else if (instOpCode == Instruction::FDiv && isa<llvm::Constant>(inInstruction->getOperand(0)))
			{
				// Handle fixed-point division (constant dividend)
				newFisrtInst  = Builder.CreateSDiv(quantizeNumValue, nonConstOperand);
				newSecondInst = Builder.CreateSDiv(newFisrtInst, compensateNumValue);
			}

			inInstruction->replaceAllUsesWith(newSecondInst);
			inInstruction->removeFromParent();
		}
	};

	for (size_t idx = 0; idx < inInstruction->getNumOperands(); idx++)
	{
		Value * inValue = inInstruction->getOperand(idx);

		if (!isa<llvm::ConstantFP>(inValue))
		{
			// Skip non-floating-point constants
			continue;
		}

		ConstantFP * constFp  = llvm::dyn_cast<llvm::ConstantFP>(inValue);
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
}

// Create a fixed-point multiplication function
llvm::Function *
createFixMul(llvm::Function * inFunction, Type * quantizedType, std::vector<llvm::Function *> & functionsToInsert)
{
	llvm::errs() << "Entering createFixMul\n";
	/*
	 * check if this function is exist
	 * */
	std::string fixmulFuncName = "fixmul";
	auto	    irModule	   = inFunction->getParent();
	for (auto & function : *irModule)
	{
		if (function.getName() == fixmulFuncName)
		{
			return &function;
		}
	}

	llvm::FunctionType * funcType = llvm::FunctionType::get(quantizedType, {quantizedType, quantizedType}, false);
	llvm::Function *     func     = llvm::Function::Create(funcType, llvm::Function::PrivateLinkage, fixmulFuncName, irModule);

	llvm::BasicBlock * entryBB = llvm::BasicBlock::Create(inFunction->getContext(), "entry", func);
	llvm::IRBuilder<>  builder(entryBB);
	builder.SetInsertPoint(entryBB);

	/*
	 * ((int64_t)x*y)>>FRAC_Q
	 *
	 * ===========>
	 *
	 * define private i32 @mulfix(i32 %0, i32 %1) {
	 * %3 = sext i32 %0 to i64
	 * %4 = sext i32 %1 to i64
	 * %5 = mul nsw i64 %3, %4
	 * %6 = ashr i64 %5, 8
	 * %7 = trunc i64 %6 to i32
	 * ret i32 %7
	 * }
	 * */
	Type * higherQuantizedType;
	switch (BIT_WIDTH)
	{
		case 8:
			higherQuantizedType = Type::getInt16Ty(inFunction->getContext());
			break;
		case 16:
			higherQuantizedType = Type::getInt32Ty(inFunction->getContext());
			break;
		default:
			higherQuantizedType = Type::getInt64Ty(inFunction->getContext());
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

	return func;
}

// Substitute hardcoded functions like multiplication with fixed-point versions
void
substituteHardcodeFunc(Instruction * inInstruction, Type * quantizedType, llvm::Function * func)
{
	llvm::errs() << "Entering substituteHardcodeFunc\n";
	IRBuilder<>   Builder(inInstruction);
	Instruction * insertPoint = inInstruction->getNextNode();
	Builder.SetInsertPoint(insertPoint);
	//    Value * newInst = nullptr;

	llvm::CallInst * callInst = Builder.CreateCall(func, {inInstruction->getOperand(0), inInstruction->getOperand(1)});
	//    InlineFunctionInfo inlineFuncInfo;
	//    llvm::InlineFunction(*callInst, inlineFuncInfo);

	inInstruction->replaceAllUsesWith(callInst);
	inInstruction->removeFromParent();
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



std::unordered_set<llvm::Instruction*> processedInstructions;
void
quantizeSimpleFPInstruction(Instruction * inInstruction, Type * quantizedType)
{
	llvm::errs() << "Entering quantizeSimpleFPInstruction\n";
	if (!inInstruction)
	{
		llvm::errs() << "inInstruction is nullptr\n";
		return;
	}


	// Check if the instruction has already been processed
	if (processedInstructions.find(inInstruction) != processedInstructions.end()) {
		llvm::errs() << "Instruction already processed: " << *inInstruction << "\n";
		return;
	}
	processedInstructions.insert(inInstruction);


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
		case Instruction::FSub:
		{
			llvm::errs() << "Handling FSub\n";
			Value * op0 = inInstruction->getOperand(0);
			Value * op1 = inInstruction->getOperand(1);
			if (op0->getType()->isFloatTy() || op0->getType()->isDoubleTy())
			{
				op0 = Builder.CreateFPToSI(op0, quantizedType);
			}
			if (op1->getType()->isFloatTy() || op1->getType()->isDoubleTy())
			{
				op1 = Builder.CreateFPToSI(op1, quantizedType);
			}
			newInst = Builder.CreateSub(op0, op1);
			break;
		}

		case Instruction::FMul:
		{
			llvm::errs() << "Handling FMul\n";
			// Replace floating-point multiplication with integer multiplication
			newInst = Builder.CreateMul(inInstruction->getOperand(0), inInstruction->getOperand(1));
			break;
		}
		case Instruction::FDiv:
		{
			llvm::errs() << "Handling FDiv\n";
			// Replace floating-point division with integer division
			newInst = Builder.CreateSDiv(inInstruction->getOperand(0), inInstruction->getOperand(1));
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
			auto constZero = ConstantInt::get(quantizedType, 0, true);
			newInst	       = Builder.CreateSub(constZero, inInstruction->getOperand(0));
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
				case Instruction::UIToFP:
				{
					auto sourceOp = llvmIrInstruction->getOperand(0);
					if (sourceOp->getType() == llvmIrInstruction->getType())
					{
						// Replace with source if types match
						llvmIrInstruction->replaceAllUsesWith(sourceOp);
						llvmIrInstruction->removeFromParent();
					}
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

std::unordered_set<std::string> processedFunctions;
// Main function to perform LLVM IR auto quantization
//void irPassLLVMIRAutoQuantization(State * N, llvm::Function & llvmIrFunction, std::vector<llvm::Function *> & functionsToInsert)	{
void irPassLLVMIRAutoQuantization(State *N, llvm::Function &llvmIrFunction, std::vector<llvm::Function*>& functionsToInsert) {


		flexprint(N->Fe, N->Fm, N->Fpinfo, "\tauto quantization.\n");
		llvm::errs() << "Entering irPassLLVMIRAutoQuantization\n";

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

		if (processedFunctions.find(llvmIrFunction.getName().str()) != processedFunctions.end()) {
			llvm::errs() << "Function already processed: " << llvmIrFunction.getName() << "\n";
			return;
		}
		processedFunctions.insert(llvmIrFunction.getName().str());

	/*	// Create a new function with the desired function type
		llvm::Function * newFunction = createNewFunctionWithQuantizedType(llvmIrFunction, quantizedType);

		// Handle Load and Store instructions in the old function
		handleLoadStoreInstructions(llvmIrFunction, quantizedType);

		// Replace all calls to the old function with calls to the new function
		replaceCallsToOldFunction(llvmIrFunction, *newFunction);

		// Rename the old function to avoid name conflict
		renameOldFunction(llvmIrFunction);

		// Adapt type casts in the old function
		adaptTypeCast(llvmIrFunction, quantizedType);

		// Replace all uses of the old function with the new function
		llvmIrFunction.replaceAllUsesWith(newFunction);*/
		// TODO

		/*
		* generate hardcode function - fixmul and fixdiv
		* */

		//llvm::Function *fixmul = createFixMul(newFunction, quantizedType, functionsToInsert);
		llvm::Function * fixmul = createFixMul(&llvmIrFunction, quantizedType, functionsToInsert);

		/*
* quantize the arguments type
* */
		/*		for (size_t idx = 0; idx < llvmIrFunction.arg_size(); idx++)
{
auto paramOp = llvmIrFunction.getArg(idx);
setQuantizedType(paramOp, quantizedType);
}*/


/*		// Continue with the rest of your quantization logic...
		for (llvm::BasicBlock &llvmIrBasicBlock : *newFunction) {
			for (llvm::BasicBlock::iterator itBB = llvmIrBasicBlock.begin(); itBB != llvmIrBasicBlock.end();) {
				llvm::Instruction *llvmIrInstruction = &*itBB++;
				static std::unordered_set<llvm::Instruction*> processedInstructions;
				if (processedInstructions.find(llvmIrInstruction) != processedInstructions.end()) {
					continue;
				}
				processedInstructions.insert(llvmIrInstruction);*/

					for (BasicBlock & llvmIrBasicBlock : llvmIrFunction)
{
for (BasicBlock::iterator itBB = llvmIrBasicBlock.begin(); itBB != llvmIrBasicBlock.end();)
{
Instruction * llvmIrInstruction = &*itBB++;
llvm::errs() << "Processing instruction in adaptTypeCast: " << *llvmIrInstruction << "\n";
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
					case Instruction::FDiv:
					{
						if (isa<llvm::Constant>(llvmIrInstruction->getOperand(0)) ||
						    isa<llvm::Constant>(llvmIrInstruction->getOperand(1)))
						{
							simplifyConstant(llvmIrInstruction, quantizedType);
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

