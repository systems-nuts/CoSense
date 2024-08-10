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

bool
isMatrixOperation(Instruction * instr)
{
	// 检查指令的操作数是否为多维数组或矩阵类型
	// 这里假设多维数组使用特定的类型或有特定的标记
	for (unsigned i = 0; i < instr->getNumOperands(); ++i)
	{
		Value * operand = instr->getOperand(i);
		if (operand->getType()->isPointerTy())
		{
			// 检查指针指向的类型是否为多维数组类型
			Type * elementType = operand->getType()->getPointerElementType();
			if (elementType->isArrayTy())
			{
				// 进一步检查数组的维度或特定特征
				return true;  // 假设这是一个矩阵操作
			}
		}
	}
	return false;  // 如果没有匹配到矩阵操作的特征
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

		// Skip integer and struct types
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

bool shouldSkipFunction(const std::string &functionName) {
	// List of function names to skip
	static const std::unordered_set<std::string> skipFunctions = {
	    "llvm.dbg.declare",
	    "llvm.dbg.value",
	    "llvm.dbg.label",
	    "fixmul",
	    "floatIntMul",
	    "fixdiv",
	    "fixsqrt",
	    "constantMulDiv",
	    "sinf",
	    "llvm.sqrt.f64",
	    "sqrt",
	    "sqrtf"
	};

	return skipFunctions.find(functionName) != skipFunctions.end();
}
// Handle the function signature change for quantization
void
handleFunctionSignature(Function & llvmIrFunction, Type * quantizedType)
{
	llvm::errs() << "Calling handleFunctionSignature for function: " << llvmIrFunction.getName() << "\n";
	// Skip certain functions
	std::string functionName = llvmIrFunction.getName().str();
	if (shouldSkipFunction(functionName)) {
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

	// Skip if the function returns void
	if (llvmIrFunction.getReturnType()->isVoidTy())
	{
		llvm::errs() << "Skipping function with void return type: " << functionName << "\n";
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

// Helper function to recursively transform types
Type *
transformToQuantizedType(Type * originalType, Type * quantizedType)
{
	if (originalType->isArrayTy())
	{
		auto elementType    = originalType->getArrayElementType();
		auto newElementType = transformToQuantizedType(elementType, quantizedType);
		return ArrayType::get(newElementType, originalType->getArrayNumElements());
	}
	else if (originalType->isFloatTy() || originalType->isDoubleTy())
	{
		return quantizedType;
	}
	// Return original type if no conversion is necessary
	return originalType;
}

// llvm::Function *createFixPow(Module *irModule, Type *quantizedType, std::vector<llvm::Function *> &functionsToInsert);

// void handlePowCall(CallInst * llvmIrCallInstruction, Type * quantizedType, std::vector<llvm::Function *> & functionsToInsert)
//{
//	IRBuilder<> Builder(llvmIrCallInstruction);
//	auto base = llvmIrCallInstruction->getArgOperand(0);
//	auto exponent = llvmIrCallInstruction->getArgOperand(1);
//
//	// Convert the operands to the appropriate type for fixpow (which expects quantizedType)
//	Value * quantizedBase = Builder.CreateFPToSI(base, quantizedType);
//	Value * quantizedExponent = Builder.CreateFPToSI(exponent, quantizedType);
//
//	// Get or create the fixpow function
//	Function * fixpowFunc = llvmIrCallInstruction->getModule()->getFunction("fixpow");
//	if (!fixpowFunc)
//	{
//		// If fixpow function doesn't exist, create it
//		fixpowFunc = createFixPow(llvmIrCallInstruction->getModule(), quantizedType, functionsToInsert);
//	}
//
//	// Create call to fixpow with the quantized operands
//	Value * fixpowResult = Builder.CreateCall(fixpowFunc, {quantizedBase, quantizedExponent});
//
//	// Replace the original pow call with the fixpow result
//	llvmIrCallInstruction->replaceAllUsesWith(fixpowResult);
//	llvmIrCallInstruction->eraseFromParent();
// }
void
handleSqrtCall(CallInst * llvmIrCallInstruction, Type * quantizedType, Function * fixsqrt)
{
	IRBuilder<> Builder(llvmIrCallInstruction);
	auto	    operand = llvmIrCallInstruction->getOperand(0);

	// Cast the instruction to CallInst to access getCalledFunction method
	CallInst * callInst = dyn_cast<CallInst>(llvmIrCallInstruction);
	if (!callInst)
	{
		llvm::errs() << "Error: Instruction is not a CallInst.\n";
		return;
	}

	// Convert the operand to fixed-point format if necessary
	if (operand->getType()->isFloatingPointTy())
	{
		operand = Builder.CreateFPToSI(operand, quantizedType);
	}

	// Create call to the fixed-point sqrt function
	llvm::Value * sqrtResult = Builder.CreateCall(fixsqrt, {operand});


	// No need to apply shl and compensation if it's already done in createFixSqrt
	llvmIrCallInstruction->replaceAllUsesWith(sqrtResult);
	llvmIrCallInstruction->eraseFromParent();
}

// void
// handleSqrtCall(CallInst * llvmIrCallInstruction, Type * quantizedType)
//{
//	IRBuilder<> Builder(llvmIrCallInstruction);
//	auto	    operand = llvmIrCallInstruction->getOperand(0);
//
//	// Cast the instruction to CallInst to access getCalledFunction method
//	CallInst * callInst = dyn_cast<CallInst>(llvmIrCallInstruction);
//	if (!callInst)
//	{
//		llvm::errs() << "Error: Instruction is not a CallInst.\n";
//		return;
//	}
//
//	// Convert integer operand to float for sqrt computation
//	Value * newOperand = Builder.CreateSIToFP(operand, Type::getDoubleTy(llvmIrCallInstruction->getContext()));
//
//	// Get the correct sqrt function with double return and argument types
//	Function * sqrtFunc = llvmIrCallInstruction->getModule()->getFunction("sqrt");
//	if (!sqrtFunc)
//	{
//		FunctionType * sqrtType = FunctionType::get(Type::getDoubleTy(llvmIrCallInstruction->getContext()), {Type::getDoubleTy(llvmIrCallInstruction->getContext())}, false);
//		sqrtFunc		= Function::Create(sqrtType, Function::ExternalLinkage, "sqrt", llvmIrCallInstruction->getModule());
//	}
//
//	// Create call to sqrt function with the double operand
//	Value * sqrtResult = Builder.CreateCall(sqrtFunc, {newOperand});
//
//	// Convert the result back to integer
//	Value * fptosiInst = Builder.CreateFPToSI(sqrtResult, quantizedType);
//	// Perform left shift for scaling
//	Value * shlInst = Builder.CreateShl(fptosiInst, FRAC_Q / 2);
//	Value * resInst = nullptr;
//
//	// If FRAC_Q is odd, apply compensation
//	if (FRAC_Q % 2)
//	{
//		Value * lhsCompensateInst = Builder.CreateSIToFP(shlInst, llvmIrCallInstruction->getType());
//		auto	compensateNum	  = ConstantFP::get(llvmIrCallInstruction->getType(), 1.414213562);
//		Value * mulInst		  = Builder.CreateFMul(lhsCompensateInst, compensateNum);
//		resInst			  = Builder.CreateFPToSI(mulInst, quantizedType);
//		llvm::errs() << "Compensation applied\n";
//	}
//	else
//	{
//		resInst = shlInst;
//	}
//
//	llvmIrCallInstruction->replaceAllUsesWith(resInst);
//	llvmIrCallInstruction->eraseFromParent();
// }
// }

void
handleSinCall(CallInst * llvmIrCallInstruction, Type * quantizedType)
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
	auto	    operand = llvmIrCallInstruction->getOperand(0);

	llvm::errs() << "Operand type: " << *operand->getType() << "\n";

	if (operand->getType()->isIntegerTy())
	{
		Value * newOperand = Builder.CreateSIToFP(operand, llvmIrCallInstruction->getType());
		llvmIrCallInstruction->setOperand(0, newOperand);
	}

	auto cloneInst = llvmIrCallInstruction->clone();
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

void
handleMemCpyCall(Instruction * llvmIrInstruction, Type * quantizedType)
{
	if (auto callInst = dyn_cast<CallInst>(llvmIrInstruction))
	{
		Function * calledFunction = callInst->getCalledFunction();
		if (calledFunction && calledFunction->getName().startswith("llvm.memcpy"))
		{
			IRBuilder<> Builder(callInst);

			// 获取原始操作数和它们的类型
			Value * dest = callInst->getArgOperand(0);
			Value * src  = callInst->getArgOperand(1);
			Value * size = callInst->getArgOperand(2);

			// 更新源和目标指针的类型
			Type * newDestType = transformToQuantizedType(dest->getType()->getPointerElementType(), quantizedType)->getPointerTo();
			Type * newSrcType  = transformToQuantizedType(src->getType()->getPointerElementType(), quantizedType)->getPointerTo();

			Value * newDest = Builder.CreateBitCast(dest, newDestType);
			Value * newSrc	= Builder.CreateBitCast(src, newSrcType);

			// 创建新的量化后的 memcpy 调用
			Function * memcpyFunc = Intrinsic::getDeclaration(
			    callInst->getModule(),
			    Intrinsic::memcpy,
			    {newDestType, newSrcType, Builder.getInt64Ty()});
			Builder.CreateCall(memcpyFunc, {newDest, newSrc, size, callInst->getArgOperand(3)});

			// 删除旧的调用
			callInst->eraseFromParent();
		}
	}
}

void
handleRsqrtCall(CallInst * llvmIrCallInstruction, Type * quantizedType, Function * fixrsqrt)
{
	IRBuilder<> builder(llvmIrCallInstruction);

	Value * operand = llvmIrCallInstruction->getOperand(0);

	if (operand->getType()->isIntegerTy())
	{
		operand = builder.CreateSIToFP(operand, llvmIrCallInstruction->getType());
		llvmIrCallInstruction->setOperand(0, operand);
	}

	// Replace the rsqrt call with the custom fixrsqrt function
	Value * rsqrtResult = builder.CreateCall(fixrsqrt, {llvmIrCallInstruction->getOperand(0)});

	// Replace all uses of the original rsqrt call with the fixed-point result
	llvmIrCallInstruction->replaceAllUsesWith(rsqrtResult);

	// Remove the original rsqrt call
	llvmIrCallInstruction->eraseFromParent();
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

//llvm::Function *
//createFixSqrt(llvm::Module * irModule, Type * quantizedType, std::vector<llvm::Function *> & functionsToInsert)
//{
//	// check if irModule is valid
//	if (!irModule)
//	{
//		llvm::errs() << "Error: irModule is nullptr\n";
//		return nullptr;
//	}
//
//	std::string fixSqrtFuncName = "fixsqrt";
//	for (auto & function : *irModule)
//	{
//		if (function.getName() == fixSqrtFuncName)
//		{
//			llvm::errs() << "fixsqrt already exists\n";
//			return &function;
//		}
//	}
//
//	llvm::FunctionType * funcType = llvm::FunctionType::get(quantizedType, {quantizedType}, false);
//	llvm::Function *     func     = llvm::Function::Create(funcType, llvm::Function::PrivateLinkage, "fixsqrt", irModule);
//
//	llvm::BasicBlock * entryBB = llvm::BasicBlock::Create(irModule->getContext(), "entry", func);
//	llvm::IRBuilder<>  builder(entryBB);
//
//	llvm::Function::arg_iterator args = func->arg_begin();
//	llvm::Value *		     x	  = &*args++;
//
//	// Initial approximation: x / 2
//	llvm::Value * approx   = builder.CreateLShr(x, 1);
//	llvm::Value * halfBase = builder.CreateLShr(ConstantInt::get(quantizedType, FRAC_BASE), 1);
//
//	for (int i = 0; i < 5; ++i)
//	{  // Run the approximation a few times
//		llvm::Value * div = builder.CreateSDiv(x, approx);
//		llvm::Value * avg = builder.CreateAdd(approx, div);
//		approx		  = builder.CreateLShr(avg, 1);	 // approx = (approx + x / approx) / 2
//	}
//
//	llvm::Value * result = approx;	// Final square root approximation
//
//	// Apply scaling: multiply by FRAC_BASE to maintain fixed-point representation
//	result = builder.CreateMul(result, halfBase);
//
//	builder.CreateRet(result);
//
//	// Add the created function to the vector of functions to be inserted
//	functionsToInsert.push_back(func);
//
//	return func;
//}

llvm::Function* createFixSqrt(llvm::Module* irModule, Type* quantizedType, std::vector<llvm::Function *> &functionsToInsert) {
	// Check if irModule is valid
	if (!irModule) {
		llvm::errs() << "Error: irModule is nullptr\n";
		return nullptr;
	}

	std::string fixSqrtFuncName = "fixsqrt";
	for (auto & function : *irModule) {
		if (function.getName() == fixSqrtFuncName) {
			llvm::errs() << "fixsqrt already exists\n";
			return &function;
		}
	}

	llvm::LLVMContext& context = irModule->getContext();

	// Function type: returns quantizedType (e.g., i32), takes one argument of quantizedType
	llvm::FunctionType* funcType = llvm::FunctionType::get(quantizedType, {quantizedType}, false);
	llvm::Function* func = llvm::Function::Create(funcType, llvm::Function::PrivateLinkage, fixSqrtFuncName, irModule);

	llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(context, "entry", func);
	llvm::IRBuilder<> builder(entryBB);

	llvm::Function::arg_iterator args = func->arg_begin();
	llvm::Value* x = &*args++;

	// Convert the fixed-point integer to a floating-point number for sqrt computation
	llvm::Value* fp_x = builder.CreateSIToFP(x, llvm::Type::getDoubleTy(context));

	// Call sqrt on the floating-point value
	llvm::Function* sqrtFunc = llvm::Intrinsic::getDeclaration(irModule, llvm::Intrinsic::sqrt, llvm::Type::getDoubleTy(context));
	llvm::Value* sqrtResult = builder.CreateCall(sqrtFunc, {fp_x});

	// Convert the result back to a fixed-point integer
	llvm::Value* res = builder.CreateFPToSI(sqrtResult, quantizedType);

	// Perform a left shift to scale the result
	llvm::Value* shlRes = builder.CreateShl(res, FRAC_Q / 2);

	// Apply compensation if FRAC_Q is odd
	llvm::Value* finalRes = shlRes;
	if (FRAC_Q % 2 != 0) {
		llvm::Value* compensationFactor = llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 1.414213562);
		llvm::Value* fpShlRes = builder.CreateSIToFP(shlRes, llvm::Type::getDoubleTy(context));
		llvm::Value* compensated = builder.CreateFMul(fpShlRes, compensationFactor);
		finalRes = builder.CreateFPToSI(compensated, quantizedType);
	}

	builder.CreateRet(finalRes);

	// Insert the newly created function into the list
	functionsToInsert.push_back(func);

	llvm::errs() << "Created fixsqrt function: " << func->getName() << "\n";

	return func;
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

// llvm::Function* createFixPow(Module* irModule, Type* quantizedType, std::vector<llvm::Function*>& functionsToInsert) {
//	llvm::errs() << "Entering createFixPow\n";
//
//	if (!irModule) {
//		llvm::errs() << "Error: irModule is nullptr\n";
//		return nullptr;
//	}
//
//	std::string fixPowFuncName = "fixpow";
//	for (auto& function : *irModule) {
//		if (function.getName() == fixPowFuncName) {
//			llvm::errs() << "fixpow already exists\n";
//			return &function;
//		}
//	}
//
//	llvm::FunctionType* funcType = llvm::FunctionType::get(quantizedType, {quantizedType, quantizedType}, false);
//	llvm::Function* func = llvm::Function::Create(funcType, llvm::Function::PrivateLinkage, fixPowFuncName, irModule);
//
//	llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(irModule->getContext(), "entry", func);
//	llvm::IRBuilder<> builder(entryBB);
//	builder.SetInsertPoint(entryBB);
//
//	llvm::Function::arg_iterator args = func->arg_begin();
//	llvm::Value* base = &*args++;
//	llvm::Value* exponent = &*args;
//
//	// Initialize result as FRAC_BASE
//	llvm::Value* result = builder.CreateAlloca(quantizedType, nullptr, "result");
//	builder.CreateStore(ConstantInt::get(quantizedType, FRAC_BASE), result);
//
//	llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(irModule->getContext(), "loop", func);
//	llvm::BasicBlock* afterBB = llvm::BasicBlock::Create(irModule->getContext(), "after", func);
//
//	builder.CreateBr(loopBB);
//
//	builder.SetInsertPoint(loopBB);
//	llvm::PHINode* currentBase = builder.CreatePHI(quantizedType, 2, "currentBase");
//	llvm::PHINode* currentExponent = builder.CreatePHI(quantizedType, 2, "currentExponent");
//
//	currentBase->addIncoming(base, entryBB);
//	currentExponent->addIncoming(exponent, entryBB);
//
//	// Check if the current exponent is odd
//	llvm::Value* isOdd = builder.CreateAnd(currentExponent, ConstantInt::get(quantizedType, 1));
//	llvm::Value* isOddCond = builder.CreateICmpEQ(isOdd, ConstantInt::get(quantizedType, 1));
//
//	// Create block for when exponent is odd
//	llvm::BasicBlock* oddBB = llvm::BasicBlock::Create(irModule->getContext(), "odd", func);
//	llvm::BasicBlock* continueBB = llvm::BasicBlock::Create(irModule->getContext(), "continue", func);
//
//	builder.CreateCondBr(isOddCond, oddBB, continueBB);
//
//	builder.SetInsertPoint(oddBB);
//	llvm::Value* currentResult = builder.CreateLoad(quantizedType, result);
//	llvm::Value* updatedResult = builder.CreateMul(currentResult, currentBase);
//	updatedResult = builder.CreateAShr(updatedResult, FRAC_Q);
//	builder.CreateStore(updatedResult, result);
//	builder.CreateBr(continueBB);
//
//	builder.SetInsertPoint(continueBB);
//	llvm::Value* squaredBase = builder.CreateMul(currentBase, currentBase);
//	squaredBase = builder.CreateAShr(squaredBase, FRAC_Q);
//
//	llvm::Value* newExponent = builder.CreateLShr(currentExponent, 1);
//
//	currentBase->addIncoming(squaredBase, continueBB);
//	currentExponent->addIncoming(newExponent, continueBB);
//
//	llvm::Value* endCond = builder.CreateICmpEQ(newExponent, ConstantInt::get(quantizedType, 0));
//	builder.CreateCondBr(endCond, afterBB, loopBB);
//
//	builder.SetInsertPoint(afterBB);
//	llvm::Value* finalResult = builder.CreateLoad(quantizedType, result);
//	builder.CreateRet(finalResult);
//
//	functionsToInsert.emplace_back(func);
//	llvm::errs() << "Created fixpow function: " << func->getName() << "\n";
//	return func;
// }

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

// Create a fixed-point reversed square root function
// llvm::Function* createFixRsqrt(llvm::Module* irModule, Type* quantizedType, std::vector<llvm::Function*>& functionsToInsert) {
//
//	// Define the function type for fixrsqrt: int32_t fixrsqrt(int32_t x)
//	llvm::FunctionType* funcType = llvm::FunctionType::get(quantizedType, {quantizedType}, false);
//	// Create the function and insert it into the module
//	llvm::Function* func = llvm::Function::Create(funcType, llvm::Function::PrivateLinkage, "fixrsqrt", irModule);
//
//	llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(irModule->getContext(), "entry", func);
//	llvm::IRBuilder<> builder(entryBB);
//
//	// Get the function argument (x)
//	llvm::Function::arg_iterator args = func->arg_begin();
//	llvm::Value* x = &*args++;
//
//	// Create the fixed-point multiplication function
//	llvm::Function* fixMulFunc = createFixMul(irModule, quantizedType, functionsToInsert);
//
//	// Step 1: int_halfx = mulfix(0.5 * FRAC_BASE, x);
//	llvm::Value* halfBase = builder.CreateCall(fixMulFunc, {ConstantInt::get(quantizedType, FRAC_BASE / 2), x});
//
//	// Step 2: Convert x to floating-point and perform the initial approximation
//	llvm::Value* fp_y = builder.CreateSIToFP(x, llvm::Type::getFloatTy(irModule->getContext()));
//	llvm::Value* i = builder.CreateBitCast(fp_y, llvm::Type::getInt32Ty(irModule->getContext()));
//	i = builder.CreateSub(ConstantInt::get(llvm::Type::getInt32Ty(irModule->getContext()), 0x5f3759df), builder.CreateLShr(i, 1));
//	fp_y = builder.CreateBitCast(i, llvm::Type::getFloatTy(irModule->getContext()));
//
//	// Step 3: int_y = fp_y * FRAC_BASE;
//	llvm::Value* int_y = builder.CreateFPToSI(builder.CreateFMul(fp_y, ConstantFP::get(llvm::Type::getFloatTy(irModule->getContext()), FRAC_BASE)), quantizedType);
//
//	// Step 4: int_y = mulfix(int_y, ((int32_t)(1.5f * FRAC_BASE) - (mulfix(mulfix(int_halfx, int_y), int_y))));
//	llvm::Value* mulfix1 = builder.CreateCall(fixMulFunc, {halfBase, int_y});
//	llvm::Value* mulfix2 = builder.CreateCall(fixMulFunc, {mulfix1, int_y});
//	llvm::Value* correction = builder.CreateSub(ConstantInt::get(quantizedType, static_cast<int>(1.5f * FRAC_BASE)), mulfix2);
//	llvm::Value* final_y = builder.CreateCall(fixMulFunc, {int_y, correction});
//
//
//	// Return the final fixed-point result
//	builder.CreateRet(final_y);
//	functionsToInsert.emplace_back(func);
//
//	return func;
//}

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

	// uto quantizeNumValue = ConstantInt::get(quantizedType, round(compensateNum), true);
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
				auto quantizedConst = ConstantInt::get(quantizedType, quantizedValue);
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
handleFAdd(Instruction * inInstruction, Type * quantizedType)
{
	llvm::errs() << "Handling FAdd\n";
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

	// Create fixed-point addition
	Value * newInst = Builder.CreateAdd(op0, op1);

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
	Value * newInst = Builder.CreateSub(op0, op1);

	// Replace the original FSub instruction with the new fixed-point subtraction
	inInstruction->replaceAllUsesWith(newInst);
	inInstruction->eraseFromParent();

	llvm::errs() << "Finished handling FSub\n";
}

void
handleMatrixOperations(Instruction * instr, Type * quantizedType)
{
	if (instr->getOpcode() == Instruction::FAdd || instr->getOpcode() == Instruction::FMul)
	{
		llvm::errs() << "Handling matrix operation: " << *instr << "\n";
		IRBuilder<> Builder(instr);

		Value * lhs = instr->getOperand(0);
		Value * rhs = instr->getOperand(1);

		if (lhs->getType()->isPointerTy() && rhs->getType()->isPointerTy())
		{
			auto * lhsElemType = lhs->getType()->getPointerElementType();
			auto * rhsElemType = rhs->getType()->getPointerElementType();

			if ((lhsElemType->isArrayTy() && lhsElemType->getArrayElementType()->isFloatTy()) ||
			    (rhsElemType->isArrayTy() && rhsElemType->getArrayElementType()->isFloatTy()))
			{
				llvm::errs() << "Quantizing matrix elements\n";

				// 替换矩阵元素操作为整数操作
				for (unsigned i = 0; i < lhsElemType->getArrayNumElements(); ++i)
				{
					Value * lhsElem = Builder.CreateLoad(lhsElemType->getArrayElementType(), Builder.CreateGEP(lhsElemType, lhs, {Builder.getInt32(0), Builder.getInt32(i)}));
					Value * rhsElem = Builder.CreateLoad(rhsElemType->getArrayElementType(), Builder.CreateGEP(rhsElemType, rhs, {Builder.getInt32(0), Builder.getInt32(i)}));

					if (instr->getOpcode() == Instruction::FAdd)
					{
						Value * resultElem = Builder.CreateAdd(lhsElem, rhsElem);
						Builder.CreateStore(resultElem, Builder.CreateGEP(quantizedType->getArrayElementType(), lhs, {Builder.getInt32(0), Builder.getInt32(i)}));
					}
					else if (instr->getOpcode() == Instruction::FMul)
					{
						Value * resultElem = Builder.CreateMul(lhsElem, rhsElem);
						Builder.CreateStore(resultElem, Builder.CreateGEP(quantizedType->getArrayElementType(), lhs, {Builder.getInt32(0), Builder.getInt32(i)}));
					}
				}

				instr->eraseFromParent();
			}
		}
	}
}

Value *
convertToQuantizedType(Value * value, Type * quantizedType, IRBuilder<> & Builder)
{
	if (value->getType()->isFloatTy() || value->getType()->isDoubleTy())
	{
		// 如果是浮点类型，首先乘以 FRAC_BASE，然后转换为量化后的整数类型
		Value * scaledValue = Builder.CreateFMul(value, ConstantFP::get(value->getType(), FRAC_BASE));
		return Builder.CreateFPToSI(scaledValue, quantizedType);
	}
	else if (value->getType()->isIntegerTy())
	{
		// 如果已经是整数类型，则直接返回
		return value;
	}
	else
	{
		llvm::errs() << "Unsupported type for quantization: " << *value->getType() << "\n";
		return nullptr;
	}
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
		//		if (isa<llvm::Constant>(lhs) || isa<llvm::Constant>(rhs))
		//		{
		//			llvm::errs() << "One of the operands is a constant, simplifying...\n";
		//			handleConstant(llvmIrInstruction, quantizedType);
		//		}

		// Check if one of the operands is a constant integer
		if (isa<llvm::ConstantInt>(lhs) || isa<llvm::ConstantInt>(rhs))
		{
			llvm::errs() << "One of the operands is a constant integer, simplifying...\n";
			// If either operand is an integer constant, no need to multiply by FRAC_BASE
			Value * newInst = Builder.CreateMul(lhs, rhs);
			llvmIrInstruction->replaceAllUsesWith(newInst);
			llvmIrInstruction->eraseFromParent();
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
handleFNeg(Instruction * inInstruction, Type * quantizedType)
{
	llvm::errs() << "Handling FNeg\n";
	IRBuilder<> Builder(inInstruction);

	// Get the operand for the FNeg operation
	Value * operand = inInstruction->getOperand(0);

	// Process the operand and quantize constants
	quantizeConstant(inInstruction, quantizedType);

	// If the operand is a floating-point type, quantize it
	if (operand->getType()->isFloatingPointTy())
	{
		operand = Builder.CreateFMul(operand, ConstantFP::get(operand->getType(), -1.0));
		operand = Builder.CreateFPToSI(operand, quantizedType);
	}

	// Perform the negation in fixed-point arithmetic
	// Fixed-point negation is equivalent to integer negation
	Value * newInst = Builder.CreateNeg(operand);

	// Replace the original FNeg instruction with the new fixed-point negation
	inInstruction->replaceAllUsesWith(newInst);
	inInstruction->eraseFromParent();

	llvm::errs() << "Finished handling FNeg\n";
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

		// 递归转换所有层级的类型为量化类型
		Type * newAllocaType = transformToQuantizedType(allocaType, quantizedType);

		llvm::errs() << "Original alloca type: " << *allocaType << "\n";
		llvm::errs() << "New quantized alloca type: " << *newAllocaType << "\n";

		// Set the new quantized type for the alloca instruction
		llvmIrAllocaInstruction->setAllocatedType(newAllocaType);
	}
}

// void handleAlloca(Instruction * llvmIrInstruction, Type * quantizedType) {
//	if (auto llvmIrAllocaInstruction = dyn_cast<AllocaInst>(llvmIrInstruction)) {
//		// 获取原始的分配类型
//		Type * allocaType = llvmIrAllocaInstruction->getAllocatedType();
//
//		// 输出原始类型和新量化类型的调试信息
//		llvm::errs() << "Original alloca type: " << *allocaType << "\n";
//		llvm::errs() << "New quantized alloca type: " << *quantizedType << "\n";
//
//		Type * newAllocaType;
//
//		// 递归转换所有层级的类型为量化类型
//		if (allocaType->isFloatTy()) {
//			// 如果分配类型是浮点类型，转换为量化的整数类型
//			newAllocaType = Type::getInt32Ty(llvmIrInstruction->getContext());
//		} else if (allocaType->isPointerTy()) {
//			// 如果分配类型是指针类型，保持其原有分配
//			newAllocaType = allocaType;
//		} else {
//			// 其他类型，保持不变
//			newAllocaType = allocaType;
//		}
//
//		// 设置新的量化类型
//		llvmIrAllocaInstruction->setAllocatedType(newAllocaType);
//	}
// }

void
handleCall(CallInst * llvmIrCallInstruction, Type * quantizedType, std::vector<llvm::Function *> & functionsToInsert, llvm::Function * fixsqrt)
{
	Function * calledFunction = llvmIrCallInstruction->getCalledFunction();
	if (calledFunction == nullptr || !calledFunction->hasName() || calledFunction->getName().empty())
		return;

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

			std::string funcName = calledFunction->getName().str();

			if (funcName == "sqrt" || funcName == "sqrtf")
			{
				// For sqrt
				handleSqrtCall(llvmIrCallInstruction, quantizedType, fixsqrt);
			}

			//			else if (funcName == "pow" || funcName == "powf" || funcName == "powl")
			//			{
			//				// For pow
			//				handlePowCall(llvmIrCallInstruction, quantizedType, functionsToInsert);
			//			}

			//			if (calledFunction && calledFunction->getName() == "rsqrt") {
			//				Function* fixrsqrt = createFixRsqrt(irModule, quantizedType);
			//				handleRsqrtCall(llvmIrCallInstruction, quantizedType, fixrsqrt);
			//			}

			else if (funcName == "sin")
			{
				// For sin
				handleSinCall(llvmIrCallInstruction, quantizedType);
			}

			else if (funcName == "llvm.memcpy.p0i8.p0i8.i64" || funcName == "llvm.memcpy.p0i8.p0i8.i32")
			{
				// For llvm.memcpy
				handleMemCpyCall(llvmIrCallInstruction, quantizedType);
			}
			else
			{
				/*
				 * for other lib functions, de-quantize the arguments and quantize the return value
				 */
				dequantizeArgumentsAndQuantizeReturn(llvmIrCallInstruction, quantizedType);
			}
		}
		else
		{
			// For user-defined functions, only handle the arguments
			for (size_t idx = 0; idx < llvmIrCallInstruction->getNumArgOperands(); ++idx)
			{
				Value * arg = llvmIrCallInstruction->getArgOperand(idx);
				if (arg->getType()->isPointerTy())
				{
					Type * elementType = arg->getType()->getPointerElementType();
					if (elementType->isFloatTy() || elementType->isDoubleTy())
					{
						// Do not change pointer type, handle it when dereferenced
						continue;
					}
				}
				// Set quantized type for the argument if it is not a pointer
				setQuantizedType(arg, quantizedType);
			}

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

// Function to handle loads involving pointer types
void
handlePointerLoad(LoadInst * llvmIrLoadInstruction, IRBuilder<> & Builder, Type * quantizedType)
{
	auto pointerOperand = llvmIrLoadInstruction->getPointerOperand();
	auto pointerType    = pointerOperand->getType()->getPointerElementType();

	if (pointerType->isPointerTy())
	{
		auto pointedType = pointerType->getPointerElementType();

		if (pointedType->isPointerTy())
		{
			auto innerPointedType = pointedType->getPointerElementType();
			if (innerPointedType->isFloatTy() || innerPointedType->isDoubleTy())
			{
				// Update pointer type for double** or float** to quantizedType** (e.g., int32_t**)
				auto quantizedInnerPointerType = quantizedType->getPointerTo();
				auto quantizedPointerType      = PointerType::get(quantizedInnerPointerType, pointerType->getPointerAddressSpace());
				pointerOperand->mutateType(quantizedPointerType);

				// Load the inner pointer value
				auto loadedPointerValue = Builder.CreateLoad(quantizedPointerType, pointerOperand);

				// Handle uses of the loaded pointer value
				for (auto & use : llvmIrLoadInstruction->uses())
				{
					auto user = use.getUser();
					if (auto storeInst = dyn_cast<StoreInst>(user))
					{
						if (storeInst->getValueOperand() == llvmIrLoadInstruction)
						{
							// If storing to a float or double, convert the loaded pointer value
							auto storeType = storeInst->getPointerOperand()->getType()->getPointerElementType();
							if (storeType->isFloatTy())
							{
								auto floatValue = Builder.CreateSIToFP(loadedPointerValue, Type::getFloatTy(llvmIrLoadInstruction->getContext()));
								storeInst->setOperand(0, floatValue);
							}
							else if (storeType->isDoubleTy())
							{
								auto doubleValue = Builder.CreateSIToFP(loadedPointerValue, Type::getDoubleTy(llvmIrLoadInstruction->getContext()));
								storeInst->setOperand(0, doubleValue);
							}
							else
							{
								storeInst->setOperand(0, loadedPointerValue);
							}
						}
					}
				}

				// Replace all uses of the original load instruction with the new loaded pointer value
				llvmIrLoadInstruction->replaceAllUsesWith(loadedPointerValue);
				llvmIrLoadInstruction->eraseFromParent();
				return;
			}
		}
	}
}

void
handleLoad(Instruction * llvmIrInstruction, Type * quantizedType)
{
	if (auto llvmIrLoadInstruction = dyn_cast<LoadInst>(llvmIrInstruction))
	{
		IRBuilder<> Builder(llvmIrLoadInstruction);
		auto	    pointerOperand = llvmIrLoadInstruction->getPointerOperand();
		auto	    pointerType	   = pointerOperand->getType()->getPointerElementType();

		// Print debug information
		llvm::errs() << "Original load pointer type: " << *pointerType << "\n";
		llvm::errs() << "New quantized load pointer type: " << *quantizedType << "\n";

		// Handle pointer to pointer (e.g., double**)
		if (pointerType->isPointerTy())
		{
			handlePointerLoad(llvmIrLoadInstruction, Builder, quantizedType);
			return;
		}

		// Handle i64 type separately
		if (pointerType->isIntegerTy(64))
		{
			// For i64* types, do not modify or handle this load instruction
			llvm::errs() << "Skipping i64 load instruction: " << *llvmIrLoadInstruction << "\n";
			return;	 // Skip further processing for i64 types
		}

		// Update pointer operand's type if necessary
		if (pointerType->isFloatTy() || pointerType->isDoubleTy())
		{
			pointerOperand->mutateType(quantizedType->getPointerTo());
		}

		// Load the value using the quantized type
		auto loadedValue = Builder.CreateLoad(quantizedType, llvmIrLoadInstruction->getPointerOperand());

		// Handle uses of the loaded value
		for (auto & use : llvmIrLoadInstruction->uses())
		{
			auto user = use.getUser();
			if (auto storeInst = dyn_cast<StoreInst>(user))
			{
				if (storeInst->getValueOperand() == llvmIrLoadInstruction)
				{
					// If storing to a float or double, convert the loaded i64 to float/double
					auto storeType = storeInst->getPointerOperand()->getType()->getPointerElementType();
					if (storeType->isFloatTy())
					{
						auto floatValue = Builder.CreateSIToFP(loadedValue, Type::getFloatTy(llvmIrInstruction->getContext()));
						storeInst->setOperand(0, floatValue);
					}
					else if (storeType->isDoubleTy())
					{
						auto doubleValue = Builder.CreateSIToFP(loadedValue, Type::getDoubleTy(llvmIrInstruction->getContext()));
						storeInst->setOperand(0, doubleValue);
					}
					else
					{
						storeInst->setOperand(0, loadedValue);
					}
				}
			}
			else
			{
				llvm::errs() << "Pointer type " << *pointerType << " is not supported for quantization.\n";
			}
		}

		// Replace all uses of the original load instruction with the new loaded value
		llvmIrLoadInstruction->replaceAllUsesWith(loadedValue);
		llvmIrLoadInstruction->eraseFromParent();
	}
}

void
handleGetElementPtr(GetElementPtrInst * gepInst, Type * quantizedType)
{
	llvm::errs() << "Handling GetElementPtr instruction: " << *gepInst << "\n";
	IRBuilder<> Builder(gepInst);

	auto sourceElementType = gepInst->getSourceElementType();
	auto resultType	       = gepInst->getResultElementType();

	if (sourceElementType->isArrayTy())
	{
		Type * elementType = sourceElementType->getArrayElementType();

		if (elementType->isFloatTy() || elementType->isDoubleTy())
		{
			llvm::errs() << "Quantizing element type: " << *elementType << "\n";

			// 量化后的元素类型
			Type * newElementType = quantizedType;

			// 更新GEP指令的源和结果类型
			Type * newSourceType = ArrayType::get(newElementType, sourceElementType->getArrayNumElements());
			gepInst->setSourceElementType(newSourceType);
			gepInst->setResultElementType(newElementType);

			// 更新索引操作数类型
			for (unsigned i = 0; i < gepInst->getNumIndices(); ++i)
			{
				Value * index = gepInst->getOperand(i + 1);
				if (index->getType()->isIntegerTy() && index->getType()->getIntegerBitWidth() != quantizedType->getIntegerBitWidth())
				{
					gepInst->setOperand(i + 1, Builder.CreateIntCast(index, quantizedType, true));
				}
			}
		}
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

// void
// handleFPTrunc(Instruction * llvmIrInstruction, Type * quantizedType)
//{
//	llvm::errs() << "Handling FPTrunc: " << *llvmIrInstruction << "\n";
//	auto * fptruncInst = cast<FPTruncInst>(llvmIrInstruction);
//	auto   srcType	   = fptruncInst->getSrcTy();
//	auto   destType	   = fptruncInst->getDestTy();
//
//	Value *operand = llvmIrInstruction->getOperand(0);
//
//	if (srcType->isFloatingPointTy() && destType->isFloatingPointTy())
//	{
//		// Erase the unnecessary FPTrunc instruction
//		llvmIrInstruction->replaceAllUsesWith(fptruncInst->getOperand(0));
//		llvmIrInstruction->eraseFromParent();
//		llvm::errs() << "Removed unnecessary FPTrunc\n";
//	}
//
//	if (operand->getType()->isIntegerTy())
//	{
//		llvmIrInstruction->replaceAllUsesWith(operand);
//		llvmIrInstruction->eraseFromParent();
//		llvm::errs() << "Removed unnecessary FPTrunc for integer operand\n";
//	}else{
//		llvm::errs() << "Unhandled FPTrunc conversion from " << *srcType << " to " << *destType << "\n";
//	}
// }

void
handleFPTrunc(Instruction * llvmIrInstruction, Type * quantizedType)
{
	llvm::errs() << "Handling FPTrunc: " << *llvmIrInstruction << "\n";
	IRBuilder<> Builder(llvmIrInstruction);

	// Insert point is after the current instruction
	Instruction * insertPoint = llvmIrInstruction->getNextNode();
	Builder.SetInsertPoint(insertPoint);

	// Get the operand of the FPTrunc instruction
	Value * operand = llvmIrInstruction->getOperand(0);
	Value * newInst = nullptr;

	// Check if the operand is an integer or a floating-point type
	if (operand->getType()->isIntegerTy())
	{
		// If it's an integer, convert it to a floating-point value
		newInst = Builder.CreateSIToFP(operand, llvmIrInstruction->getType());
	}
	else
	{
		// Otherwise, perform the FPTrunc as a floating-point cast
		newInst = Builder.CreateFPCast(operand, llvmIrInstruction->getType());
	}

	// Replace all uses of the original FPTrunc with the new instruction
	llvmIrInstruction->replaceAllUsesWith(newInst);

	// Remove the original FPTrunc instruction from the parent
	llvmIrInstruction->removeFromParent();

	llvm::errs() << "Finished handling FPTrunc\n";
}

void
handleBitCast(Instruction * llvmIrInstruction, Type * quantizedType)
{
	if (auto bitcastInst = dyn_cast<BitCastInst>(llvmIrInstruction))
	{
		IRBuilder<> Builder(bitcastInst);

		// 获取源类型和目标类型
		auto srcType  = bitcastInst->getSrcTy();
		auto destType = bitcastInst->getDestTy();

		// 转换源类型
		if (srcType->isPointerTy())
		{
			auto originalElementType = srcType->getPointerElementType();
			auto newElementType	 = transformToQuantizedType(originalElementType, quantizedType);
			srcType			 = newElementType->getPointerTo();
		}

		// 如果目标类型是浮点类型的指针，则转换为量化类型
		if (destType->isPointerTy() && destType->getPointerElementType()->isFloatTy())
		{
			destType = quantizedType->getPointerTo();
		}

		// 输出日志信息
		llvm::errs() << "Original srcType: " << *bitcastInst->getSrcTy() << "\n";
		llvm::errs() << "Original destType: " << *bitcastInst->getDestTy() << "\n";
		llvm::errs() << "Transformed srcType: " << *srcType << "\n";
		llvm::errs() << "Transformed destType: " << *destType << "\n";

		// 创建新的BitCast指令
		Value * newInst = Builder.CreateBitCast(bitcastInst->getOperand(0), destType);
		bitcastInst->replaceAllUsesWith(newInst);
		bitcastInst->eraseFromParent();

		llvm::errs() << "Created new BitCast: " << *newInst << "\n";
		llvm::errs() << "Erased original BitCast\n";
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
					//					IRBuilder<>   Builder(llvmIrInstruction);
					//					Instruction * insertPoint = llvmIrInstruction->getNextNode();
					//					Builder.SetInsertPoint(insertPoint);
					//					Value * newInst = Builder.CreateBitCast(llvmIrInstruction->getOperand(0), llvmIrInstruction->getType());
					//					llvmIrInstruction->replaceAllUsesWith(newInst);
					//					llvmIrInstruction->removeFromParent();
					llvm::errs() << "handle bitcast\n";
					handleBitCast(llvmIrInstruction, quantizedType);
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



	// Usage in the original function
	std::string functionName = llvmIrFunction.getName().str();
	if (shouldSkipFunction(functionName)) {
		llvm::errs() << "Should Skipping function: " << functionName << "\n";
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

	llvm::Function * fixdiv	 = createFixDiv(module, quantizedType, functionsToInsert);
	llvm::Function * fixmul	 = createFixMul(module, quantizedType, functionsToInsert);
	llvm::Function * fixsqrt = createFixSqrt(module, quantizedType, functionsToInsert);
	// fixpow
	// llvm::Function * fixpow = createFixPow(module, quantizedType, functionsToInsert);

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
					handleCall(cast<CallInst>(llvmIrInstruction), quantizedType, functionsToInsert, fixsqrt);
					break;
				case Instruction::GetElementPtr:

					handleGetElementPtr(cast<GetElementPtrInst>(llvmIrInstruction), quantizedType);
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
					if (isMatrixOperation(llvmIrInstruction))
					{
						llvm::errs() << "Found matrix multiplication operation.\n";
						handleMatrixOperations(llvmIrInstruction, quantizedType);
					}
					else
					{
						llvm::errs() << "Found FMul instruction.\n";
						handleFMul(llvmIrInstruction, quantizedType, fixmul);
					}
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
					if (isMatrixOperation(llvmIrInstruction))
					{
						llvm::errs() << "Found matrix addition operation.\n";
						handleMatrixOperations(llvmIrInstruction, quantizedType);
					}
					else
					{
						llvm::errs() << "Found FAdd instruction.\n";
						handleFAdd(llvmIrInstruction, quantizedType);
					}
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
					handleFNeg(llvmIrInstruction, quantizedType);

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
					// log
					llvm::errs() << "handle bitcast\n";
					handleBitCast(llvmIrInstruction, quantizedType);
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
}