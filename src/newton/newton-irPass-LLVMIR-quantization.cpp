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

#define FRAC_Q 10
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
// void
// handleLoadStoreInstructions(Function & llvmIrFunction, Type * quantizedType)
//{
//	llvm::errs() << "Entering handleLoadStoreInstructions\n";
//	for (BasicBlock & llvmIrBasicBlock : llvmIrFunction)
//	{
//		for (BasicBlock::iterator itBB = llvmIrBasicBlock.begin(); itBB != llvmIrBasicBlock.end();)
//		{
//			Instruction * llvmIrInstruction = &*itBB++;
//			if (llvmIrInstruction->getOpcode() == Instruction::Load)
//			{
//				if (auto loadInst = dyn_cast<LoadInst>(llvmIrInstruction))
//				{
//					auto ptr = loadInst->getPointerOperand();
//					if (ptr->getType()->getPointerElementType()->isFloatTy() ||
//					    ptr->getType()->getPointerElementType()->isDoubleTy())
//					{
//						// 如果是全局变量，确保它的类型已经被转换
//						if (auto globalVar = dyn_cast<GlobalVariable>(ptr))
//						{
//							if (globalVar->getType()->getElementType()->isFloatTy() ||
//							    globalVar->getType()->getElementType()->isDoubleTy())
//							{
//								globalVar->mutateType(quantizedType->getPointerTo());
//							}
//						}
//						else
//						{
//							ptr->mutateType(quantizedType->getPointerTo());
//						}
//					}
//				}
//			}
//			else if (llvmIrInstruction->getOpcode() == Instruction::Store)
//			{
//				if (auto storeInst = dyn_cast<StoreInst>(llvmIrInstruction))
//				{
//					auto ptr = storeInst->getPointerOperand();
//					if (ptr->getType()->getPointerElementType()->isFloatTy() ||
//					    ptr->getType()->getPointerElementType()->isDoubleTy())
//					{
//						// 如果是全局变量，确保它的类型已经被转换
//						if (auto globalVar = dyn_cast<GlobalVariable>(ptr))
//						{
//							if (globalVar->getType()->getElementType()->isFloatTy() ||
//							    globalVar->getType()->getElementType()->isDoubleTy())
//							{
//								globalVar->mutateType(quantizedType->getPointerTo());
//							}
//						}
//						else
//						{
//							ptr->mutateType(quantizedType->getPointerTo());
//						}
//					}
//				}
//			}
//		}
//	}
//}

void
fixLoadStoreTypes(Function & F, Type * quantizedType)
{
	for (BasicBlock & BB : F)
	{
		for (Instruction & I : BB)
		{
			if (auto * loadInst = dyn_cast<LoadInst>(&I))
			{
				IRBuilder<> Builder(loadInst);

				// 确保load指令使用量化类型
				auto * pointerOperand = loadInst->getPointerOperand();
				pointerOperand->mutateType(quantizedType->getPointerTo());

				// 创建新的load指令，并替换旧的load指令
				auto * newLoad = Builder.CreateLoad(quantizedType, pointerOperand);
				loadInst->replaceAllUsesWith(newLoad);
				loadInst->eraseFromParent();
			}
			else if (auto * storeInst = dyn_cast<StoreInst>(&I))
			{
				IRBuilder<> Builder(storeInst);

				// 确保store指令使用量化类型
				auto * valueOperand   = storeInst->getValueOperand();
				auto * pointerOperand = storeInst->getPointerOperand();

				// 转换store值为量化类型
				auto * newValue = Builder.CreateFPToSI(valueOperand, quantizedType);
				pointerOperand->mutateType(quantizedType->getPointerTo());

				// 创建新的store指令，并替换旧的store指令
				auto * newStore = Builder.CreateStore(newValue, pointerOperand);
				storeInst->replaceAllUsesWith(newStore);
				storeInst->eraseFromParent();
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
		params.push_back(arg.getType());  // 保持原参数类型
	}

	Type *	       returnType  = quantizedType;  // 返回量化后的整数类型
	FunctionType * newFuncType = FunctionType::get(returnType, params, false);
	Function *     newFunc	   = Function::Create(newFuncType, llvmIrFunction.getLinkage(), llvmIrFunction.getName() + "_quantized", llvmIrFunction.getParent());
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
	// if (functionName == "llvm.dbg.declare" || functionName == "llvm.dbg.value" || functionName == "llvm.dbg.label" || functionName == "fixmul" || functionName == "floatIntMul" || functionName == "sqrt" ||functionName == "printf")
	if (functionName == "llvm.dbg.declare" || functionName == "llvm.dbg.value" || functionName == "llvm.dbg.label" || functionName == "fixmul" || functionName == "floatIntMul" || functionName == "sqrt"||functionName =="sin")

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


	llvm::errs() << "Original function name: " << llvmIrFunction.getName() << "\n";
	for (auto & arg : llvmIrFunction.args())
	{
		llvm::errs() << "Original argument type: " << *arg.getType() << "\n";
	}

	// Keep the original function parameters
	std::vector<Type *> params;
	for (auto & arg : llvmIrFunction.args())
	{
		params.push_back(arg.getType());
	}

	// Create a new function with the quantized return type
	Type *	       returnType  = quantizedType;
	FunctionType * newFuncType = FunctionType::get(returnType, params, false);

	Function * newFunc = createQuantizedFunction(llvmIrFunction, quantizedType);


	llvm::errs() << "New function name: " << newFunc->getName() << "\n";
	for (auto & arg : newFunc->args())
	{
		llvm::errs() << "New argument type: " << *arg.getType() << "\n";
	}

	cloneFunctionBody(llvmIrFunction, newFunc);
	replaceFunctionUses(llvmIrFunction, newFunc);

	// Add the old function to the list of functions to erase
	functionsToErase.push_back(&llvmIrFunction);
	llvmIrFunction.replaceAllUsesWith(UndefValue::get(llvmIrFunction.getType()));
	llvm::errs() << "Finished handling function signature for: " << newFunc->getName() << "\n";
}

void
handleSqrtCall(CallInst * llvmIrCallInstruction, Type * quantizedType)
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

	IRBuilder<> Builder(llvmIrCallInstruction);
	auto	    operand = llvmIrCallInstruction->getOperand(0);

	llvm::errs() << "Operand type: " << *operand->getType() << "\n";
	if (operand->getType()->isIntegerTy())
	{
		Value * newOperand = Builder.CreateSIToFP(operand, llvmIrCallInstruction->getType());
		llvmIrCallInstruction->setOperand(0, newOperand);
	}

	auto	cloneInst  = llvmIrCallInstruction->clone();
	llvm::errs() << "Cloned instruction: " << *cloneInst << "\n";
	Value * fptosiInst = Builder.CreateFPToSI(cloneInst, quantizedType);
	Value * shlInst	   = Builder.CreateShl(fptosiInst, FRAC_Q / 2);
	Value * resInst	   = nullptr;

	/*
	 * if (FRAC_Q%2) then multiply with 1.414213562;
	 * */

	if (FRAC_Q % 2)
	{
		Value * lhsCompensateInst = Builder.CreateSIToFP(shlInst, llvmIrCallInstruction->getType());
		auto	compensateNum	  = ConstantFP::get(llvmIrCallInstruction->getType(), 1.414213562);
		Value * mulInst		  = Builder.CreateFMul(lhsCompensateInst, compensateNum);
		resInst			  = Builder.CreateFPToSI(mulInst, quantizedType);
		llvm::errs() << "Compensation applied\n";
	}
	else
	{
		resInst = shlInst;
	}

	llvmIrCallInstruction->replaceAllUsesWith(resInst);
	ReplaceInstWithInst(llvmIrCallInstruction, cloneInst);
	llvm::errs() << "Exiting handleSinCall\n";
}
}


