#include "llvm/IR/IRBuilder.h"
#include "newton-irPass-LLVMIR-quantization.h"
#include "llvm/Support/raw_ostream.h"
#include <unordered_map>
#include <llvm-14/llvm/IR/Value.h>
#include "llvm/IR/Metadata.h"
using namespace llvm;

unsigned int FRAC_Q;
#define FRAC_BASE (1 << FRAC_Q)
#define BIT_WIDTH 32




// 64bit fixed point multiplication
llvm::Value *
performFixedPointMul(llvm::IRBuilder<> & Builder, llvm::Value * lhs, llvm::Value * rhs, unsigned FRAC_Q)
{
	// Sign extend the 32-bit operands to 64-bit integers
	llvm::Value * lhs64 = Builder.CreateSExt(lhs, llvm::Type::getInt64Ty(Builder.getContext()));
	llvm::Value * rhs64 = Builder.CreateSExt(rhs, llvm::Type::getInt64Ty(Builder.getContext()));

	// Perform 64-bit multiplication
	llvm::Value * mulResult64 = Builder.CreateNSWMul(lhs64, rhs64);

	// Right shift the result to simulate fixed-point division by FRAC_Q
	llvm::Value * divResult64 = Builder.CreateLShr(mulResult64, llvm::ConstantInt::get(llvm::Type::getInt64Ty(Builder.getContext()), FRAC_Q));

	// Truncate the 64-bit result back to 32-bit integer
	llvm::Value * result32 = Builder.CreateTrunc(divResult64, llvm::Type::getInt32Ty(Builder.getContext()));

	return result32;
}


