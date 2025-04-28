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

#ifdef __cplusplus
#include "newton-irPass-LLVMIR-rangeAnalysis.h"
#include "newton-irPass-LLVMIR-simplifyControlFlowByRange.h"
#include "newton-irPass-LLVMIR-constantSubstitution.h"
#include "newton-irPass-LLVMIR-shrinkTypeByRange.h"
#include "newton-irPass-LLVMIR-quantization.h"
#include "newton-irPass-LLVMIR-optimizeByRange.h"
#include "newton-irPass-LLVMIR-memoryAlignment.h"
#include "newton-irPass-LLVMIR-emitAssume.h"


#endif /* __cplusplus */

#include <algorithm>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdint.h>
#include <string.h>
#include <set>

#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Path.h"

#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/IR/Function.h"

#include "config.h"

#include <unordered_map>
#include <set>
#include <limits>

using namespace llvm;
// #define FRAC_BASE (1 << maxPrecisionBits)
#define FRAC_BASE (1 << MAX_PRECISION_BITS)

void
checkOverflow(State * N, BoundInfo * boundInfo, int FRAC_Q)
{
	int maxVal, minVal;
	if (BIT_WIDTH == 16)
	{
		maxVal = INT16_MAX;
		minVal = INT16_MIN;
	}
	else if (BIT_WIDTH == 32)
	{
		maxVal = INT32_MAX;
		minVal = INT32_MIN;
	}
	else
	{
		flexprint(N->Fe, N->Fm, N->Fperr, "Unsupported BIT_WIDTH: %d\n", BIT_WIDTH);
		return;
	}

	for (const auto & entry : boundInfo->virtualRegisterRange)
	{
		double scaledMin = entry.second.first * FRAC_BASE;
		double scaledMax = entry.second.second * FRAC_BASE;

		std::string instStr = "unknown";
		if (Instruction * inst = dyn_cast<Instruction>(entry.first))
		{
			instStr = inst->getOpcodeName();
		}

		if ((scaledMin > maxVal && scaledMax > maxVal) || (scaledMin < minVal && scaledMax < minVal))
		{
			flexprint(N->Fe, N->Fm, N->Fperr,
				  "Definite overflow detected: %s range [%f, %f] when scaled by 2^%d is completely outside int%d bounds\n",
				  instStr.c_str(), entry.second.first, entry.second.second,
				  FRAC_Q, BIT_WIDTH);
		}
		else if (scaledMax > maxVal || scaledMin < minVal)
		{
			flexprint(N->Fe, N->Fm, N->Fperr,
				  "Possible overflow detected: %s range [%f, %f] when scaled by 2^%d partially exceeds int%d bounds\n",
				  instStr.c_str(), entry.second.first, entry.second.second,
				  FRAC_Q, BIT_WIDTH);
		}
	}
}

void
autoWhitelistFunctions(Module &Mod, std::set<std::string> &whitelist)
{
	for (Function &F : Mod)
	{
		if (F.isDeclaration()) continue;

		bool hasSensorParams = false;

		for (auto &Arg : F.args())
		{
			if (Arg.hasName())
			{
				std::string argName = Arg.getName().str();
				// 这里你可以加更丰富的匹配，比如查 typedef 类型名（需要调试信息）
				if (argName.find("bmx055") != std::string::npos)
				{
					hasSensorParams = true;
					break;
				}
			}
		}

		if (hasSensorParams)
		{
			llvm::errs() << "Auto-Whitelisting function based on sensor params: " << F.getName() << "\n";
			whitelist.insert(F.getName().str());
		}
	}
}



// #define IS_POINTER 1
std::set<std::string> whitelist = {
    "MadgwickAHRSupdate",
    "MahonyAHRSupdate",
    "sensfusion6UpdateQImpl",
    "matrixMul",
    "pzero",
    "qzero",
    "pone",
    "qone",
    "__ieee754_exp"};


//void eraseUnusedConstant(Module &M) {
//	std::set<std::string> quantizedGlobals;
//
//	// First pass: collect all _quantized names
//	for (auto &GV : M.globals()) {
//		if (GV.getName().endswith("_quantized")) {
//			std::string baseName = GV.getName().str().substr(0, GV.getName().size() - 10);
//			quantizedGlobals.insert(baseName);
//		}
//	}
//	// Second pass: delete unused original globals
//	std::vector<GlobalVariable *> toDelete;
//	for (auto &GV : M.globals()) {
//		std::string name = GV.getName().str();
//		if (GV.use_empty() && quantizedGlobals.count(name)) {
//			toDelete.push_back(&GV);
//		}
//	}
//	for (auto *GV : toDelete) {
//		GV->eraseFromParent();
//	}
//}
void eraseUnusedConstant(Module &M) {
	std::set<std::string> quantizedBaseNames;
	for (auto &GV : M.globals()) {
		if (GV.getName().endswith("_quantized")) {
			std::string baseName = GV.getName().str().substr(0, GV.getName().size() - 10); // remove "_quantized"
			quantizedBaseNames.insert(baseName);
		}
	}
	std::vector<GlobalVariable *> toDelete;
	for (auto &GV : M.globals()) {
		std::string name = GV.getName().str();

		if (GV.use_empty() && quantizedBaseNames.count(name)) {
			toDelete.push_back(&GV);
		}
		if (GV.use_empty() && name.size() > 10 && name.substr(name.size() - 10) == "_quantized") {
			toDelete.push_back(&GV);
		}
	}
	for (auto *GV : toDelete) {
		GV->eraseFromParent();
	}
}