void handleSinCall(CallInst *llvmIrCallInstruction, Type *quantizedType)
{

	// Debugging output
	llvm::errs() << "Entering handleSinCall\n";
	/*
     * if the arg's type is int, convert to fp,
     * after the call node, convert to int and shl FRAC_Q/2
     *
     * int32_t res = (int32_t)sin(x)<<(FRAC_Q/2);
     * if (FRAC_Q%2)
     *   return res*1.414213562;
     * else
     *   return res;
     *
     * %25 = sitofp i32 %0 to double
     * %26 = call double @sin(double %25) #3
     * %27 = fptosi double %26 to i32
     * %28 = shl i32 %27, 4
     * */
	IRBuilder<> Builder(llvmIrCallInstruction);
	auto operand = llvmIrCallInstruction->getOperand(0);

	llvm::errs() << "Operand type: " << *operand->getType() << "\n";

	if (operand->getType()->isIntegerTy())
	{
		Value *newOperand = Builder.CreateSIToFP(operand, llvmIrCallInstruction->getType());
		llvmIrCallInstruction->setOperand(0, newOperand);
	}

	auto cloneInst = llvmIrCallInstruction->clone();
	llvm::errs() << "Cloned instruction: " << *cloneInst << "\n";
	Value *fptosiInst = Builder.CreateFPToSI(cloneInst, quantizedType);
	Value *shlInst = Builder.CreateShl(fptosiInst, FRAC_Q / 2);
	Value *resInst = nullptr;

	/*
     * if (FRAC_Q%2) then multiply with 1.414213562;
     * */

	if (FRAC_Q % 2)
	{
		Value *lhsCompensateInst = Builder.CreateSIToFP(shlInst, llvmIrCallInstruction->getType());
		auto compensateNum = ConstantFP::get(llvmIrCallInstruction->getType(), 1.414213562);
		Value *mulInst = Builder.CreateFMul(lhsCompensateInst, compensateNum);
		resInst = Builder.CreateFPToSI(mulInst, quantizedType);
		llvm::errs() << "Compensation applied\n";
	}
	else
	{
		resInst = shlInst;
	}

	llvmIrCallInstruction->replaceAllUsesWith(resInst);
	ReplaceInstWithInst(llvmIrCallInstruction, cloneInst);
	llvm::errs() << "Exiting handleSinCall\n";
}

// A list of global variables to erase after processing
std::vector<GlobalVariable *> globalsToErase;