extern "C" {
// TODO : float version rsqrt
llvm::Function *
createFixSqrt(llvm::Module * irModule, Type * quantizedType, std::vector<llvm::Function *> & functionsToInsert)
{
	if (!irModule)
	{
		llvm::errs() << "Error: irModule is nullptr\n";
		return nullptr;
	}

	std::string fixSqrtFuncName = "fixsqrt";
	for (auto & function : *irModule)
	{
		if (function.getName() == fixSqrtFuncName)
		{
			llvm::errs() << "fixsqrt already exists\n";
			return &function;
		}
	}

	llvm::LLVMContext &  context  = irModule->getContext();
	llvm::FunctionType * funcType = llvm::FunctionType::get(quantizedType, {quantizedType}, false);
	llvm::Function *     func     = llvm::Function::Create(funcType, llvm::Function::PrivateLinkage, fixSqrtFuncName, irModule);
	llvm::BasicBlock *   entryBB  = llvm::BasicBlock::Create(context, "entry", func);
	llvm::IRBuilder<>    builder(entryBB);

	llvm::Function::arg_iterator args = func->arg_begin();
	llvm::Value *		     x	  = &*args++;

	// Convert the fixed-point integer to a floating-point number for sqrt computation
	llvm::Value * fp_x = builder.CreateSIToFP(x, llvm::Type::getFloatTy(context));

	// Call sqrt on the floating-point value
	llvm::Function * sqrtFunc   = llvm::Intrinsic::getDeclaration(irModule, llvm::Intrinsic::sqrt, llvm::Type::getFloatTy(context));
	llvm::Value *	 sqrtResult = builder.CreateCall(sqrtFunc, {fp_x});

	// Convert the result back to a fixed-point integer
	llvm::Value * res = builder.CreateFPToSI(sqrtResult, quantizedType);

	// Perform a left shift to scale the result
	llvm::Value * shlRes = builder.CreateShl(res, FRAC_Q / 2);

	// Apply compensation if FRAC_Q is odd
	llvm::Value * finalRes = shlRes;
	if (FRAC_Q % 2 != 0)
	{
		llvm::Value * compensationFactor = llvm::ConstantFP::get(llvm::Type::getFloatTy(context), 1.414213562);
		llvm::Value * fpShlRes		 = builder.CreateSIToFP(shlRes, llvm::Type::getFloatTy(context));
		llvm::Value * compensated	 = builder.CreateFMul(fpShlRes, compensationFactor);
		finalRes			 = builder.CreateFPToSI(compensated, quantizedType);
	}

	builder.CreateRet(finalRes);
	functionsToInsert.push_back(func);
	llvm::errs() << "Created fixsqrt function: " << func->getName() << "\n";

	return func;
}


// TODO 64 bit version of fixmul
llvm::Function *
createFixMul(Module * irModule, Type * quantizedType, std::vector<llvm::Function *> & functionsToInsert)
{
	llvm::errs() << "Entering createFixMul\n";

	// Check if irModule is valid
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

	// Create fixed-point multiplication instruction
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

	llvm::Function::arg_iterator arg1    = &*(func->arg_begin());
	llvm::Value *		     sext1   = builder.CreateSExt(arg1, higherQuantizedType);
	llvm::Function::arg_iterator arg2    = &*(++arg1);
	llvm::Value *		     sext2   = builder.CreateSExt(arg2, higherQuantizedType);
	llvm::Value *		     mulInst = builder.CreateNSWMul(sext1, sext2);
	// llvm::Value * mulInst	= builder.CreateMul(sext1, sext2);
	llvm::Value * ashrInst	= builder.CreateLShr(mulInst, ConstantInt::get(higherQuantizedType, FRAC_Q));
	llvm::Value * truncInst = builder.CreateTrunc(ashrInst, quantizedType);
	builder.CreateRet(truncInst);

	functionsToInsert.emplace_back(func);
	llvm::errs() << "Created fixmul function: " << func->getName() << "\n";
	return func;
}

// TODO  Original version of fixrsqrt
llvm::Function *
createFixRsqrt(llvm::Module * irModule, Type * quantizedType, std::vector<llvm::Function *> & functionsToInsert)
{
	llvm::errs() << "Entering createFixRsqrt\n";

	// Check if irModule is valid
	if (!irModule)
	{
		llvm::errs() << "Error: irModule is nullptr\n";
		return nullptr;
	}

	std::string fixrsqrtFuncName = "fixrsqrt";
	for (auto & function : *irModule)
	{
		if (function.getName() == fixrsqrtFuncName)
		{
			llvm::errs() << "fixrsqrt already exists\n";
			return &function;
		}
	}

	// Define the function type for fixrsqrt: int32_t fixrsqrt(int32_t x)
	llvm::FunctionType * funcType = llvm::FunctionType::get(quantizedType, {quantizedType}, false);
	// Create the function and insert it into the module
	llvm::Function * func = llvm::Function::Create(funcType, llvm::Function::PrivateLinkage, "fixrsqrt", irModule);

	llvm::BasicBlock * entryBB = llvm::BasicBlock::Create(irModule->getContext(), "entry", func);
	llvm::IRBuilder<>  builder(entryBB);

	// Get the function argument (x)
	llvm::Function::arg_iterator args = func->arg_begin();
	llvm::Value *		     x	  = &*args++;

	// Create the fixed-point multiplication function
	llvm::Function * fixMulFunc = createFixMul(irModule, quantizedType, functionsToInsert);


	// Step 1: int_halfx = mulfix(0.5 * FRAC_BASE, x);
	llvm::Value * halfBase = builder.CreateLShr(x, ConstantInt::get(quantizedType, 1));

	// Step 2: Convert x to floating-point and perform the initial approximation
	llvm::Value * fp_y = builder.CreateSIToFP(x, llvm::Type::getFloatTy(irModule->getContext()));

	// Added step: fp_y = fp_y / FRAC_BASE.0;

	fp_y = builder.CreateFMul(fp_y, ConstantFP::get(llvm::Type::getFloatTy(irModule->getContext()), 1.0f / FRAC_BASE));
	//performFixedPointMul


	llvm::Value * i = builder.CreateBitCast(fp_y, llvm::Type::getInt32Ty(irModule->getContext()));
	i		= builder.CreateNSWSub(ConstantInt::get(llvm::Type::getInt32Ty(irModule->getContext()), 0x5f3759df), builder.CreateLShr(i, 1));
	// i    = builder.CreateSub(ConstantInt::get(llvm::Type::getInt32Ty(irModule->getContext()), 0x5f3759df), builder.CreateLShr(i, 1));
	fp_y = builder.CreateBitCast(i, llvm::Type::getFloatTy(irModule->getContext()));

	// Step 3: int_y = fp_y * FRAC_BASE;
	llvm::Value * int_y = builder.CreateFPToSI(builder.CreateFMul(fp_y, ConstantFP::get(llvm::Type::getFloatTy(irModule->getContext()), FRAC_BASE)), quantizedType);

	// Step 4: int_y = mulfix(int_y, ((int32_t)(1.5f * FRAC_BASE) - (mulfix(mulfix(int_halfx, int_y), int_y))));
	//llvm::Value * mulfix1	 = builder.CreateCall(fixMulFunc, {halfBase, int_y});
	//performFixedPointMul
	llvm::Value * mulfix1	 = performFixedPointMul(builder, halfBase, int_y, FRAC_Q);
	//llvm::Value * mulfix2	 = builder.CreateCall(fixMulFunc, {mulfix1, int_y});
	llvm::Value * mulfix2	 = performFixedPointMul(builder, mulfix1, int_y, FRAC_Q);
	llvm::Value * correction = builder.CreateSub(ConstantInt::get(quantizedType, static_cast<int>(1.5f * FRAC_BASE)), mulfix2);
	//llvm::Value * final_y	 = builder.CreateCall(fixMulFunc, {int_y, correction});
	llvm::Value * final_y	 = performFixedPointMul(builder, int_y, correction, FRAC_Q);

	// Return the final fixed-point result
	builder.CreateRet(final_y);
	functionsToInsert.emplace_back(func);

	return func;
}

// A list of global variables to erase after processing
std::vector<GlobalVariable *> globalsToErase;
void
eraseOldGlobals()
{
	std::set<llvm::GlobalVariable *> uniqueGlobals(globalsToErase.begin(), globalsToErase.end());
	for (auto * global : uniqueGlobals)
	{
		if (global)
		{
			llvm::errs() << "Erasing old global variable: " << global->getName() << "\n";
			global->eraseFromParent();
		}
		else
		{
			llvm::errs() << "Skipping null global variable\n";
		}
	}
	globalsToErase.clear();
}

// A list of functions to erase after processing
std::vector<Function *> functionsToErase;

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

void
updateGlobalVariables(Module * module, Type * quantizedType)
{
	llvm::errs() << "Updating global variables\n";

	for (GlobalVariable & globalVar : module->globals())
	{
		if (globalVar.getType()->getElementType()->isFloatTy() || globalVar.getType()->getElementType()->isDoubleTy())
		{
			llvm::errs() << "Quantizing global variable: " << globalVar.getName() << "\n";

			// Ensure the new global is dso_local by specifying correct linkage
			GlobalValue::LinkageTypes linkage = GlobalValue::InternalLinkage;

			// Create the new integer type pointer
			Type * newType = quantizedType->getPointerTo();

			// Update the initializer of the global variable
			if (llvm::Constant * init = globalVar.getInitializer())
			{
				if (llvm::ConstantFP * constFp = llvm::dyn_cast<llvm::ConstantFP>(init))
				{
					double	value	       = constFp->getValueAPF().convertToDouble();
					int64_t quantizedValue = static_cast<int64_t>(round(value * FRAC_BASE));
					globalVar.setInitializer(llvm::ConstantInt::get(quantizedType, quantizedValue));
				}
			}

			// Check if a quantized version of the global variable already exists
			std::string quantizedName = globalVar.getName().str() + "_quantized";
			if (GlobalVariable * existingGlobalVar = module->getNamedGlobal(quantizedName))
			{
				// Replace all uses of the old global variable with the existing quantized one
				globalVar.replaceAllUsesWith(existingGlobalVar);
			}
			else
			{
				// Create a new global variable with the updated type and initializer
				GlobalVariable * newGlobalVar = new GlobalVariable(
				    *module,
				    quantizedType,
				    globalVar.isConstant(),
				    globalVar.getLinkage(),
				    globalVar.getInitializer(),
				    quantizedName);
				newGlobalVar->setAlignment(llvm::MaybeAlign(4));
				newGlobalVar->setDSOLocal(true);

				// Replace all uses of the old global variable with the new one
				globalVar.replaceAllUsesWith(newGlobalVar);
			}

			// Add the old global variable to the list of globals to erase
			globalsToErase.push_back(&globalVar);
		}
	}
}

// void
// handleLoad(Instruction * llvmIrInstruction, Type * quantizedType)
//{
//	if (auto * loadInst = dyn_cast<LoadInst>(llvmIrInstruction))
//	{
//		IRBuilder<> Builder(loadInst);
//
//		// Check if the loaded value is of floating-point type
//		Type * loadedType = loadInst->getType();
//		// Type * pointerType = loadInst->getPointerOperandType();
//
//		if (loadedType->isFloatingPointTy())
//		{
//			llvm::errs() << "Handling normal load instruction\n";
//			// Create a new load instruction with the original pointer operand
//			LoadInst * newLoadInst = Builder.CreateLoad(quantizedType, loadInst->getPointerOperand(), loadInst->getName() + ".orig");
//
//			// Replace all uses of the original load instruction with the new converted value
//			loadInst->replaceAllUsesWith(newLoadInst);
//
//			// Schedule the original load instruction for removal
//			loadInst->eraseFromParent();
//
//			llvm::errs() << "Replaced floating-point load with quantized integer load.\n";
//		}
//	}
// }

void
quantizePointer(LoadInst * loadInst, IRBuilder<> & Builder, Type * quantizedType, Type * loadedType)
{
	Value * pointerOperand = loadInst->getPointerOperand();
	llvm::errs() << "Quantizing load from local pointer: " << *pointerOperand << "\n";

	Value * loadedValue = Builder.CreateLoad(loadedType, pointerOperand, loadInst->getName() + ".p");

	Value * scaledValue = Builder.CreateFMul(loadedValue, ConstantFP::get(loadedType, FRAC_BASE), loadInst->getName() + ".scaled_ptr");

	Value * quantizedValue = Builder.CreateFPToSI(scaledValue, quantizedType, loadInst->getName() + ".quantized_ptr");

	loadInst->replaceAllUsesWith(quantizedValue);
	loadInst->eraseFromParent();

	llvm::errs() << "Replaced load with quantized integer value.\n";
}

void
handleLoad(Instruction * llvmIrInstruction, Type * quantizedType)
{
	if (auto * loadInst = dyn_cast<LoadInst>(llvmIrInstruction))
	{
		IRBuilder<> Builder(loadInst);
		Type *	    loadedType = loadInst->getType();
		if (loadedType->isFloatingPointTy())
		{
			Value *	   pointerOperand = loadInst->getPointerOperand();
			Function * parentFunc	  = loadInst->getFunction();

			if (isa<GlobalVariable>(pointerOperand) || parentFunc->getName() == "MadgwickAHRSupdateIMU")
			{
				llvm::errs() << "Handling load from global variable: " << *pointerOperand << "\n";
				LoadInst * newLoadInst = Builder.CreateLoad(quantizedType, pointerOperand, loadInst->getName() + ".global_quantized");
				llvm::errs() << "New load instruction: " << *newLoadInst << "\n";
				loadInst->replaceAllUsesWith(newLoadInst);
				loadInst->eraseFromParent();
			}

			//			if (parentFunc->getName() == "MadgwickAHRSupdateIMU")
			//			{
			//				llvm::errs() << "Handling load for MadgwickAHRSupdateIMU: " << *pointerOperand << "\n";
			//				LoadInst *newLoadInst = Builder.CreateLoad(quantizedType, pointerOperand, loadInst->getName() + ".global_quantized");
			//				llvm ::errs() << "New load instruction: " << *newLoadInst << "\n";
			//				loadInst->replaceAllUsesWith(newLoadInst);
			//				loadInst->eraseFromParent();
			//			}

			else if (!isa<GlobalVariable>(pointerOperand))
			{
				quantizePointer(loadInst, Builder, quantizedType, loadedType);
			}
		}
	}
}

void
handleFAdd(Instruction * inInstruction, Type * quantizedType)
{
	llvm::errs() << "Handling FAdd\n";
	IRBuilder<> Builder(inInstruction);

	Value * op0 = inInstruction->getOperand(0);
	Value * op1 = inInstruction->getOperand(1);

	// Check if this instruction has metadata indicating it's quantized
	if (inInstruction->getMetadata("quantized"))
	{
		llvm::errs() << "Skipping quantized instruction\n";
		return;
	}

	// Create fixed-point addition
	Value * newInst = Builder.CreateNSWAdd(op0, op1);
	// Value * newInst = Builder.CreateAdd(op0, op1);

	// Replace the original FAdd instruction with the new fixed-point addition
	inInstruction->replaceAllUsesWith(newInst);
	inInstruction->eraseFromParent();

	llvm::errs() << "Finished handling FAdd\n";
}

void
handleFSub(Instruction * inInstruction, Type * quantizedType)
{
	llvm::errs() << "Handling FSub\n";
	IRBuilder<> Builder(inInstruction);

	Value * op0 = inInstruction->getOperand(0);
	Value * op1 = inInstruction->getOperand(1);

	// Check if one of the operands is a floating-point constant that needs to be multiplied by FRAC_BASE
	if (ConstantFP * constFp = dyn_cast<ConstantFP>(op0))
	{
		// Multiply the constant by FRAC_BASE and convert to integer
		float	constValue     = constFp->getValueAPF().convertToFloat();
		int64_t quantizedValue = static_cast<int64_t>(round(constValue * FRAC_BASE));
		op0		       = ConstantInt::get(quantizedType, quantizedValue);
	}

	if (ConstantFP * constFp = dyn_cast<ConstantFP>(op1))
	{
		// Multiply the constant by FRAC_BASE and convert to integer
		float	constValue     = constFp->getValueAPF().convertToFloat();
		int64_t quantizedValue = static_cast<int64_t>(round(constValue * FRAC_BASE));
		op1		       = ConstantInt::get(quantizedType, quantizedValue);
	}

	// Create fixed-point subtraction
	Value * newInst = Builder.CreateNSWSub(op0, op1);
	// Value * newInst = Builder.CreateSub(op0, op1);

	// Replace the original FSub instruction with the new fixed-point subtraction
	inInstruction->replaceAllUsesWith(newInst);
	inInstruction->eraseFromParent();

	llvm::errs() << "Finished handling FSub\n";
}

void
handleFNeg(Instruction * inInstruction, Type * quantizedType)
{
	llvm::errs() << "Handling FNeg\n";
	IRBuilder<> Builder(inInstruction);

	// Get the operand for the FNeg operation
	Value * operand = inInstruction->getOperand(0);
	llvm::errs() << "Operand: " << *operand << "\n";

	// Create a constant zero of the same integer type as the operand
	Value * zero = ConstantInt::get(operand->getType(), 0);

	// Perform the integer subtraction: sub <type> 0, operand
	Value * newInst = Builder.CreateSub(zero, operand);

	// Replace the original FNeg instruction with the new subtraction
	inInstruction->replaceAllUsesWith(newInst);
	inInstruction->eraseFromParent();

	llvm::errs() << "Finished handling FNeg with integer subtraction\n";
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
		default:
			llvm::errs() << "Unhandled floating point predicate\n";
			return ICmpInst::ICMP_EQ;  // Default to equal for safety
	}
}