void
handleGlobalStore(StoreInst * storeInst, IRBuilder<> & Builder, int maxPrecisionBits)
{
	auto * pointerOperand = storeInst->getPointerOperand();

	// Ensure the operation is on a global variable
	if (auto * quantizedGlobalVar = dyn_cast<GlobalVariable>(pointerOperand))
	{
		llvm::errs() << "Processing quantized global variable: " << quantizedGlobalVar->getName() << "\n";

		// Identify the corresponding original global variable (e.g., remove "_quantized" suffix)
		std::string originalName = quantizedGlobalVar->getName().str();
		if (originalName.size() > 10 && originalName.compare(originalName.size() - 10, 10, "_quantized") == 0)
		{
			originalName = originalName.substr(0, originalName.size() - 10);
		}
		else
		{
			llvm::errs() << "Skipping: No matching original global for " << quantizedGlobalVar->getName() << "\n";
			return;
		}

		// Find the original global variable
		GlobalVariable * originalGlobalVar = quantizedGlobalVar->getParent()->getNamedGlobal(originalName);
		if (!originalGlobalVar || !originalGlobalVar->getType()->getElementType()->isFloatingPointTy())
		{
			llvm::errs() << "Skipping: Original global variable not found or not floating-point: " << originalName << "\n";
			return;
		}

		llvm::errs() << "Found corresponding original global variable: " << originalGlobalVar->getName() << "\n";

		// Check if the previous instruction is `trunc`
		Instruction * prevInst = storeInst->getPrevNode();
		if (!prevInst || !isa<TruncInst>(prevInst))
		{
			llvm::errs() << "Skipping: Previous instruction is not trunc.\n";
			return;
		}

		// Load the integer value from the quantized global variable
		auto * loadInst = Builder.CreateLoad(quantizedGlobalVar->getType()->getPointerElementType(), quantizedGlobalVar);

		// Convert the integer value to a floating-point value
		Value * convertedFloat = Builder.CreateSIToFP(loadInst, Type::getFloatTy(storeInst->getContext()));

		// Perform dequantization
		Value * dequantizedValue = Builder.CreateFMul(
		    convertedFloat, ConstantFP::get(Type::getFloatTy(storeInst->getContext()), 1.0 / FRAC_BASE));

		// Store the dequantized floating-point value back into the original global variable
		Builder.CreateStore(dequantizedValue, originalGlobalVar);

		llvm::errs() << "Dequantized and stored value for original global variable: " << originalGlobalVar->getName() << "\n";
	}
	else
	{
		llvm::errs() << "Pointer operand is not a global variable. Skipping.\n";
	}
}

void
handlePointerStore(StoreInst * storeInst, IRBuilder<> & Builder, int maxPrecisionBits)
{
	auto * pointerOperand = storeInst->getPointerOperand();

	if (!pointerOperand->getType()->getPointerElementType()->isIntegerTy(BIT_WIDTH))
	{
		llvm::errs() << "Pointer operand type is not an integer of expected bit width.\n";
		return;
	}

	auto * loadInst = Builder.CreateLoad(pointerOperand->getType()->getPointerElementType(), pointerOperand);
	if (isa<GlobalVariable>(loadInst->getPointerOperand()))
	{
		llvm::errs() << "Skipping StoreInst due to global variable in load operand.\n";
		return;
	}

	Value * convertedFloat = Builder.CreateSIToFP(loadInst, Type::getFloatTy(storeInst->getContext()));
	Value * dividedValue   = Builder.CreateFMul(
	      convertedFloat, ConstantFP::get(Type::getFloatTy(storeInst->getContext()), 1.0 / FRAC_BASE));

	if (auto * bitcastInst = dyn_cast<BitCastInst>(pointerOperand))
	{
		Value * finalStorePtr = nullptr;
		bool	isValidSource = false;
		llvm::errs() << "BIT_WIDTH: " << BIT_WIDTH << "\n";
		// Determine the final store pointer based on bit width
		switch (BIT_WIDTH)

		{
			case 16:
				if (bitcastInst->getSrcTy()->getPointerElementType()->isIntegerTy(32))
				{
					auto * i32Ptr = bitcastInst->getOperand(0);
					if (auto * floatBitcast = dyn_cast<BitCastInst>(i32Ptr))
					{
						if (floatBitcast->getSrcTy()->getPointerElementType()->isFloatTy())
						{
							finalStorePtr = floatBitcast->getOperand(0);  // Original float*
							isValidSource = true;
						}
					}
				}
				break;

			case 32:
				if (bitcastInst->getSrcTy()->getPointerElementType()->isFloatTy())
				{
					finalStorePtr = bitcastInst->getOperand(0);  // Original float*
					isValidSource = true;
				}
				break;

			default:
				llvm::errs() << "Unsupported BIT_WIDTH: " << BIT_WIDTH << "\n";
				return;
		}

		if (isValidSource && finalStorePtr)
		{
			Builder.CreateStore(dividedValue, finalStorePtr);
			llvm::errs() << "Dequantized and stored value for pointer.\n";
		}
		else
		{
			llvm::errs() << "Invalid source for StoreInst: " << *storeInst << "\n";
		}
	}
}

void
handleMatrixStore(StoreInst * storeInst, IRBuilder<> & Builder, int maxPrecisionBits)
{
	Value * valueOperand   = storeInst->getValueOperand();
	Value * pointerOperand = storeInst->getPointerOperand();

	// Ensure the stored value is an integer and the destination is a float pointer
	Type * valueType	  = valueOperand->getType();
	Type * pointerElementType = pointerOperand->getType()->getPointerElementType();

	if (valueType->isIntegerTy() && pointerElementType->isFloatingPointTy())
	{
		llvm::errs() << "Processing matrix store (quantized to dequantized): " << *storeInst << "\n";

		// Convert integer value to floating-point (dequantization step 1)
		llvm::errs() << "Converting integer to float: " << *valueOperand << "\n";
		Value * convertedFloat = Builder.CreateSIToFP(valueOperand, Type::getFloatTy(storeInst->getContext()), storeInst->getName() + ".dequantized");

		// Perform dequantization by multiplying by (1 / fracBase)
		Value * dequantizedValue = Builder.CreateFMul(
		    convertedFloat, ConstantFP::get(Type::getFloatTy(storeInst->getContext()), 1.0 / FRAC_BASE), storeInst->getName() + ".scaled_back");

		// Store the dequantized floating-point value back to the original float memory location
		Builder.CreateStore(dequantizedValue, pointerOperand);

		llvm::errs() << "Dequantized and stored float value at: " << *pointerOperand << "\n";

		// Remove the original store instruction
		storeInst->eraseFromParent();
	}
	else
	{
		llvm::errs() << "Skipping store: Not storing i32 into float*.\n";
	}
}