// Function to actually erase global variables after processing
void
eraseOldGlobals()
{
	llvm::errs() << "Entering eraseOldGlobals\n";
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
	llvm::errs() << "Exiting eraseOldGlobals\n";
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

// Function to update global variables from floating-point to integer types
// void updateGlobalVariables(llvm::Module *module, llvm::Type *quantizedType) {
//	for (llvm::GlobalVariable &globalVar : module->globals()) {
//		if (globalVar.getType()->getElementType()->isFloatTy() || globalVar.getType()->getElementType()->isDoubleTy()) {
//			llvm::errs() << "Quantizing global variable: " << globalVar.getName() << "\n";
//
//			// Create the new integer type pointer
//			llvm::Type* newType = quantizedType->getPointerTo();
//
//			// Update the initializer of the global variable
//			llvm::Constant *newInitializer = nullptr;
//			if (llvm::Constant *init = globalVar.getInitializer()) {
//				if (llvm::ConstantFP *constFp = llvm::dyn_cast<llvm::ConstantFP>(init)) {
//					double value = constFp->getValueAPF().convertToDouble();
//					int64_t quantizedValue = static_cast<int64_t>(round(value * FRAC_BASE));
//					newInitializer = llvm::ConstantInt::get(quantizedType, quantizedValue);
//				}
//			}
//
//			// Create a new global variable with the updated type and initializer
//			llvm::GlobalVariable *newGlobalVar = new llvm::GlobalVariable(
//			    *module,
//			    quantizedType,
//			    globalVar.isConstant(),
//			    globalVar.getLinkage(),
//			    newInitializer,
//			    globalVar.getName() + "_quantized",
//			    nullptr,
//			    globalVar.getThreadLocalMode(),
//			    globalVar.getType()->getAddressSpace(),
//			    globalVar.isExternallyInitialized()
//			);
//
//			// Replace all uses of the old global variable with the new one
//			globalVar.replaceAllUsesWith(newGlobalVar);
//
//			// Add the old global variable to the list of globals to erase
//			globalsToErase.push_back(&globalVar);
//		}
//	}
//}

void
updateGlobalVariables(Module * module, Type * quantizedType)
{
	llvm::errs() << "Updating global variables\n";

	for (GlobalVariable & globalVar : module->globals())
	{
		if (globalVar.getType()->getElementType()->isFloatTy() || globalVar.getType()->getElementType()->isDoubleTy())
		{
			llvm::errs() << "Quantizing global variable: " << globalVar.getName() << "\n";

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

				// Replace all uses of the old global variable with the new one
				globalVar.replaceAllUsesWith(newGlobalVar);
			}

			// Add the old global variable to the list of globals to erase
			globalsToErase.push_back(&globalVar);
		}
	}
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

// 量化常量：
// void
// quantizeConstant(Instruction * inInstruction, Type * quantizedType)
//{
//	for (size_t idx = 0; idx < inInstruction->getNumOperands(); idx++)
//	{
//		Value * inValue = inInstruction->getOperand(idx);
//
//		if (!isa<llvm::ConstantFP>(inValue))
//		{
//			continue;
//		}
//
//		ConstantFP * constFp  = llvm::dyn_cast<llvm::ConstantFP>(inValue);
//		Value *	     newValue = nullptr;
//
//		if (inValue->getType()->isFloatTy())
//		{
//			float constValue = constFp->getValueAPF().convertToFloat();
//			constValue *= FRAC_BASE;
//			newValue = ConstantInt::get(quantizedType, round(constValue), true);
//		}
//		else if (inValue->getType()->isDoubleTy())
//		{
//			double constValue = constFp->getValueAPF().convertToDouble();
//			constValue *= FRAC_BASE;
//			newValue = ConstantInt::get(quantizedType, round(constValue), true);
//		}
//		else
//		{
//			assert(false && "unknown floating type");
//		}
//
//		inInstruction->replaceUsesOfWith(inValue, newValue);
//	}
//}

// void
// handleFloatIntMul(Instruction * llvmIrInstruction, Type * intType, Type * floatType, Function * floatIntMul)
//{
//	llvm::errs() << "Handling FloatIntMul\n";
//	llvm::errs() << "Original Instruction: " << *llvmIrInstruction << "\n";
//	IRBuilder<> Builder(llvmIrInstruction);
//
//	// 获取操作数
//	Value * lhs = llvmIrInstruction->getOperand(0);
//	Value * rhs = llvmIrInstruction->getOperand(1);
//
//	llvm::errs() << "LHS: " << *lhs << "\n";
//	llvm::errs() << "RHS: " << *rhs << "\n";
//
//	// 确保左操作数是浮点数，右操作数是整数
//	if (!lhs->getType()->isFloatTy() && !lhs->getType()->isDoubleTy())
//	{
//		std::swap(lhs, rhs);
//	}
//
//	if (lhs->getType()->isFloatTy() || lhs->getType()->isDoubleTy())
//	{
//		if (rhs->getType()->isIntegerTy())
//		{
//			llvm::CallInst * callInst = Builder.CreateCall(floatIntMul, {lhs, rhs});
//			llvmIrInstruction->replaceAllUsesWith(callInst);
//			llvmIrInstruction->eraseFromParent();
//			llvm::errs() << "Replaced with call to floatIntMul\n";
//		}
//		else
//		{
//			llvm::errs() << "RHS is not an integer\n";
//		}
//	}
//	else
//	{
//		llvm::errs() << "LHS is not a float\n";
//	}
//
//	llvm::errs() << "Finished handling FloatIntMul\n";
// }

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
		float	 compensateNum	 = quantizedNum / decimalNum;
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
				llvm::errs() << "Calling handleFloatIntMul\n";
				llvm::CallInst * callInst = Builder.CreateCall(floatIntMul, {nonConstOperand, quantizeNumValue});
				inInstruction->replaceAllUsesWith(callInst);
			}
			else
			{
				llvm::errs() << "Replacing original fmul instruction with integer mul\n";
				Value * newMulValue = Builder.CreateMul(nonConstOperand, quantizeNumValue);
				inInstruction->replaceAllUsesWith(newMulValue);
			}
			inInstruction->eraseFromParent();
			// inInstruction->setOperand(constIdx, quantizeNumValue);
		}
		else
		{
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
				newFirstInst = Builder.CreateMul(nonConstOperand, quantizeNumValue);
				llvm::errs() << "Created Mul instruction: " << *newFirstInst << "\n";
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

		ConstantFP * constFp = dyn_cast<ConstantFP>(inValue);
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

llvm::Function *
createFloatIntMul(Module * irModule, Type * intType, Type * floatType, std::vector<llvm::Function *> & functionsToInsert)
{
	llvm::errs() << "Entering createFloatIntMul\n";

	// Check if irModule is valid
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

// Create a fixed-point division function
llvm::Function *
createFixDiv(Module * irModule, Type * quantizedType, std::vector<llvm::Function *> & functionsToInsert)
{
	llvm::errs() << "Entering createFixDiv\n";

	// Check if irModule is valid
	if (!irModule)
	{
		llvm::errs() << "Error: irModule is nullptr\n";
		return nullptr;
	}

	std::string fixdivFuncName = "fixdiv";
	for (auto & function : *irModule)
	{
		if (function.getName() == fixdivFuncName)
		{
			llvm::errs() << "fixdiv already exists\n";
			return &function;
		}
	}

	llvm::FunctionType * funcType = llvm::FunctionType::get(quantizedType, {quantizedType, quantizedType}, false);
	llvm::Function *     func     = llvm::Function::Create(funcType, llvm::Function::PrivateLinkage, fixdivFuncName, irModule);

	llvm::BasicBlock * entryBB = llvm::BasicBlock::Create(irModule->getContext(), "entry", func);
	llvm::IRBuilder<>  builder(entryBB);
	builder.SetInsertPoint(entryBB);

	// Create fixed-point division instruction
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

	llvm::Function::arg_iterator arg1 = &*(func->arg_begin());

	llvm::Value *		     sext1 = builder.CreateSExt(arg1, higherQuantizedType);
	llvm::Function::arg_iterator arg2  = &*(++arg1);
	llvm::Value *		     sext2 = builder.CreateSExt(arg2, higherQuantizedType);

	// Multiply numerator by FRAC_BASE before division to maintain precision
	llvm::Value * mulInst	= builder.CreateMul(sext1, ConstantInt::get(higherQuantizedType, FRAC_BASE));
	llvm::Value * divInst	= builder.CreateSDiv(mulInst, sext2);
	llvm::Value * truncInst = builder.CreateTrunc(divInst, quantizedType);
	builder.CreateRet(truncInst);

	functionsToInsert.emplace_back(func);
	llvm::errs() << "Created fixdiv function: " << func->getName() << "\n";
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
		default:
			llvm::errs() << "Unhandled floating point predicate\n";
			return ICmpInst::ICMP_EQ;  // Default to equal for safety
	}
}

void
dequantizeArgumentsAndQuantizeReturn(CallInst * llvmIrCallInstruction, Type * quantizedType)
{
	IRBuilder<>	 Builder(llvmIrCallInstruction);
	llvm::Function * calledFunction = llvmIrCallInstruction->getCalledFunction();

	if (!calledFunction)
	{
		llvm::errs() << "Error: Called function is null.\n";
		return;
	}

	llvm::errs() << "De-quantizing arguments for library function call\n";

	// Create a vector to store de-quantized arguments
	std::vector<Value *> dequantizedArgs;

	// Iterate through the call instruction's operands (arguments)
	for (unsigned i = 0; i < llvmIrCallInstruction->getNumArgOperands(); ++i)
	{
		Value * arg = llvmIrCallInstruction->getArgOperand(i);

		if (arg->getType()->isIntegerTy())
		{
			// Convert integer arguments back to floating-point
			Value * dequantizedArg = Builder.CreateSIToFP(arg, llvmIrCallInstruction->getFunctionType()->getParamType(i));
			dequantizedArgs.push_back(dequantizedArg);
		}
		else
		{
			// If not an integer, keep the original argument
			dequantizedArgs.push_back(arg);
		}
	}

	// Create a new call instruction with de-quantized arguments
	CallInst * newCall = CallInst::Create(calledFunction, dequantizedArgs, "", llvmIrCallInstruction);
	newCall->setCallingConv(llvmIrCallInstruction->getCallingConv());
	newCall->setDebugLoc(llvmIrCallInstruction->getDebugLoc());

	llvm::errs() << "Quantizing return value of the library function call\n";
	// Quantize the return value of the call instruction
	if (newCall->getType()->isFloatingPointTy())
	{
		Value * quantizedReturnValue = Builder.CreateFPToSI(newCall, quantizedType);
		llvmIrCallInstruction->replaceAllUsesWith(quantizedReturnValue);
	}
	else
	{
		llvmIrCallInstruction->replaceAllUsesWith(newCall);
	}

	// Remove the old call instruction
	llvmIrCallInstruction->eraseFromParent();
}

void
handleFAdd(Instruction * inInstruction, Type * quantizedType)
{
	llvm::errs() << "Handling FSub\n";
	IRBuilder<> Builder(inInstruction);

	// quantizeConstant(inInstruction, quantizedType);
	Value * op0 = inInstruction->getOperand(0);
	Value * op1 = inInstruction->getOperand(1);

	Value * newInst = Builder.CreateAdd(op0, op1);
	inInstruction->replaceAllUsesWith(newInst);
	inInstruction->eraseFromParent();
	llvm::errs() << "Finished handling FSub\n";
}

void
handleFSub(Instruction * inInstruction, Type * quantizedType)
{
	llvm::errs() << "Handling FSub\n";
	IRBuilder<> Builder(inInstruction);
	Value *	    op0 = inInstruction->getOperand(0);
	Value *	    op1 = inInstruction->getOperand(1);

	//	if (op0->getType()->isFloatTy() || op0->getType()->isDoubleTy())
	//	{
	//		op0 = Builder.CreateFPToSI(op0, quantizedType);
	//	}
	//	if (op1->getType()->isFloatTy() || op1->getType()->isDoubleTy())
	//	{
	//		op1 = Builder.CreateFPToSI(op1, quantizedType);
	//	}
	Value * newInst = Builder.CreateSub(op0, op1);
	inInstruction->replaceAllUsesWith(newInst);
	inInstruction->eraseFromParent();
	llvm::errs() << "Finished handling FSub\n";
}

float
checkDecimal(float decimalNum)
{
	int digits = 0;
	while (fabs(round(decimalNum) - decimalNum) > 0.001 && digits < 4)
	{
		decimalNum *= 10;  // Scale decimal to avoid precision issues
		digits++;
	}
	return decimalNum;
}

void
compensateFP(Instruction * inInstruction, float quantizedNum, float decimalNum, Type * quantizedType)
{
	float	 compensateNum	 = quantizedNum / decimalNum;
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

	//uto quantizeNumValue = ConstantInt::get(quantizedType, round(compensateNum), true);
	auto quantizeNumValue = ConstantInt::get(quantizedType, round(quantizedNum), true);

	llvm::errs() << "Compensate Num: " << compensateNum << "\n";
	llvm::errs() << "Quantized Num Value: " << quantizedNum << "\n";
	llvm::errs() << "Instruction Opcode: " << inInstruction->getOpcodeName() << "\n";

	if (compensateNum == 1)
	{
		llvm::errs() << "Compensation factor is 1, directly setting the quantized value\n";
		inInstruction->setOperand(constIdx, quantizeNumValue);
	}
	else
	{
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
			llvm::errs() << "Handling FMul instruction\n";
			newFirstInst  = Builder.CreateMul(nonConstOperand, quantizeNumValue);
			newSecondInst = Builder.CreateSDiv(newFirstInst, compensateNumValue);
		}
		else if (instOpCode == Instruction::FDiv && isa<llvm::Constant>(inInstruction->getOperand(1)))
		{
			llvm::errs() << "Handling FDiv instruction with constant denominator\n";
			newFirstInst  = Builder.CreateMul(nonConstOperand, compensateNumValue);
			newSecondInst = Builder.CreateSDiv(newFirstInst, quantizeNumValue);
		}
		else if (instOpCode == Instruction::FDiv && isa<llvm::Constant>(inInstruction->getOperand(0)))
		{
			llvm::errs() << "Handling FDiv instruction with constant numerator\n";
			newFirstInst  = Builder.CreateSDiv(quantizeNumValue, nonConstOperand);
			newSecondInst = Builder.CreateSDiv(newFirstInst, compensateNumValue);
		}

		if (newSecondInst)
		{
			llvm::errs() << "Replacing old instruction with compensated instruction\n";
			inInstruction->replaceAllUsesWith(newSecondInst);
			inInstruction->eraseFromParent();
		}
		else
		{
			llvm::errs() << "Failed to create new compensated instruction\n";
		}
	}
}