void
handleFCmp(Instruction * inInstruction, Type * quantizedType)
{
	IRBuilder<> Builder(inInstruction);

	if (auto fcmp_inst = dyn_cast<FCmpInst>(inInstruction))
	{
		CmpInst::Predicate pred = quantizePredict(fcmp_inst->getPredicate());
		if (fcmp_inst->getPredicate() == FCmpInst::FCMP_TRUE)
		{
			Value * newInst = ConstantInt::getTrue(quantizedType);
			inInstruction->replaceAllUsesWith(newInst);
		}
		else if (fcmp_inst->getPredicate() == FCmpInst::FCMP_FALSE)
		{
			Value * newInst = ConstantInt::getFalse(quantizedType);
			inInstruction->replaceAllUsesWith(newInst);
		}
		else
		{
			Value * fpOp0 = fcmp_inst->getOperand(0);
			Value * fpOp1 = fcmp_inst->getOperand(1);

			Value * intOp0 = Builder.CreateFPToSI(fpOp0, quantizedType);
			Value * intOp1 = Builder.CreateFPToSI(fpOp1, quantizedType);

			Value * newInst = Builder.CreateICmp(pred, intOp0, intOp1);
			inInstruction->replaceAllUsesWith(newInst);
		}
		inInstruction->eraseFromParent();
	}
}