void
handleReturnValue(ReturnInst * retInst, int maxPrecisionBits)
{
	if (!retInst->getReturnValue())
		return;

	Value * retVal = retInst->getReturnValue();

	if (!retVal->getType()->isIntegerTy())
	{
		errs() << "Return value is not integer type, skipping dequantization.\n";
		return;
	}

	IRBuilder<> Builder(retInst);
	Type *	    targetType = Type::getDoubleTy(retInst->getContext());

	Value * fpVal = Builder.CreateSIToFP(retVal, targetType);

	llvm::Constant * oneDivFrac = llvm::ConstantFP::get(targetType, 1.0 / FRAC_BASE);

	Value * dequantizedVal = Builder.CreateFMul(fpVal, oneDivFrac);
	ReturnInst * newRet = ReturnInst::Create(retInst->getContext(), dequantizedVal, retInst);

	retInst->eraseFromParent();

	errs() << "Replaced return with dequantized value: " << *newRet << "\n";
}

void
dequantizeResults(StoreInst * storeInst, Function & F, int maxPrecisionBits)
{
	IRBuilder<> Builder(storeInst->getNextNode());
	llvm::errs() << "Processing StoreInst in function: " << F.getName() << " | Store instruction: " << *storeInst << "\n";

#if IS_MATRIX
	handleMatrixStore(storeInst, Builder, maxPrecisionBits);
#elif IS_POINTER
	llvm::errs() << "Handling pointer store.\n";
	handlePointerStore(storeInst, Builder, maxPrecisionBits);

#else
	handleGlobalStore(storeInst, Builder, maxPrecisionBits);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool enableAutoQuantization = false;

void
detectFloatingPointOps(Module & Mod)
{
	bool			   hasFloatOps = false;
	std::map<std::string, int> floatOpCounts;  // Map to store the count of floating-point operations per function

	for (auto & F : Mod)
	{
		int functionFloatOpCount = 0;  // Counter for floating-point operations in the current function

		// Analyze function parameters
		int			 paramCount  = 0;
		int			 returnCount = 0;
		std::vector<std::string> paramTypes;  // To store parameter types
		for (auto & Arg : F.args())
		{
			paramCount++;
			if (Arg.getType()->isPointerTy())
			{
				paramTypes.push_back("pointer");
			}
			else if (Arg.getType()->isFloatingPointTy())
			{
				paramTypes.push_back("floating-point");
			}
			else if (Arg.getType()->isIntegerTy())
			{
				paramTypes.push_back("integer");
			}
			else
			{
				paramTypes.push_back("unknown");
			}
		}

		// Analyze function return type
		std::string returnType = "void";  // Default return type
		if (!F.getReturnType()->isVoidTy())
		{
			if (F.getReturnType()->isPointerTy())
			{
				returnType = "pointer";
			}
			else if (F.getReturnType()->isFloatingPointTy())
			{
				returnType = "floating-point";
			}
			else if (F.getReturnType()->isIntegerTy())
			{
				returnType = "integer";
			}
			else
			{
				returnType = "unknown";
			}
		}

		for (auto & BB : F)
		{
			for (auto & I : BB)
			{
				// Check if the instruction is a floating-point operation
				if (I.getOpcode() == Instruction::FAdd ||
				    I.getOpcode() == Instruction::FMul ||
				    I.getOpcode() == Instruction::FSub ||
				    I.getOpcode() == Instruction::FDiv)
				{
					hasFloatOps = true;
					functionFloatOpCount++;
				}

				// Check if the instruction is a return
				if (isa<ReturnInst>(I))
				{
					returnCount++;
				}
			}
		}

		// Store the count for this function
		if (functionFloatOpCount > 0)
		{
			floatOpCounts[F.getName().str()] = functionFloatOpCount;
		}

		// Output function details
		llvm::errs() << "Function: " << F.getName() << "\n";
		llvm::errs() << "  Return Type: " << returnType << "\n";
		llvm::errs() << "  Parameter Count: " << paramCount << "\n";
		llvm::errs() << "  Parameter Types: ";
		for (const auto & type : paramTypes)
		{
			llvm::errs() << type << " ";
		}
		llvm::errs() << "\n";
		// f
	}

	// Output the results
	if (hasFloatOps)
	{
		llvm::errs() << "Floating-point operations detected in the module.\n";
		for (const auto & entry : floatOpCounts)
		{
			llvm::errs() << "Function: " << entry.first
				     << " - Floating-point operations: " << entry.second << "\n";
		}
		llvm::errs() << "Enabling Auto-Quantization.\n";
		enableAutoQuantization = true;
	}
	else
	{
		llvm::errs() << "No floating-point operations detected. Skipping Auto-Quantization.\n";
	}
}

void
checkFPUAvailability(Module & Mod)
{
	bool		      hasFPU = false;
	std::set<std::string> detectedFeatures;

	// Iterate over functions to check attributes
	for (auto & F : Mod)
	{
		if (F.hasFnAttribute("target-features"))
		{
			std::string features = F.getFnAttribute("target-features").getValueAsString().str();
			detectedFeatures.insert(features);

			// Check for x86 floating-point features
			if (features.find("+sse") != std::string::npos ||
			    features.find("+sse2") != std::string::npos ||
			    features.find("+avx") != std::string::npos ||
			    features.find("+x87") != std::string::npos ||
			    features.find("+fma") != std::string::npos)
			{
				hasFPU = true;
			}

			// Check for ARM floating-point features
			if (features.find("+vfp") != std::string::npos ||
			    features.find("+neon") != std::string::npos ||
			    features.find("+fp-armv8") != std::string::npos ||
			    features.find("+fp16") != std::string::npos)
			{
				hasFPU = true;
			}
		}
	}

	// Print detected target features (only once)
	if (!detectedFeatures.empty())
	{
		llvm::errs() << "Target Features: ";
		for (const auto & feature : detectedFeatures)
		{
			llvm::errs() << feature << " ";
		}
		llvm::errs() << "\n";
	}

	// Final decision based on FPU detection
	if (hasFPU)
	{
		llvm::errs() << "FPU detected via function attributes. \n";
	}
	else
	{
		llvm::errs() << "No FPU detected. Enabling Auto-Quantization.\n";
		enableAutoQuantization = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Process functions that are whitelisted for dequantization

void
processWhitelistedFunctions(Module & module, const std::set<std::string> & whitelist, int maxPrecisionBits)
{
	for (Function & F : module)
	{
		if (whitelist.find(F.getName().str()) != whitelist.end())
		{
			llvm::errs() << "Found whitelisted function: " << F.getName() << "\n";
			std::vector<ReturnInst *> retWorkList;
			for (BasicBlock & BB : F)
			{
				for (Instruction & I : BB)
				{
					if (ReturnInst * retInst = dyn_cast<ReturnInst>(&I))
					{
						if (retInst->getReturnValue() && retInst->getReturnValue()->getType()->isIntegerTy())
						{
							retWorkList.push_back(retInst);
						}
					}
				}
			}
			for (ReturnInst * retInst : retWorkList)
			{
				handleReturnValue(retInst, maxPrecisionBits);
			}

			for (BasicBlock & BB : F)
			{
				for (Instruction & I : BB)
				{
					// llvm::errs() << "Processing instruction: " << I << "\n";
					if (auto * storeInst = dyn_cast<StoreInst>(&I))
					{
						llvm::errs() << "Found valid StoreInst.\n";
						dequantizeResults(storeInst, F, maxPrecisionBits);
					}
				}
			}
		}
	}
}

// void
// processWhitelistedFunctions(Module & module, const std::set<std::string> & whitelist, int maxPrecisionBits)
//{
//	for (auto & F : module)
//	{
//		if (whitelist.find(F.getName().str()) != whitelist.end())
//		{
//			llvm::errs() << "Found whitelisted function: " << F.getName() << "\n";
//
//
//
//
//			for (auto & B : F)
//			{
//				for (auto & I : B)
//				{
//					llvm::errs() << "Processing instruction: " << I << "\n";
//					if (auto * storeInst = dyn_cast<StoreInst>(&I))
//					{
//						llvm::errs() << "Found valid StoreInst.\n";
//						dequantizeResults(storeInst, F, maxPrecisionBits);
//					}
//
//				}
//			}
//
//
//		}
//	}
//
// }

// Function to save the IR of a module to a file
void
saveModuleIR(llvm::Module & M, const std::string & fileName)
{
	std::error_code	     EC;
	llvm::raw_fd_ostream file(fileName, EC, llvm::sys::fs::OF_Text);
	if (EC)
	{
		llvm::errs() << "Error opening file " << fileName << " for writing: " << EC.message() << "\n";
		return;
	}
	M.print(file, nullptr);
	llvm::errs() << "IR saved to " << fileName << "\n";
	file.close();
}

void
removeQuantizedSuffixInModule(llvm::Module & M)
{
	for (auto & F : M)
	{
		if (F.hasName())
		{
			std::string FuncName = F.getName().str();
			size_t	    pos	     = FuncName.find("_quantized");
			if (pos != std::string::npos)
			{
				FuncName.erase(pos, 10);
				F.setName(FuncName);
			}
		}
	}

	// Remove suffix from global variables
	for (auto & G : M.globals())
	{
		if (G.hasName())
		{
			std::string GlobalName = G.getName().str();
			size_t	    pos	       = GlobalName.find("_quantized");
			if (pos != std::string::npos)
			{
				GlobalName.erase(pos, 10);  // Remove "_quantized"
				G.setName(GlobalName);
			}
		}
	}
}

double
computeResolution(Modality * mod)
{
	return (mod->rangeUpperBound - mod->rangeLowerBound) / (1 << mod->precisionBits);
}

void
dumpIR(State * N, std::string fileSuffix, const std::unique_ptr<Module> & Mod)
{
	StringRef   filePath(N->llvmIR);
	std::string dirPath	= std::string(sys::path::parent_path(filePath)) + "/";
	std::string fileName	= std::string(sys::path::stem(filePath)) + "_" + fileSuffix + ".bc";
	std::string filePathStr = dirPath + fileName;
	filePath		= StringRef(filePathStr);

	flexprint(N->Fe, N->Fm, N->Fpinfo, "Dump IR of: %s\n", filePath.str().c_str());
	std::error_code errorCode(errno, std::generic_category());
	raw_fd_ostream	dumpedFile(filePath, errorCode);
	WriteBitcodeToFile(*Mod, dumpedFile);
	dumpedFile.close();
}

// void dumpIR(State *N, std::string fileSuffix, const std::unique_ptr<Module> &Mod) {
//	StringRef filePath(N->llvmIR);
//	std::string dirPath = std::string(sys::path::parent_path(filePath)) + "/";
//	std::string fileName = std::string(sys::path::stem(filePath)) + "_" + fileSuffix + ".bc";
//	std::string filePathStr = dirPath + fileName;
//	filePath = StringRef(filePathStr);
//
//	// 输出调试信息：目标 IR 文件路径
//	flexprint(N->Fe, N->Fm, N->Fpinfo, "Dump IR of: %s\n", filePath.str().c_str());
//	llvm::errs() << "DumpIR: File path = " << filePath.str() << "\n";
//
//	// 使用 errorCode 检查创建文件是否成功
//	std::error_code errorCode;
//	raw_fd_ostream dumpedFile(filePath, errorCode);
//	if (errorCode) {
//		// 输出错误信息
//		flexprint(N->Fe, N->Fm, N->Fpinfo, "Error opening file %s: %s\n", filePath.str().c_str(), errorCode.message().c_str());
//		llvm::errs() << "DumpIR: Failed to open file: " << filePath.str() << " (" << errorCode.message() << ")\n";
//		return;
//	} else {
//		llvm::errs() << "DumpIR: File opened successfully.\n";
//	}
//
//	// 写入 IR 并检查写入过程
//	WriteBitcodeToFile(*Mod, dumpedFile);
//	dumpedFile.flush();
//	if (dumpedFile.has_error()) {
//		llvm::errs() << "DumpIR: Error during WriteBitcodeToFile: " << dumpedFile.error().message() << "\n";
//		flexprint(N->Fe, N->Fm, N->Fpinfo, "Error during WriteBitcodeToFile: %s\n", dumpedFile.error().message().c_str());
//	} else {
//		llvm::errs() << "DumpIR: WriteBitcodeToFile completed successfully.\n";
//	}
//
//	dumpedFile.close();
//	llvm::errs() << "DumpIR: File closed.\n";
// }

void
mergeBoundInfo(BoundInfo * dst, const BoundInfo * src)
{
	dst->virtualRegisterRange.insert(src->virtualRegisterRange.begin(),
					 src->virtualRegisterRange.end());
	return;
}

void
collectCalleeInfo(std::vector<std::string> &	       calleeNames,
		  std::map<std::string, BoundInfo *> & funcBoundInfo,
		  const BoundInfo *		       boundInfo)
{
	for (auto & calleeInfo : boundInfo->calleeBound)
	{
		calleeNames.emplace_back(calleeInfo.first);
		funcBoundInfo.emplace(calleeInfo.first, calleeInfo.second);
		collectCalleeInfo(calleeNames, funcBoundInfo, calleeInfo.second);
	}
	return;
}

class FunctionNode {
	mutable AssertingVH<Function>	 F;
	FunctionComparator::FunctionHash Hash;

	public:
	// Note the hash is recalculated potentially multiple times, but it is cheap.
	FunctionNode(Function * F)
	    : F(F), Hash(FunctionComparator::functionHash(*F)) {}

	Function *
	getFunc() const
	{
		return F;
	}

	FunctionComparator::FunctionHash
	getHash() const
	{
		return Hash;
	}
};

GlobalNumberState GlobalNumbers;

class FunctionNodeCmp {
	public:
	bool
	operator()(const FunctionNode & LHS, const FunctionNode & RHS) const
	{
		// Order first by hashes, then full function comparison.
		if (LHS.getHash() != RHS.getHash())
			return LHS.getHash() < RHS.getHash();
		FunctionComparator FCmp(LHS.getFunc(), RHS.getFunc(), &GlobalNumbers);
		return FCmp.compare() == -1;
	}
};

using hashFuncSet = std::set<FunctionNode, FunctionNodeCmp>;

void
cleanFunctionMap(const std::unique_ptr<Module> & Mod, std::map<std::string, CallInst *> & callerMap)
{
	for (auto itFunc = callerMap.begin(); itFunc != callerMap.end();)
	{
		if (nullptr == Mod->getFunction(itFunc->first))
			itFunc = callerMap.erase(itFunc);
		else
			++itFunc;
	}
}

void
overloadFunc(std::unique_ptr<Module> & Mod, std::map<std::string, CallInst *> & callerMap)
{
	/*
	 * compare the functions and remove the redundant one
	 * */
	hashFuncSet baseFuncs;
	auto	    baseFuncNum = baseFuncs.size();
	for (auto itFunc = Mod->getFunctionList().rbegin(); itFunc != Mod->getFunctionList().rend(); itFunc++)
	{
		if (!itFunc->hasName() || itFunc->getName().empty())
			continue;
		if (itFunc->getName().startswith("llvm.dbg.value") ||
		    itFunc->getName().startswith("llvm.dbg.declare"))
			continue;
		if (itFunc->isDeclaration())
			continue;
		baseFuncs.emplace(FunctionNode(&(*itFunc)));
		/*
		 * find the function with the same implementation and change the callInst
		 * */
		if (baseFuncNum == baseFuncs.size())
		{
			auto callerIt = callerMap.find(itFunc->getName().str());
			assert(callerIt != callerMap.end());
			auto		  currentCallerInst = callerIt->second;
			auto		  currentFuncNode   = FunctionNode(&(*itFunc));
			GlobalNumberState cmpGlobalNumbers;
			auto		  sameImplIt = std::find_if(baseFuncs.begin(), baseFuncs.end(),
								    [currentFuncNode, &cmpGlobalNumbers](const FunctionNode & func) {
								    FunctionComparator FCmp(func.getFunc(), currentFuncNode.getFunc(), &cmpGlobalNumbers);
								    return func.getHash() == currentFuncNode.getHash() && FCmp.compare() == 0;
							    });
			assert(sameImplIt != baseFuncs.end());
			currentCallerInst->setCalledFunction(sameImplIt->getFunc());
		}
		else
			baseFuncNum = baseFuncs.size();
	}

	legacy::PassManager passManager;
	passManager.add(createGlobalDCEPass());
	passManager.run(*Mod);
}

void
irPassLLVMIROptimizeByRange(State * N, bool enableQuantization, bool enableOverload, bool enableBuiltinAssume)
{
	llvm::errs() << "Entering irPassLLVMIROptimizeByRange\n";
	if (N->llvmIR == nullptr)
	{
		flexprint(N->Fe, N->Fm, N->Fperr, "Please specify the LLVM IR input file\n");
		llvm::errs() << "Error: llvmIR is nullptr\n";
		fatal(N, Esanity);
	}

	SMDiagnostic		Err;
	LLVMContext		Context;
	std::unique_ptr<Module> Mod(parseIRFile(N->llvmIR, Err, Context));
	if (!Mod)
	{
		flexprint(N->Fe, N->Fm, N->Fperr, "Error: Couldn't parse IR file.");
		llvm::errs() << "Error: Couldn't parse IR file: " << N->llvmIR << "\n";
		fatal(N, Esanity);
	}
	llvm::errs() << "Module successfully parsed: " << N->llvmIR << "\n";
	auto				   globalBoundInfo = new BoundInfo();
	std::map<std::string, BoundInfo *> funcBoundInfo;

	/*
	 * get sensor info, we only concern the id and range here
	 * */
	std::map<std::string, std::pair<double, double>> typeRange;
	if (N->sensorList != NULL)
	{
		for (Modality * currentModality = N->sensorList->modalityList; currentModality != NULL; currentModality = currentModality->next)
		{
			flexprint(N->Fe, N->Fm, N->Fpinfo, "\tModality: %s\n", currentModality->identifier);
			flexprint(N->Fe, N->Fm, N->Fpinfo, "\t\trangeLowerBound: %f\n", currentModality->rangeLowerBound);
			flexprint(N->Fe, N->Fm, N->Fpinfo, "\t\trangeUpperBound: %f\n", currentModality->rangeUpperBound);
			typeRange.emplace(currentModality->identifier, std::make_pair(currentModality->rangeLowerBound, currentModality->rangeUpperBound));
			double resolution = computeResolution(currentModality);
			flexprint(N->Fe, N->Fm, N->Fpinfo, "\t\tresolution: %.10f\n", resolution);
		}
	}

	/*
	 *TODO  get sensor info, we only concern the id and precisionBits here
	 * */
	std::map<std::string, int> typePrecisionBits;
	if (N->sensorList != NULL)
	{
		for (Modality * currentModality = N->sensorList->modalityList; currentModality != NULL; currentModality = currentModality->next)
		{
			flexprint(N->Fe, N->Fm, N->Fpinfo, "\tModality: %s\n", currentModality->identifier);
			flexprint(N->Fe, N->Fm, N->Fpinfo, "\t\tprecisionBits: %d\n", currentModality->precisionBits);
			typePrecisionBits.emplace(currentModality->identifier, currentModality->precisionBits);

			// resolution
		}
	}

	//	int maxPrecisionBits = 0;
	//	int maxPrecisionBits = MAX_PRECISION_BITS;
	//	for (auto & typePrecisionBit : typePrecisionBits)
	//	{
	//		if (typePrecisionBit.second > maxPrecisionBits)
	//		{
	//			maxPrecisionBits = typePrecisionBit.second;
	//		}
	//	}
	//


	//	int maxPrecisionBits = 0;
	int maxPrecisionBits = MAX_PRECISION_BITS;

	/**
	 * Precision Analysis
	 */
	double minResolution = 0.0;
	bool   isFirstSensor = true;

	for (Modality * currentModality = N->sensorList->modalityList; currentModality != NULL; currentModality = currentModality->next)
	{
		// Calculate resolution
		double resolution = (currentModality->rangeUpperBound - currentModality->rangeLowerBound) /
				    (1 << currentModality->precisionBits);

		// Store and print
		currentModality->resolution = resolution;

		// Initialize or compare for minimum
		if (isFirstSensor)
		{
			minResolution = resolution;
			isFirstSensor = false;
		}
		else if (resolution < minResolution)
		{
			minResolution = resolution;
		}
	}

	double fracQ_exact = -log2(minResolution) - 1.0;
	int    fracQ	   = (int)ceil(fracQ_exact);

	flexprint(N->Fe, N->Fm, N->Fpinfo, "Minimum resolution across all sensors: %f\n", minResolution);
	flexprint(N->Fe, N->Fm, N->Fpinfo, "Required FRAC_Q: ceil(-log2(minResolution) - 1) = ceil(%f) = %d\n",
		  fracQ_exact, fracQ);

	/**
	 * Bitwidth Setting
	 */

//	int maxPrecisionBits = 0;
//	//	int maxPrecisionBits = MAX_PRECISION_BITS;
//	for (auto & typePrecisionBit : typePrecisionBits)
//	{
//		if (typePrecisionBit.second > maxPrecisionBits)
//		{
//			maxPrecisionBits = typePrecisionBit.second;
//		}
//	}
//
//	int BIT_WIDTH = (maxPrecisionBits > 16) ? 32 : 16;
//	flexprint(N->Fe, N->Fm, N->Fpinfo,
//		  "Max precisionBits among sensors: %d → BIT_WIDTH = %d\n",
//		  maxPrecisionBits, BIT_WIDTH);
//
//	flexprint(N->Fe, N->Fm, N->Fpinfo, "bitwidth: %d => using %s\n",
//		  BIT_WIDTH,
//		  (BIT_WIDTH == 32 ? "int32_t (i32 fix)" : "int16_t (i16 fix)"));




	/**
	 * Config
	 */
	//	int BIT_WIDTH = 32;
	//	maxPrecisionBits = 16;

	/*
	 * get const global variables
	 * */
	std::map<llvm::Value *, std::vector<std::pair<double, double>>> virtualRegisterVectorRange;
	for (auto & globalVar : Mod->getGlobalList())
	{
		if (!globalVar.hasInitializer())
		{
			llvm::errs() << "Global variable " << globalVar.getName() << " has no initializer\n";
			continue;
		}
		llvm::errs() << "Processing global variable: " << globalVar.getName() << "\n";
		auto constValue = globalVar.getInitializer();
		if (ConstantFP * constFp = llvm::dyn_cast<llvm::ConstantFP>(constValue))
		{
			if (constValue->getType()->isFloatTy())
			{
				float constValue = constFp->getValueAPF().convertToFloat();
				globalBoundInfo->virtualRegisterRange.emplace(&globalVar, std::make_pair(constValue, constValue));
			}
			else if (constValue->getType()->isDoubleTy())
			{
				double constValue = constFp->getValueAPF().convertToDouble();
				globalBoundInfo->virtualRegisterRange.emplace(&globalVar, std::make_pair(constValue, constValue));
			}
		}
		else if (ConstantInt * constInt = llvm::dyn_cast<llvm::ConstantInt>(constValue))
		{
			auto constValue = constInt->getSExtValue();
			globalBoundInfo->virtualRegisterRange.emplace(&globalVar, std::make_pair(static_cast<double>(constValue),
												 static_cast<double>(constValue)));
		}
		else if (ConstantDataArray * constArr = llvm::dyn_cast<llvm::ConstantDataArray>(constValue))
		{
			auto arrType = constArr->getElementType();
			if (arrType->isDoubleTy())
			{
				for (size_t idx = 0; idx < constArr->getNumElements(); idx++)
				{
					double dbValue = constArr->getElementAsDouble(idx);
					virtualRegisterVectorRange[&globalVar].emplace_back(std::make_pair(dbValue, dbValue));
				}
			}
			else if (arrType->isFloatTy())
			{
				for (size_t idx = 0; idx < constArr->getNumElements(); idx++)
				{
					double ftValue = constArr->getElementAsFloat(idx);
					virtualRegisterVectorRange[&globalVar].emplace_back(std::make_pair(ftValue, ftValue));
				}
			}
			else if (arrType->isIntegerTy())
			{
				for (size_t idx = 0; idx < constArr->getNumElements(); idx++)
				{
					uint64_t intValue = constArr->getElementAsInteger(idx);
					virtualRegisterVectorRange[&globalVar].emplace_back(std::make_pair(intValue, intValue));
				}
			}
			else if (arrType->isPointerTy())
			{
				flexprint(N->Fe, N->Fm, N->Fperr, "\t\tTODO: Didn't support const pointer!\n");
			}
			else
			{
				flexprint(N->Fe, N->Fm, N->Fperr, "\t\tUnknown constant type!\n");
			}
		}
		else
		{
			flexprint(N->Fe, N->Fm, N->Fperr, "\t\tUnknown type!\n");
		}
	}

	detectFloatingPointOps(*Mod);
	checkFPUAvailability(*Mod);

	/*
	 * analyze the range of all local variables in each function
	 * */
	flexprint(N->Fe, N->Fm, N->Fpinfo, "infer bound\n");
	std::map<std::string, CallInst *> callerMap;
	callerMap.clear();
	funcBoundInfo.clear();
	bool useOverLoad = false;
	for (auto & mi : *Mod)
	{
		auto boundInfo = new BoundInfo();
		mergeBoundInfo(boundInfo, globalBoundInfo);
		rangeAnalysis(N, mi, boundInfo, callerMap, typeRange, virtualRegisterVectorRange, useOverLoad);
		funcBoundInfo.emplace(mi.getName().str(), boundInfo);
		std::vector<std::string> calleeNames;
		collectCalleeInfo(calleeNames, funcBoundInfo, boundInfo);
	}

	/**
	 * Check for potential overflows
	 */
	flexprint(N->Fe, N->Fm, N->Fpinfo, "checking for potential overflows\n");
	for (auto & funcPair : funcBoundInfo)
	{
		checkOverflow(N, funcPair.second, maxPrecisionBits);
	}

	//
	//		flexprint(N->Fe, N->Fm, N->Fpinfo, "shrink data type by range\n");
	//		for (auto & mi : *Mod)
	//		{
	//			auto boundInfoIt = funcBoundInfo.find(mi.getName().str());
	//			if (boundInfoIt != funcBoundInfo.end())
	//			{
	//				shrinkType(N, boundInfoIt->second, mi);
	//			}
	//			//            else
	//			//            {
	//			//	            assert(false);
	//			//	        }
	//		}

	if (enableQuantization)
	{
		flexprint(N->Fe, N->Fm, N->Fpinfo, "auto quantization\n");
		llvm::errs() << "Auto quantization enabled\n";
		std::vector<llvm::Function *> functionsToInsert;
		for (auto & mi : *Mod)
		{
			llvm::errs() << "Quantizing function: " << mi.getName() << "\n";

			irPassLLVMIRAutoQuantization(N, mi, functionsToInsert, maxPrecisionBits);
		}
		for (auto mi : functionsToInsert)
		{
			Mod->getFunctionList().remove(mi);
			Mod->getFunctionList().push_front(mi);
		}
	}

	//		/*
	//		 * analyze the range of all local variables in each function
	//		 * */
	//		flexprint(N->Fe, N->Fm, N->Fpinfo, "infer bound\n");
	//		std::map<std::string, CallInst *> callerMap;
	//		callerMap.clear();
	//		funcBoundInfo.clear();
	//		bool useOverLoad = false;
	//		for (auto & mi : *Mod)
	//		{
	//			auto boundInfo = new BoundInfo();
	//			mergeBoundInfo(boundInfo, globalBoundInfo);
	//			rangeAnalysis(N, mi, boundInfo, callerMap, typeRange, virtualRegisterVectorRange, useOverLoad);
	//			funcBoundInfo.emplace(mi.getName().str(), boundInfo);
	//			std::vector<std::string> calleeNames;
	//			collectCalleeInfo(calleeNames, funcBoundInfo, boundInfo);
	//		}
	//
	//		/**
	//		 * Check for potential overflows
	//		 */
	//		flexprint(N->Fe, N->Fm, N->Fpinfo, "checking for potential overflows\n");
	//		for (auto & funcPair : funcBoundInfo) {
	//			checkOverflow(N, funcPair.second, maxPrecisionBits);
	//		}
	//
	//		flexprint(N->Fe, N->Fm, N->Fpinfo, "memory alignment\n");
	//		for (auto & mi : *Mod)
	//		{
	//			auto boundInfoIt = funcBoundInfo.find(mi.getName().str());
	//			if (boundInfoIt != funcBoundInfo.end())
	//			{
	//				memoryAlignment(N, boundInfoIt->second, mi);
	//			}
	//			//        else
	//			//        {
	//			//            assert(false);
	//			//        }
	//		}
	//
	/*
	 * remove the functions that are optimized by passes.
	 * */
	//		if (useOverLoad)
	//			cleanFunctionMap(Mod, callerMap);
	//
	//		if (useOverLoad)
	//			overloadFunc(Mod, callerMap);
	//
	//		callerMap.clear();
	//		funcBoundInfo.clear();
	//		useOverLoad = true;
	//		for (auto & mi : *Mod)
	//		{
	//			auto boundInfo = new BoundInfo();
	//			mergeBoundInfo(boundInfo, globalBoundInfo);
	//			rangeAnalysis(N, mi, boundInfo, callerMap, typeRange, virtualRegisterVectorRange, useOverLoad);
	//			funcBoundInfo.emplace(mi.getName().str(), boundInfo);
	//			std::vector<std::string> calleeNames;
	//			collectCalleeInfo(calleeNames, funcBoundInfo, boundInfo);
	//		}

	//	/*
	//	 * simplify the condition of each branch
	//	 * */
	//		flexprint(N->Fe, N->Fm, N->Fpinfo, "simplify control flow by range\n");
	//		for (auto & mi : *Mod)
	//		{
	//			auto boundInfoIt = funcBoundInfo.find(mi.getName().str());
	//			if (boundInfoIt != funcBoundInfo.end())
	//			{
	//				simplifyControlFlow(N, boundInfoIt->second, mi);
	//			}
	//			//		else
	//			//		{
	//			//			assert(false);
	//			//		}
	//		}
	//
	//		legacy::PassManager passManager;
	//		passManager.add(createCFGSimplificationPass());
	//		passManager.add(createInstSimplifyLegacyPass());
	//		passManager.add(createGlobalDCEPass());
	//		passManager.run(*Mod);

	//

	//
	//	/*
	//	 * remove the functions that are optimized by passes.
	//	 * */
	//	if (useOverLoad)
	//		cleanFunctionMap(Mod, callerMap);
	//
	//	if (useOverLoad)
	//		overloadFunc(Mod, callerMap);
	//
	//	flexprint(N->Fe, N->Fm, N->Fpinfo, "infer bound\n");
	//	callerMap.clear();
	//	funcBoundInfo.clear();
	//	useOverLoad = false;
	//	for (auto & mi : *Mod)
	//	{
	//		auto boundInfo = new BoundInfo();
	//		mergeBoundInfo(boundInfo, globalBoundInfo);
	//		rangeAnalysis(N, mi, boundInfo, callerMap, typeRange, virtualRegisterVectorRange, useOverLoad);
	//		funcBoundInfo.emplace(mi.getName().str(), boundInfo);
	//		std::vector<std::string> calleeNames;
	//		collectCalleeInfo(calleeNames, funcBoundInfo, boundInfo);
	//	}
	//
	//	flexprint(N->Fe, N->Fm, N->Fpinfo, "constant substitution\n");
	//	for (auto & mi : *Mod)
	//	{
	//		auto boundInfoIt = funcBoundInfo.find(mi.getName().str());
	//		if (boundInfoIt != funcBoundInfo.end())
	//		{
	//			constantSubstitution(N, boundInfoIt->second, mi);
	//		}
	//		//		else
	//		//		{
	//		//			assert(false);
	//		//		}
	//	}
	//
	//		/*
	//		 * remove the functions that are optimized by passes.
	//		 * */
	//		if (useOverLoad)
	//			cleanFunctionMap(Mod, callerMap);
	//./
	//		if (useOverLoad)
	//			overloadFunc(Mod, callerMap);

	// Finally, erase old functions
	eraseOldFunctions();

	// eraseOldGlobals();
	eraseUnusedConstant(*Mod);

	// Perform text replacement to remove "_quantized" suffixes
	// removeQuantizedSuffixInModule(*Mod);

	// eraseOldInstructions();

	processWhitelistedFunctions(*Mod, whitelist, maxPrecisionBits);

	const char * homeDir = getenv("HOME");
	if (!homeDir)
	{
		llvm::errs() << "Error: HOME environment variable not set.\n";
		return;
	}
	// Save the optimized IR to a file
	// std::string fileName = std::string(homeDir) + "/CoSense/applications/newton/llvm-ir/MadgwickAHRS_output.ll";
	// saveModuleIR(*Mod, fileName);
	// Save the optimized IR to a file
	saveModuleIR(*Mod, "/home/xyf/CoSense/applications/newton/llvm-ir/MadgwickAHRS_output.ll");
	saveModuleIR(*Mod, "/home/xyf/CoSense/applications/newton/llvm-ir/MahonyAHRS_output.ll");
	saveModuleIR(*Mod, "/home/xyf/CoSense/applications/newton/llvm-ir/sensfusion6_output.ll");
	// saveModuleIR(*Mod, "/home/xyf/CoSense/applications/newton/llvm-ir/floating_point_operations_output.ll");

	/*
	 * Dump BC file to a file.
	 * */
	dumpIR(N, "output", Mod);
	llvm::errs() << "Exiting irPassLLVMIROptimizeByRange\n";
}