void
handleConstant(Instruction * inInstruction, Type * quantizedType)
{
	llvm::errs() << "Handling constant operand\n";

	for (size_t idx = 0; idx < inInstruction->getNumOperands(); idx++)
	{
		Value * inValue = inInstruction->getOperand(idx);
		if (!isa<llvm::ConstantFP>(inValue))
		{
			continue;
		}

		ConstantFP * constFp = dyn_cast<ConstantFP>(inValue);

		if (inValue->getType()->isFloatTy())
		{
			float constValue = constFp->getValueAPF().convertToFloat();
			llvm::errs() << "Original float constant: " << constValue << "\n";
			float decimalValue = checkDecimal(constValue);
			llvm::errs() << "Decimal value after checkDecimal: " << decimalValue << "\n";
			if (decimalValue == constValue)
			{
				// If the decimal part is already an integer, quantize directly
				auto quantizedValue = static_cast<int64_t>(round(constValue * FRAC_BASE));
				auto	quantizedConst = ConstantInt::get(quantizedType, quantizedValue);
				llvm::errs() << "Quantized float value: " << quantizedValue << "\n";
				inInstruction->setOperand(idx, quantizedConst);
			}
			else
			{
				compensateFP(inInstruction, decimalValue, constValue, quantizedType);
			}
		}
		else if (inValue->getType()->isDoubleTy())
		{
			double constValue = constFp->getValueAPF().convertToDouble();
			llvm::errs() << "Original double constant: " << constValue << "\n";
			double decimalValue = checkDecimal(constValue);
			llvm::errs() << "Decimal value after checkDecimal: " << decimalValue << "\n";
			if (decimalValue == constValue)
			{
				// If the decimal part is already an integer, quantize directly
				int64_t quantizedValue = static_cast<int64_t>(round(constValue * FRAC_BASE));
				auto	quantizedConst = ConstantInt::get(quantizedType, quantizedValue);
				llvm::errs() << "Quantized double value: " << quantizedValue << "\n";
				inInstruction->setOperand(idx, quantizedConst);
			}
			else
			{
				compensateFP(inInstruction, decimalValue, constValue, quantizedType);
			}
		}
		else
		{
			assert(false && "unknown floating type");
		}
	}
	llvm::errs() << "Exiting handleConstant\n";
}