void
simplifyConstant(Instruction * inInstruction, Type * quantizedType)
{
	llvm::errs() << "Handling constant operand\n";
	auto checkDecimal = [](float decimalNum) {
		int digits = 0;
		/*
		 * Since the max value of `int16` is 32767,
		 * we maximum multiply with 1,000 to make sure it won't exceed max_int16
		 * */
		while (fabs(round(decimalNum) - decimalNum) > 0.001 && digits < 4)
		{
			decimalNum *= 10;
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

		IRBuilder<>   Builder(inInstruction);
		Instruction * insertPoint = inInstruction->getNextNode();
		Builder.SetInsertPoint(insertPoint);
		Value * newFisrtInst  = nullptr;
		Value * newSecondInst = nullptr;
		auto	instOpCode    = inInstruction->getOpcode();

		if (compensateNum == 1)
		{
			llvm::errs() << "Compensation factor is 1, directly setting the quantized value\n";
			// newFisrtInst = Builder.CreateMul(nonConstOperand, quantizeNumValue);
			inInstruction->setOperand(constIdx, quantizeNumValue);
			//			inInstruction->replaceAllUsesWith(newSecondInst);
			//			//inInstruction->removeFromParent();
			//			instructionsToErase.push_back(inInstruction);
		}
		else
		{
			llvm::errs() << "Applying compensation to the fixed-point arithmetic\n";
			auto compensateNumValue = ConstantInt::get(quantizedType, round(compensateNum), true);

			IRBuilder<>   Builder(inInstruction);
			Instruction * insertPoint = inInstruction->getNextNode();
			Builder.SetInsertPoint(insertPoint);
			Value * newFisrtInst  = nullptr;
			Value * newSecondInst = nullptr;
			auto	instOpCode    = inInstruction->getOpcode();
			if (instOpCode == Instruction::FMul)
			{
				llvm::errs() << "Handling FMul instruction\n";
				newFisrtInst  = Builder.CreateMul(nonConstOperand, quantizeNumValue);
				newSecondInst = Builder.CreateSDiv(newFisrtInst, compensateNumValue);
			}
			else if (instOpCode == Instruction::FDiv && isa<llvm::Constant>(inInstruction->getOperand(1)))
			{
				llvm::errs() << "Handling FDiv instruction with constant denominator\n";
				newFisrtInst  = Builder.CreateMul(nonConstOperand, compensateNumValue);
				newSecondInst = Builder.CreateSDiv(newFisrtInst, quantizeNumValue);
			}
			else if (instOpCode == Instruction::FDiv && isa<llvm::Constant>(inInstruction->getOperand(0)))
			{
				llvm::errs() << "Handling FDiv instruction with constant numerator\n";
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
	llvm::errs() << "Exiting Simplifying Constant\n";
}
}
void
simplifyUsingShift(Value * operand, Instruction * instruction, int shiftAmount, bool isRightShift)
{
	llvm::IRBuilder<> Builder(instruction);
	Type *		  intType    = Type::getInt32Ty(instruction->getContext());
	Value *		  shiftValue = ConstantInt::get(intType, shiftAmount);
	Instruction *	  shiftInst;

	if (isRightShift)
	{
		shiftInst = BinaryOperator::CreateAShr(operand, shiftValue, "", instruction);
	}
	else
	{
		shiftInst = BinaryOperator::CreateShl(operand, shiftValue, "", instruction);
	}

	instruction->replaceAllUsesWith(shiftInst);
	instruction->eraseFromParent();
}

bool
checkAndSimplifyForConstant(ConstantFP * constFP, Value * otherOperand, Instruction * instruction)
{
	static const std::map<double, std::pair<int, bool>> constantsMap = {
	    {0.5, {1, true}},
	    {2.0, {1, false}},
	    {4.0, {2, false}},
	    {8.0, {3, false}},
	    {1 / 128.0, {7, true}}};
	double value = constFP->getValueAPF().convertToDouble();
	auto   it    = constantsMap.find(value);
	if (it != constantsMap.end())
	{
		int  shiftAmount  = it->second.first;
		bool isRightShift = it->second.second;
		simplifyUsingShift(otherOperand, instruction, shiftAmount, isRightShift);
		return true;
	}
	return false;
}



void
handleFMul(Instruction * llvmIrInstruction, Type * quantizedType)
{
	llvm::errs() << "Handling FMul\n";
	llvm::errs() << "Original Instruction: " << *llvmIrInstruction << "\n";
	IRBuilder<> Builder(llvmIrInstruction);

	// Check if this instruction has metadata indicating it's quantized
	if (llvmIrInstruction->getMetadata("quantized"))
	{
		llvm::errs() << "Skipping already quantized instruction.\n";
		return;	 // Skip processing this instruction
	}

	// Ensure operands are correctly converted to fixed-point integers
	Value * lhs = llvmIrInstruction->getOperand(0);
	Value * rhs = llvmIrInstruction->getOperand(1);

	llvm::errs() << "LHS: " << *lhs << "\n";
	llvm::errs() << "RHS: " << *rhs << "\n";

	bool lhsIsFloat = lhs->getType()->isFloatTy() || lhs->getType()->isDoubleTy();
	bool rhsIsFloat = rhs->getType()->isFloatTy() || rhs->getType()->isDoubleTy();

	ConstantFP * lhsConst = dyn_cast<ConstantFP>(lhs);
	ConstantFP * rhsConst = dyn_cast<ConstantFP>(rhs);

	// Check if either operand is a constant that can be simplified
	if (auto rhsConst = dyn_cast<ConstantFP>(rhs))
	{
		if (checkAndSimplifyForConstant(rhsConst, lhs, llvmIrInstruction))
			return;
	}
	if (auto lhsConst = dyn_cast<ConstantFP>(lhs))
	{
		if (checkAndSimplifyForConstant(lhsConst, rhs, llvmIrInstruction))
			return;
	}

	// If either operand is a float constant, convert it to fixed-point using handleConstant
	if (isa<ConstantFP>(lhs))
	{
		llvm::errs() << "LHS is a floating-point constant, handling it with handleConstant\n";
		// handleConstant(llvmIrInstruction, quantizedType);
		simplifyConstant(llvmIrInstruction, quantizedType);
		lhs	   = llvmIrInstruction->getOperand(0);
		lhsIsFloat = false;  // Update float status as it's now a fixed-point
	}

	if (isa<ConstantFP>(rhs))
	{
		llvm::errs() << "RHS is a floating-point constant, handling it with handleConstant\n";
		// handleConstant(llvmIrInstruction, quantizedType);
		simplifyConstant(llvmIrInstruction, quantizedType);
		rhsIsFloat = false;
		rhs	   = llvmIrInstruction->getOperand(1);	// Update rhs after conversion
	}

	// If either operand is an integer constant, directly use mul
	if (isa<ConstantInt>(lhs) || isa<ConstantInt>(rhs))
	{
		llvm::errs() << "One of the operands is an integer constant, using mul\n";
		Value * newInst = Builder.CreateMul(lhs, rhs);
		// Value * newInst = Builder.CreateNSWMul(lhs, rhs);
		llvmIrInstruction->replaceAllUsesWith(newInst);
		llvmIrInstruction->eraseFromParent();
		return;
	}

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

	// If both operands are integers, but neither is a constant, use fixmul
	bool lhsIsInteger = lhs->getType()->isIntegerTy();
	bool rhsIsInteger = rhs->getType()->isIntegerTy();

	//	if (lhsIsInteger && rhsIsInteger)

	if (lhsIsInteger && rhsIsInteger)

	// fixmul
	//			{
	//				llvm::errs() << "Both operands are integers, substituting with fixmul function...\n";
	//				llvm::CallInst * callInst = Builder.CreateCall(fixmul, {lhs, rhs});
	//				llvmIrInstruction->replaceAllUsesWith(callInst);
	//				llvmIrInstruction->eraseFromParent();
	//			}
	// 64bit
	{
		llvm::Value * newInst = performFixedPointMul(Builder, lhs, rhs, FRAC_Q);
		llvmIrInstruction->replaceAllUsesWith(newInst);
		llvmIrInstruction->eraseFromParent();
	}

	// 32bit
	//			{
	//				llvm::errs() << "Both operands are integers, performing  multiplication and shifting...\n";
	//
	//				// Perform multiplication directly
	//				llvm::Value * mulResult = Builder.CreateMul(lhs, rhs, "");
	//
	//				// Perform right arithmetic shift
	//				llvm::Value * shiftResult = Builder.CreateLShr(mulResult, llvm::ConstantInt::get(lhs->getType(), FRAC_Q));
	//
	//				// Replace all uses of the original instruction with the result of the shift
	//				llvmIrInstruction->replaceAllUsesWith(shiftResult);
	//				llvmIrInstruction->eraseFromParent();
	//			}

	llvm::errs() << "Finished handling FMul\n";
}

void
setQuantizedType(Value * inValue, Type * quantizedType)
{
	auto	 valueType = inValue->getType();
	unsigned pointerAddr;
	bool	 isPointer = false;
	if (valueType != nullptr)
	{
		if (valueType->isPointerTy())
		{
			llvm::errs() << "Pointer type detected\n";
			isPointer   = true;
			pointerAddr = valueType->getPointerAddressSpace();
			valueType   = valueType->getPointerElementType();
		}
		if (valueType->isDoubleTy() || valueType->isFloatTy() || valueType->isArrayTy())
		{
			if (isPointer)
			{
				llvm::errs() << "Original type: " << *valueType << "\n";
				llvm::errs() << "New quantized type: " << *quantizedType << "\n";
				inValue->mutateType(quantizedType->getPointerTo(pointerAddr));
			}
			else
			{
				inValue->mutateType(quantizedType);
			}
		}
	}
}

void
handleAlloca(AllocaInst * llvmIrAllocaInstruction, Type * quantizedType)
{
	auto   allocaType = llvmIrAllocaInstruction->getAllocatedType();
	Type * newType	  = quantizedType;

	llvm::errs() << "Handling Alloca for variable: " << llvmIrAllocaInstruction->getName() << "\n";
	llvm::errs() << " - Original type: " << *allocaType << "\n";

	if (allocaType->getTypeID() == Type::ArrayTyID)
	{
		newType	   = ArrayType::get(quantizedType, allocaType->getArrayNumElements());
		allocaType = allocaType->getArrayElementType();
		llvm::errs() << " - Detected array type. New quantized array type: " << *newType << "\n";
	}

	if (allocaType->isDoubleTy() || allocaType->isFloatTy())
	{
		llvmIrAllocaInstruction->setAllocatedType(newType);
		llvm::errs() << " - Updated allocation type to quantized type: " << *newType << "\n";
	}
	else
	{
		llvm::errs() << " - No need to change type for non-floating point allocation.\n";
	}
}

void
handleStore(Instruction * llvmIrInstruction, Type * quantizedType)
{
	if (auto llvmIrStoreInstruction = dyn_cast<StoreInst>(llvmIrInstruction))
	{
		IRBuilder<> Builder(llvmIrStoreInstruction);

		auto valueType = llvmIrStoreInstruction->getValueOperand()->getType();
		if (valueType->isFloatTy() || valueType->isDoubleTy())
		{
			llvm::errs() << "Original store value type: " << *valueType << "\n";
			llvm::errs() << "New quantized store value type: " << *quantizedType << "\n";

			// Quantize the value operand
			auto quantizedValue = Builder.CreateFMul(llvmIrStoreInstruction->getValueOperand(), ConstantFP::get(valueType, FRAC_BASE));
			quantizedValue	    = Builder.CreateFPToSI(quantizedValue, quantizedType);
			llvmIrStoreInstruction->setOperand(0, quantizedValue);
		}

		auto pointerType = llvmIrStoreInstruction->getPointerOperand()->getType()->getPointerElementType();
		if (pointerType->isFloatTy() || pointerType->isDoubleTy())
		{
			llvm::errs() << "Original store pointer type: " << *pointerType << "\n";
			llvm::errs() << "New quantized store pointer type: " << *quantizedType << "\n";

			// Set the new quantized type for the pointer operand
			llvmIrStoreInstruction->getPointerOperand()->mutateType(quantizedType->getPointerTo());
		}
	}
}

void
bitcastFloatPtrArgs(Function & F, IRBuilder<> & Builder)
{
	// Check if the function is the specific one to be skipped
	if (F.getName() == "MadgwickAHRSupdateIMU")
	{
		llvm::errs() << "Skipping bitcast for function: " << F.getName() << "\n";
		return;	 // Early exit if it's the function to skip
	}

	SmallVector<std::pair<Argument *, Value *>, 4> argReplacements;
	// Iterate over all function arguments
	for (Argument & Arg : F.args())
	{
		// Check if the argument is a pointer to float
		if (Arg.getType()->isPointerTy() && Arg.getType()->getPointerElementType()->isFloatTy())
		{
			llvm::PointerType * i32PtrType = llvm::Type::getInt32PtrTy(F.getContext());
			// Create a bitcast instruction at the beginning of the function
			Builder.SetInsertPoint(&*F.getEntryBlock().getFirstInsertionPt());
			Value * newArg = Builder.CreateBitCast(&Arg, i32PtrType, Arg.getName() + ".to_i32_ptr");
			// llvm::Value *newArg = Builder.CreateBitCast(&Arg, i32PtrType, Arg.getName() + ".toI32Ptr");

			// Store the original argument and the bitcast result
			argReplacements.push_back({&Arg, newArg});
		}
	}
	llvm::errs() << "Starting use replacement\n";  // Log added

	// Iterate over the function to replace uses of the original arguments
	for (auto & replacement : argReplacements)
	{
		Argument * oldArg = replacement.first;
		Value *	   newArg = replacement.second;

		// Replace all uses of the old argument with the new bitcasted value
		for (auto & replacement : argReplacements)
		{
			Argument * oldArg = replacement.first;
			Value *	   newArg = replacement.second;

			// 替换前，移除 bitcast 指令自身的使用
			SmallVector<Use *, 8> usesToReplace;
			for (auto & U : oldArg->uses())
			{
				User * user = U.getUser();

				// Skip if the use is a Load instruction
				if (isa<LoadInst>(user))
				{
					llvm::errs() << "Skipping load instruction: " << *user << "\n";
					continue;  // 跳过 Load 指令
				}

				if (user != newArg)
				{
					usesToReplace.push_back(&U);
				}
			}

			for (auto * use : usesToReplace)
			{
				use->set(newArg);
			}
		}
	}
}

void
handleRsqrtCall(CallInst * llvmIrCallInstruction, Type * quantizedType, Function * fixrsqrt)
{
	IRBuilder<> builder(llvmIrCallInstruction);
	auto	    operand = llvmIrCallInstruction->getOperand(0);

	if (!fixrsqrt)
	{
		llvm::errs() << "Error: fixrsqrt function is null.\n";
		return;
	}

	CallInst * rsqrtResult = builder.CreateCall(fixrsqrt, {operand});

	llvmIrCallInstruction->replaceAllUsesWith(rsqrtResult);

	llvmIrCallInstruction->eraseFromParent();
}

/*
 * // Call sqrt on the floating-point value
llvm::Function * sqrtFunc   = llvm::Intrinsic::getDeclaration(irModule, llvm::Intrinsic::sqrt, llvm::Type::getFloatTy(context));
llvm::Value *	 sqrtResult = builder.CreateCall(sqrtFunc, {fp_x});

// Convert the result back to a fixed-point integer
llvm::Value * res = builder.CreateFPToSI(sqrtResult, quantizedType);

// Perform a left shift to scale the result
llvm::Value * shlRes = builder.CreateShl(res, FRAC_Q / 2);

// Apply compensation if FRAC_Q is odd
llvm::Value * finalRes = shlRes;
if (FRAC_Q % 2 != 0)
{
llvm::Value * compensationFactor = llvm::ConstantFP::get(llvm::Type::getFloatTy(context), 1.414213562);
llvm::Value * fpShlRes		 = builder.CreateSIToFP(shlRes, llvm::Type::getFloatTy(context));
llvm::Value * compensated	 = builder.CreateFMul(fpShlRes, compensationFactor);
finalRes			 = builder.CreateFPToSI(compensated, quantizedType);
}

 */

llvm::Value *
performFixedPointSqrt(IRBuilder<> &builder, Module *irModule, Value *fixedPointValue, Type *quantizedType, int FRAC_Q, LLVMContext &context)
{
	// Convert the fixed-point value to floating-point for sqrt calculation
	llvm::Value *fpValue = builder.CreateSIToFP(fixedPointValue, llvm::Type::getFloatTy(context));

	// Call sqrt on the floating-point value
	llvm::Function *sqrtFunc = llvm::Intrinsic::getDeclaration(irModule, llvm::Intrinsic::sqrt, llvm::Type::getFloatTy(context));
	llvm::Value *sqrtResult  = builder.CreateCall(sqrtFunc, {fpValue});

	// Convert the result back to a fixed-point integer
	llvm::Value *res = builder.CreateFPToSI(sqrtResult, quantizedType);

	// Perform a left shift to scale the result (FRAC_Q / 2)
	llvm::Value *shlRes = builder.CreateShl(res, FRAC_Q / 2);

	// Initialize the final result as the shifted result
	llvm::Value *finalRes = shlRes;

	// Apply compensation if FRAC_Q is odd
	if (FRAC_Q % 2 != 0)
	{
		// Compensation factor for odd FRAC_Q (1.414213562 ≈ sqrt(2))
		llvm::Value *compensationFactor = llvm::ConstantFP::get(llvm::Type::getFloatTy(context), 1.414213562f);

		// Convert the shifted result back to float
		llvm::Value *fpShlRes = builder.CreateSIToFP(shlRes, llvm::Type::getFloatTy(context));

		// Multiply by the compensation factor to adjust for odd FRAC_Q
		llvm::Value *compensated = builder.CreateFMul(fpShlRes, compensationFactor);

		// Convert the compensated value back to a fixed-point integer
		finalRes = builder.CreateFPToSI(compensated, quantizedType);
	}

	return finalRes;

}


//void
//handleSqrtCall(CallInst * llvmIrCallInstruction, Type * quantizedType)
//{
//	IRBuilder<> Builder(llvmIrCallInstruction);
//	auto	    operand = llvmIrCallInstruction->getOperand(0);
//
//	// Convert the operand to fixed-point format if necessary
//	if (operand->getType()->isFloatingPointTy())
//	{
//		operand = Builder.CreateFPToSI(operand, quantizedType);
//	}
//
//	// Create call to the fixed-point sqrt function
//	//llvm::Value * sqrtResult = Builder.CreateCall(fixsqrt, {operand});
//	//手动写减少call overhead
//	llvm::Value * sqrtResult = performFixedPointSqrt(Builder, llvmIrCallInstruction->getModule(), operand, quantizedType, FRAC_Q, llvmIrCallInstruction->getContext());
//	// No need to apply shl and compensation if it's already done in createFixSqrt
//	llvmIrCallInstruction->replaceAllUsesWith(sqrtResult);
//	llvmIrCallInstruction->eraseFromParent();
//}

void
handleSqrtCall(CallInst * llvmIrCallInstruction, Type * quantizedType, Function * fixsqrt)
{
	IRBuilder<> Builder(llvmIrCallInstruction);
	auto	    operand = llvmIrCallInstruction->getOperand(0);

	// Ensure operand is in the correct type
	if (operand->getType()->isFloatingPointTy())
	{
		operand = Builder.CreateFPToSI(operand, quantizedType);
	}

	// Create call to the fixed-point sqrt function
	llvm::Value * sqrtResult = Builder.CreateCall(fixsqrt, {operand});

	// Replace the original instruction with the new fixed-point sqrt result
	llvmIrCallInstruction->replaceAllUsesWith(sqrtResult);
	llvmIrCallInstruction->eraseFromParent();
}

void

//handleCall(CallInst * llvmIrCallInstruction, Type * quantizedType, std::vector<llvm::Function *> & functionsToInsert,  llvm::Function * fixrsqrt)
handleCall(CallInst * llvmIrCallInstruction, Type * quantizedType, std::vector<llvm::Function *> & functionsToInsert, llvm::Function * fixsqrt, llvm::Function * fixrsqrt)
{
	llvm::errs() << "Handling Call\n";
	Function * calledFunction = llvmIrCallInstruction->getCalledFunction();
	if (calledFunction == nullptr || !calledFunction->hasName() || calledFunction->getName().empty())
		return;

	std::string funcName = calledFunction->getName().str();

	if (funcName == "invSqrt")
	{
		IRBuilder<>   Builder(llvmIrCallInstruction);
		Instruction * insertPoint = llvmIrCallInstruction->getNextNode();
		Builder.SetInsertPoint(insertPoint);
		Value * newInst = nullptr;
		llvm::errs() << "Handling invsqrt call\n";
		handleRsqrtCall(llvmIrCallInstruction, quantizedType, fixrsqrt);

		// Add @invSqrt function to functionsToErase if it is no longer used
		if (calledFunction->use_empty())  // Check if the function has no more uses
		{
			functionsToErase.push_back(calledFunction);  // Add it to the list for later erasure
		}
	}

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
			if (funcName == "sqrt" || funcName == "sqrtf")
			{
				// For sqrt
				//handleSqrtCall(llvmIrCallInstruction, quantizedType);
				handleSqrtCall(llvmIrCallInstruction, quantizedType, fixsqrt);
				if (calledFunction->use_empty())
				{
					functionsToErase.push_back(calledFunction);
				}
			}
			else
			{
				/*
				 * for other lib functions, de-quantize the arguments and quantize the return value
				 */
				// dequantizeArgumentsAndQuantizeReturn(llvmIrCallInstruction, quantizedType);
			}
		}
		else
		{
			// Quantize the return type if necessary
			if (llvmIrCallInstruction->getType()->isPointerTy())
			{
				Type * returnElementType = llvmIrCallInstruction->getType()->getPointerElementType();
				if (returnElementType->isFloatTy() || returnElementType->isDoubleTy())
				{
					// Keep the pointer type, but track the need for de-quantization
					return;
				}
			}
			setQuantizedType(llvmIrCallInstruction, quantizedType);
		}
	}
}

// Helper function to quantize floating-point parameters
void
quantizeFunctionArguments(llvm::Function & func, llvm::IRBuilder<> & builder)
{
	// Skip the function if it is MadgwickAHRSupdateIMU
	if (func.getName() == "MadgwickAHRSupdateIMU")
	{
		llvm::errs() << "Skipping quantization for: " << func.getName() << "\n";
		return;
	}

	builder.SetInsertPoint(&func.getEntryBlock(), func.getEntryBlock().begin());

	for (auto & arg : func.args())
	{
		if (arg.getType()->isFloatingPointTy())
		{
			std::vector<Use *> usesToReplace;
			for (auto & use : arg.uses())
			{
				usesToReplace.push_back(&use);
			}

			// Create multiplication and rounding instructions
			llvm::Instruction * scaled    = cast<llvm::Instruction>(builder.CreateFMul(&arg, llvm::ConstantFP::get(arg.getContext(), llvm::APFloat((float)FRAC_BASE)), arg.getName() + ".scaled"));
			llvm::Instruction * rounded   = cast<llvm::Instruction>(builder.CreateFAdd(scaled, llvm::ConstantFP::get(arg.getContext(), llvm::APFloat(0.5f)), arg.getName() + ".rounded"));
			llvm::Instruction * quantized = cast<llvm::Instruction>(builder.CreateFPToSI(rounded, llvm::Type::getInt32Ty(arg.getContext()), arg.getName() + ".changed"));

			// Attach metadata to each instruction
			llvm::MDNode * metadataNode = llvm::MDNode::get(arg.getContext(), llvm::MDString::get(arg.getContext(), "quantized"));
			scaled->setMetadata("quantized", metadataNode);
			rounded->setMetadata("quantized", metadataNode);
			quantized->setMetadata("quantized_changed", metadataNode);

			// Replace all occurrences of the original argument with the new quantized value
			for (Use * use : usesToReplace)
			{
				Use & u = *use;
				u.set(quantized);
			}
			// arg.replaceAllUsesWith(quantized);

			llvm::errs() << "Quantizing argument: " << arg.getName() << "\n";
			llvm::errs() << " - Scaled value: " << *scaled << "\n";
			llvm::errs() << " - Rounded value: " << *rounded << "\n";
			llvm::errs() << " - Quantized value: " << *quantized << "\n";
		}
	}
}

bool
shouldSkipFunction(const std::string & functionName)
{
	// List of function names to skip
	static const std::unordered_set<std::string> skipFunctions = {
	    "llvm.dbg.declare",
	    "llvm.dbg.value",
	    "llvm.dbg.label",
	    "invSqrt",
	    "fixsqrt",
	    "fixrsqrt",
	    "fixmul",
	    "llvm.sqrt.f64",
	    "llvm.sqrt.f32",
	    "sqrt",
	    "sqrtf"};

	return skipFunctions.find(functionName) != skipFunctions.end();
}

void
quantizeArguments(llvm::Function & llvmIrFunction, llvm::Type * quantizedType)
{
	// Check if the function is specifically MadgwickAHRSupdateIMU
	if (llvmIrFunction.getName() == "MadgwickAHRSupdateIMU")
	{
		llvm::errs() << "Quantizing arguments for function: " << llvmIrFunction.getName() << "\n";

		// Process each argument of the function
		for (int idx = 0; idx < llvmIrFunction.arg_size(); idx++)
		{
			auto * paramOp = llvmIrFunction.getArg(idx);
			setQuantizedType(paramOp, quantizedType);  // Assuming setQuantizedType is defined elsewhere
		}
	}
}

void
adaptTypeCast(llvm::Function & llvmIrFunction, Type * quantizedType)
{
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
						newInst = Builder.CreateSIToFP(
						    llvmIrInstruction->getOperand(0), llvmIrInstruction->getType());
					}
					else
					{
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
}

// Main function to perform LLVM IR auto quantization
void
irPassLLVMIRAutoQuantization(State * N, llvm::Function & llvmIrFunction, std::vector<llvm::Function *> & functionsToInsert, int maxPrecisionBits)
{
	FRAC_Q = maxPrecisionBits;
	flexprint(N->Fe, N->Fm, N->Fpinfo, "\tauto quantization.\n");
	llvm::errs() << "Entering irPassLLVMIRAutoQuantization\n";

	// Usage in the original function
	std::string functionName = llvmIrFunction.getName().str();
	if (shouldSkipFunction(functionName))
	{
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

	// Iterate over the function's arguments to apply quantization
	llvm::IRBuilder<> builder(llvmIrFunction.getContext());
	quantizeFunctionArguments(llvmIrFunction, builder);
	quantizeArguments(llvmIrFunction, quantizedType);

	// Perform bitcasting of float poiter arguments to i32*
	bitcastFloatPtrArgs(llvmIrFunction, builder);

	// Update global variables to integer type
	updateGlobalVariables(module, quantizedType);

	/*
	 * generate hardcode function
	 * */
	//llvm::Function * fixmul	  = createFixMul(module, quantizedType, functionsToInsert);
	llvm::Function * fixsqrt  = createFixSqrt(module, quantizedType, functionsToInsert);
	llvm::Function * fixrsqrt = createFixRsqrt(module, quantizedType, functionsToInsert);

	for (BasicBlock & llvmIrBasicBlock : llvmIrFunction)
	{
		for (BasicBlock::iterator itBB = llvmIrBasicBlock.begin(); itBB != llvmIrBasicBlock.end();)
		{
			Instruction * llvmIrInstruction = &*itBB++;

			//			if (llvmIrInstruction->getMetadata("quantized_changed"))
			//			{
			//				llvm::errs() << "quantized_changed.\n";
			//				return;	 // Skip processing this instruction
			//			}

			llvm::errs() << "Processing instruction in main: " << *llvmIrInstruction << "\n";
			switch (llvmIrInstruction->getOpcode())
			{
				case Instruction::Alloca:
					handleAlloca(cast<AllocaInst>(llvmIrInstruction), quantizedType);
					break;
				case Instruction::Call:
					//handleCall(cast<CallInst>(llvmIrInstruction), quantizedType, functionsToInsert,  fixrsqrt);
					handleCall(cast<CallInst>(llvmIrInstruction), quantizedType, functionsToInsert, fixsqrt, fixrsqrt);
					break;
				case Instruction::GetElementPtr:
				case Instruction::Load:
				{
					llvm::errs() << "Handling load\n";
					handleLoad(llvmIrInstruction, quantizedType);
					break;
				}
				break;
				case Instruction::PHI:
				{
					setQuantizedType(llvmIrInstruction, quantizedType);
				}
				break;

				case Instruction::Store:
					/*
					 * If either of the operands is constant, change it to a int value
					 * */
					llvm::errs() << "handle store " << *llvmIrInstruction << "\n";
					// setQuantizedType(llvmIrInstruction->getOperand(0), quantizedType);
					break;

				case Instruction::FMul:
				{
					handleFMul(llvmIrInstruction, quantizedType);
					break;
				}

					/*
					 * If either one of the operands is a constant value, quantize it,
					 * then replace the instruction to the int version.
					 * */
				case Instruction::FDiv:

				case Instruction::FCmp:
					handleFCmp(llvmIrInstruction, quantizedType);
					break;
				case Instruction::FAdd:
				{
					handleFAdd(llvmIrInstruction, quantizedType);
					break;
				}
				case Instruction::FSub:
				{
					handleFSub(llvmIrInstruction, quantizedType);
					break;
				}
				case Instruction::FRem:
				case Instruction::FNeg:
				{
					handleFNeg(llvmIrInstruction, quantizedType);
					break;
				}

				case Instruction::FPToUI:
				case Instruction::FPToSI:
				case Instruction::SIToFP:
				case Instruction::UIToFP:

				case Instruction::ZExt:
				case Instruction::SExt:
				case Instruction::Trunc:
				case Instruction::FPExt:
				case Instruction::FPTrunc:
					//				{
					//					IRBuilder<>   Builder(llvmIrInstruction);
					//					Instruction * insertPoint = llvmIrInstruction->getNextNode();
					//					Builder.SetInsertPoint(insertPoint);
					//					Value * newInst = nullptr;
					//					if (llvmIrInstruction->getOperand(0)->getType()->isIntegerTy())
					//					{
					//						newInst = Builder.CreateSIToFP(
					//						    llvmIrInstruction->getOperand(0), llvmIrInstruction->getType());
					//					}
					//					else
					//					{
					//						newInst = Builder.CreateFPCast(
					//						    llvmIrInstruction->getOperand(0), llvmIrInstruction->getType());
					//					}
					//					llvmIrInstruction->replaceAllUsesWith(newInst);
					//					llvmIrInstruction->removeFromParent();
					//					break;
					//				}

				case Instruction::BitCast:
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

	// adaptTypeCast(llvmIrFunction, quantizedType);

	// Process functions that are whitelisted for dequantization
	// processWhitelistedFunctions(*module, whitelist);
	return;
}