void
handleFMul(Instruction * llvmIrInstruction, Type * quantizedType, Function * fixmul)
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
		if (isa<llvm::Constant>(lhs) || isa<llvm::Constant>(rhs))
		{
			llvm::errs() << "One of the operands is a constant, simplifying...\n";
			handleConstant(llvmIrInstruction, quantizedType);
		}
		else
		{
			llvm::errs() << "Both operands are integers, substituting with fixmul function...\n";
			{
				llvm::errs() << "Both operands are integers, substituting with fixmul function...\n";
				llvm::CallInst * callInst = Builder.CreateCall(fixmul, {lhs, rhs});
				llvmIrInstruction->replaceAllUsesWith(callInst);
				llvmIrInstruction->eraseFromParent();
			}
		}
	}
}

void
handleFDiv(Instruction * llvmIrInstruction, Type * quantizedType, Function * fixdiv)
{
	llvm::errs() << "Handling FDiv	\n";
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
		if (isa<llvm::Constant>(lhs) || isa<llvm::Constant>(rhs))
		{
			llvm::errs() << "One of the operands is a constant, simplifying...\n";
			handleConstant(llvmIrInstruction, quantizedType);
		}
		else
		{
			llvm::errs() << "Both operands are integers, substituting with fixmul function...\n";
			{
				// Value * newInst = Builder.CreateMul(lhs, rhs);
				Value * newInst = Builder.CreateCall(fixdiv, {lhs, rhs});
				llvmIrInstruction->replaceAllUsesWith(newInst);
				llvmIrInstruction->eraseFromParent();
				llvm::errs() << "Finished handling FDiv\n";
			}
		}
	}
}

void
handleFRem(Instruction * inInstruction, Type * quantizedType)
{
	llvm::errs() << "Handling FRem\n";
	IRBuilder<> Builder(inInstruction);
	Value *	    op0 = inInstruction->getOperand(0);
	Value *	    op1 = inInstruction->getOperand(1);

	if (op0->getType()->isFloatTy() || op0->getType()->isDoubleTy())
	{
		op0 = Builder.CreateFPToSI(op0, quantizedType);
	}
	if (op1->getType()->isFloatTy() || op1->getType()->isDoubleTy())
	{
		op1 = Builder.CreateFPToSI(op1, quantizedType);
	}
	Value * newInst = Builder.CreateSRem(op0, op1);
	inInstruction->replaceAllUsesWith(newInst);
	inInstruction->eraseFromParent();
	llvm::errs() << "Finished handling FRem\n";
}

void
handleFCmp(Instruction * inInstruction, Type * quantizedType)
{
	llvm::errs() << "Handling FCmp\n";
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
		llvm::errs() << "Finished handling FCmp\n";
	}
}
void
handleAlloca(Instruction * llvmIrInstruction, Type * quantizedType)
{
	if (auto llvmIrAllocaInstruction = dyn_cast<AllocaInst>(llvmIrInstruction))
	{
		auto allocaType = llvmIrAllocaInstruction->getAllocatedType();

		// Check if the alloca instruction is of type float or double
		if (allocaType->isFloatTy() || allocaType->isDoubleTy())
		{
			llvm::errs() << "Original alloca type: " << *allocaType << "\n";
			llvm::errs() << "New quantized alloca type: " << *quantizedType << "\n";

			// Set the new quantized type for the alloca instruction
			llvmIrAllocaInstruction->setAllocatedType(quantizedType);
		}
	}
}

void handleCall(CallInst *llvmIrCallInstruction, Type *quantizedType) {

	Function *calledFunction = llvmIrCallInstruction->getCalledFunction();
	if (calledFunction == nullptr || !calledFunction->hasName() || calledFunction->getName().empty())
		return;

	if (!calledFunction->getName().startswith("llvm.dbg.value") &&
	    !calledFunction->getName().startswith("llvm.dbg.declare") &&
	    !calledFunction->getName().startswith("llvm.dbg.label")) {

		if (calledFunction->isDeclaration()) {
			// For library functions
			IRBuilder<> Builder(llvmIrCallInstruction);
			Instruction *insertPoint = llvmIrCallInstruction->getNextNode();
			Builder.SetInsertPoint(insertPoint);
			Value *newInst = nullptr;

			if (calledFunction->getName().str() == "sqrt") {
				// For sqrt
				handleSqrtCall(llvmIrCallInstruction, quantizedType);
			} else if  (calledFunction->getName().str() == "sin") {
				//For sin
				handleSinCall(llvmIrCallInstruction, quantizedType);
			}
			else {
				/*
                 * for other lib functions, de-quantize the arguments and quantize the return value
				 */
				dequantizeArgumentsAndQuantizeReturn(llvmIrCallInstruction, quantizedType);
			}
		} else {
			/*
             * for user-defined function, quantize the arguments
			 */
			// Uncomment and adapt the following lines if needed
			 for (size_t idx = 0; idx < llvmIrCallInstruction->getNumOperands() - 1; idx++) {
			     setQuantizedType(llvmIrCallInstruction->getOperand(idx), quantizedType);
			 }
			quantizeConstant(llvmIrCallInstruction, quantizedType);
			 /*
			  * then quantize the return type
			  */
			 setQuantizedType(llvmIrCallInstruction, quantizedType);
		}
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

			// 转换存储的值为量化后的类型
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




void handleLoad(Instruction *llvmIrInstruction, Type *quantizedType) {
	if (auto llvmIrLoadInstruction = dyn_cast<LoadInst>(llvmIrInstruction)) {
		IRBuilder<> Builder(llvmIrLoadInstruction);

		auto pointerType = llvmIrLoadInstruction->getPointerOperand()->getType()->getPointerElementType();

		// Print the pointer type of the load instruction
		llvm::errs() << "Pointer type of load instruction: " << *pointerType << "\n";

		//if (pointerType->isFloatTy() || pointerType->isDoubleTy()) {
		llvm::errs() << "Original load pointer type: " << *pointerType << "\n";
		llvm::errs() << "New quantized load pointer type: " << *quantizedType << "\n";

		// 更新指针操作数的类型为量化类型
		llvmIrLoadInstruction->getPointerOperand()->mutateType(quantizedType->getPointerTo());

		// 以整数形式加载值
		auto loadedValue = Builder.CreateLoad(quantizedType, llvmIrLoadInstruction->getPointerOperand());

		// 检查后续使用并确保类型匹配
		for (auto &use : llvmIrLoadInstruction->uses()) {
			auto user = use.getUser();
			if (auto storeInst = dyn_cast<StoreInst>(user)) {
				// 在存储指令中，如果值被存储为浮点型，则进行适当转换
				if (storeInst->getValueOperand() == llvmIrLoadInstruction) {
					auto floatValue = Builder.CreateSIToFP(loadedValue, Type::getFloatTy(llvmIrInstruction->getContext()));
					storeInst->setOperand(0, floatValue);
				}
			}
		}

		//replace all uses of the original load instruction with the loaded value
		llvmIrLoadInstruction->replaceAllUsesWith(loadedValue);

		// erase the original load instruction
		llvmIrLoadInstruction->eraseFromParent();
		//		} else {
		//			llvm::errs() << "Pointer type is not float or double, skipping load handling.\n";
		//		}
	}
}







void
handleFPExt(Instruction * llvmIrInstruction, Type * quantizedType)
{
	llvm::errs() << "Handling FPExt: " << *llvmIrInstruction << "\n";
	auto * fpextInst = cast<FPExtInst>(llvmIrInstruction);
	auto   srcType	 = fpextInst->getSrcTy();
	auto   destType	 = fpextInst->getDestTy();

	// If the source type is an integer and the destination type is a floating-point type
	if (srcType->isIntegerTy() && (destType->isFloatTy() || destType->isDoubleTy()))
	{
		// Erase the unnecessary FPExt instruction
		llvmIrInstruction->replaceAllUsesWith(fpextInst->getOperand(0));
		llvmIrInstruction->eraseFromParent();
		llvm::errs() << "Removed unnecessary FPExt\n";
	}
	else
	{
		llvm::errs() << "Unhandled FPExt conversion from " << *srcType << " to " << *destType << "\n";
	}
}

void
handleFPTrunc(Instruction * llvmIrInstruction, Type * quantizedType)
{
	llvm::errs() << "Handling FPTrunc: " << *llvmIrInstruction << "\n";
	auto * fptruncInst = cast<FPTruncInst>(llvmIrInstruction);
	auto   srcType	   = fptruncInst->getSrcTy();
	auto   destType	   = fptruncInst->getDestTy();

	if (srcType->isFloatingPointTy() && destType->isFloatingPointTy())
	{
		// Erase the unnecessary FPTrunc instruction
		llvmIrInstruction->replaceAllUsesWith(fptruncInst->getOperand(0));
		llvmIrInstruction->eraseFromParent();
		llvm::errs() << "Removed unnecessary FPTrunc\n";
	}
	else
	{
		llvm::errs() << "Unhandled FPTrunc conversion from " << *srcType << " to " << *destType << "\n";
	}
}

void
adaptTypeCast(llvm::Function & llvmIrFunction, Type * quantizedType)
{
	llvm::errs() << "Entering adaptTypeCast\n";
	for (BasicBlock & llvmIrBasicBlock : llvmIrFunction)
	{
		for (BasicBlock::iterator itBB = llvmIrBasicBlock.begin(); itBB != llvmIrBasicBlock.end();)
		{
			Instruction * llvmIrInstruction = &*itBB++;
			llvm::errs() << "Processing instruction in adaptTypeCast: " << *llvmIrInstruction << "\n";

			switch (llvmIrInstruction->getOpcode())
			{
				case Instruction::Alloca:
					handleAlloca(llvmIrInstruction, quantizedType);
					break;
				case Instruction::Store:
					handleStore(llvmIrInstruction, quantizedType);
					break;
				case Instruction::Load:
					handleLoad(llvmIrInstruction, quantizedType);
					break;
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
				case Instruction::FPExt:
				{
					handleFPExt(llvmIrInstruction, quantizedType);
					break;
				}

				case Instruction::FPTrunc:
				{
					handleFPTrunc(llvmIrInstruction, quantizedType);
					break;
				}
				case Instruction::BitCast:
				{
					IRBuilder<>   Builder(llvmIrInstruction);
					Instruction * insertPoint = llvmIrInstruction->getNextNode();
					Builder.SetInsertPoint(insertPoint);
					Value * newInst = Builder.CreateBitCast(llvmIrInstruction->getOperand(0), llvmIrInstruction->getType());
					llvmIrInstruction->replaceAllUsesWith(newInst);
					llvmIrInstruction->removeFromParent();
					break;
				}
			}
		}
	}
	llvm::errs() << "Exiting adaptTypeCast\n";
}

// Main function to perform LLVM IR auto quantization
void
irPassLLVMIRAutoQuantization(State * N, llvm::Function & llvmIrFunction, std::vector<llvm::Function *> & functionsToInsert)
{
	flexprint(N->Fe, N->Fm, N->Fpinfo, "\tauto quantization.\n");
	llvm::errs() << "Entering irPassLLVMIRAutoQuantization\n";

	// Skip certain functions
	std::string functionName = llvmIrFunction.getName().str();
	if (functionName == "llvm.dbg.declare" || functionName == "llvm.dbg.value" || functionName == "llvm.dbg.label" || functionName == "fixmul" || functionName == "floatIntMul"
	    || functionName == "fixdiv" || functionName == "constantMulDiv"||functionName=="sin" )
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
	Type * floatType = Type::getFloatTy(llvmIrFunction.getContext());
	Type * intType	 = Type::getInt32Ty(llvmIrFunction.getContext());

	// Deal with function signature

	handleFunctionSignature(llvmIrFunction, quantizedType);

	// Update global variable type to integer type

	// Update global variables
	updateGlobalVariables(module, quantizedType);

	/*
	 * generate hardcode function - fixmul and fixdiv
	 * */

	// 先创建 fixmul 和 fixdiv 函数

	llvm::Function * fixdiv = createFixDiv(module, quantizedType, functionsToInsert);
	llvm::Function * fixmul = createFixMul(module, quantizedType, functionsToInsert);

	// generate hardcode function -  floatIntMul function
	// llvm::Function * floatIntMul = createFloatIntMul(module, quantizedType, Type::getFloatTy(llvmIrFunction.getContext()), functionsToInsert);

	/*
	 * quantize the arguments type
	 * */

	// llvm::errs() << "Quantizing arguments for function: " << llvmIrFunction.getName() << "\n";
	//	for (size_t idx = 0; idx < llvmIrFunction.arg_size(); idx++)
	//	{
	//		llvm::errs() << "Quantizing parameter at index " << idx << "\n";
	//		llvm::Function::arg_iterator argIt = llvmIrFunction.arg_begin();
	//		std::advance(argIt, idx);
	//
	//		if (argIt == llvmIrFunction.arg_end())
	//		{
	//			llvm::errs() << "Error: Reached end of arguments while trying to access index " << idx << "\n";
	//			continue;
	//		}
	//
	//		llvm::Argument * paramOp = &*argIt;
	//		llvm::errs() << "Processing parameter: " << paramOp->getName() << "\n";
	//		if (!paramOp)
	//		{
	//			llvm::errs() << "Error: paramOp is nullptr at index " << idx << "\n";
	//			continue;
	//		}
	//
	//		if (paramOp->getType()->isFloatTy() || paramOp->getType()->isDoubleTy())
	//		{
	//			llvm::errs() << "Quantizing parameter from " << *paramOp->getType() << " to " << *quantizedType << "\n";
	//			setQuantizedType(paramOp, quantizedType);
	//		}
	//		else
	//		{
	//			llvm::errs() << "Parameter at index " << idx << " is not a floating-point type, skipping\n";
	//		}
	//	}
	//	for (int idx = 0; idx < llvmIrFunction.arg_size(); idx++)
	//	{
	//		auto	 paramOp	 = llvmIrFunction.getArg(idx);
	//		setQuantizedType(paramOp, quantizedType);
	//	}

	// Process each instruction
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
					handleAlloca(llvmIrInstruction, quantizedType);
					break;
				case Instruction::Call:
					handleCall(cast<CallInst>(llvmIrInstruction), quantizedType);
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
					llvm::errs() << "Handling Load instruction: " << *llvmIrInstruction << "\n";
					handleLoad(llvmIrInstruction, quantizedType);
					break;
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
					handleStore(llvmIrInstruction, quantizedType);
				}
				break;

				/*
				 * For fmul/fdiv,
				 *
				 * if either one of the operands is a constant value, simplify it by multiplying with 10^n,
				 * then replace the instruction to mul/div;
				 *
				 * else substitute this instruction to a pre-implemented function: mulfix/fixdiv.
				 * */
				case Instruction::FMul:
					llvm::errs() << "Found FMul instruction.\n";
					handleFMul(llvmIrInstruction, quantizedType, fixmul);
					break;

				case Instruction::FDiv:
				{
					llvm::errs() << "Found FDiv instruction.\n";
					handleFDiv(llvmIrInstruction, quantizedType, fixdiv);
					break;
				}

				/*
				 * If either one of the operands is a constant value, quantize it,
				 * then replace the instruction to the int version.
				 * */
				case Instruction::FCmp:
					handleFCmp(llvmIrInstruction, quantizedType);
					break;
				case Instruction::FAdd:
					handleFAdd(llvmIrInstruction, quantizedType);
					break;
				case Instruction::FSub:
					handleFSub(llvmIrInstruction, quantizedType);
					break;
				case Instruction::FRem:
					handleFRem(llvmIrInstruction, quantizedType);
					break;
					{
						// quantizeConstant(llvmIrInstruction, quantizedType);
						//  量化浮点常量
						for (size_t idx = 0; idx < llvmIrInstruction->getNumOperands(); idx++)
						{
							Value * inValue = llvmIrInstruction->getOperand(idx);

							if (isa<llvm::ConstantFP>(inValue))
							{
								ConstantFP * constFp  = llvm::dyn_cast<ConstantFP>(inValue);
								Value *	     newValue = nullptr;

								if (inValue->getType()->isFloatTy())
								{
									float constValue = constFp->getValueAPF().convertToFloat();
									constValue *= FRAC_BASE;
									newValue = ConstantInt::get(quantizedType, round(constValue), true);
								}
								else if (inValue->getType()->isDoubleTy())
								{
									double constValue = constFp->getValueAPF().convertToDouble();
									constValue *= FRAC_BASE;
									newValue = ConstantInt::get(quantizedType, round(constValue), true);
								}
								else
								{
									assert(false && "unknown floating type");
								}

								llvmIrInstruction->setOperand(idx, newValue);
							}
						}
					}
				case Instruction::FNeg:

				{
					// quantizeSimpleFPInstruction(llvmIrInstruction, quantizedType);
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
	// handleLoadStoreInstructions(llvmIrFunction, quantizedType);
	adaptTypeCast(llvmIrFunction, quantizedType);


	return;
}
