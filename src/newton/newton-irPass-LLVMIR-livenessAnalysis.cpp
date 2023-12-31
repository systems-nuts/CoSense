/*
	Authored 2021. Nikos Mavrogeorgis.

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

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdint.h>
#include <set>
#include <algorithm>
#include <map>

#include "llvm/IR/Metadata.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/DebugInfoMetadata.h"

using namespace llvm;


extern "C"
{

#include "flextypes.h"
#include "flexerror.h"
#include "flex.h"
#include "common-errors.h"
#include "version.h"
#include "newton-timeStamps.h"
#include "common-timeStamps.h"
#include "common-data-structures.h"
#include "noisy-parser.h"
#include "newton-parser.h"
#include "noisy-lexer.h"
#include "newton-lexer.h"
#include "common-irPass-helpers.h"
#include "common-lexers-helpers.h"
#include "common-irHelpers.h"
#include "common-symbolTable.h"
#include "newton-types.h"
#include "newton-symbolTable.h"
#include "newton-irPass-cBackend.h"
#include "newton-irPass-autoDiff.h"
#include "newton-irPass-estimatorSynthesisBackend.h"
#include "newton-irPass-invariantSignalAnnotation.h"


typedef struct LivenessState {
	std::map<BasicBlock *, std::set<Value *>>	upwardExposedVariables;
	std::map<BasicBlock *, std::set<Value *>>	killedVariables;
	std::map<BasicBlock *, std::set<Value *>>	liveOutVariables;
} LivenessState;

void
printBasicBlockSets(const std::map<BasicBlock *, std::set<Value *>>&  basicBlockSets)
{
	for (auto const& basicBlockSet : basicBlockSets)
	{
		outs() << "Basic Block: \n";
		outs() << *(basicBlockSet.first->getFirstNonPHI()) << "\n";
		for (auto const& var : basicBlockSet.second)
		{
			outs() << "		" << *var <<  "\n";
		}
		outs() << "=================================\n";
	}
}


void
initBasicBlock(LivenessState *  livenessState, BasicBlock &  llvmIrBasicBlock)
{
	livenessState->upwardExposedVariables[&llvmIrBasicBlock].empty();
	livenessState->killedVariables[&llvmIrBasicBlock].empty();

	for (Instruction &  llvmIrInstruction : llvmIrBasicBlock)
	{
		switch (llvmIrInstruction.getOpcode())
		{
			case Instruction::Add:
			case Instruction::FAdd:
			case Instruction::Sub:
			case Instruction::FSub:
			case Instruction::Mul:
			case Instruction::FMul:
			case Instruction::SDiv:
			case Instruction::FDiv:
			case Instruction::UDiv:
			case Instruction::URem:
			case Instruction::SRem:
			case Instruction::FRem:
			case Instruction::And:
			case Instruction::Or:
			case Instruction::Xor:
			case Instruction::ICmp:
			case Instruction::FCmp:
			{
				auto leftOperand = llvmIrInstruction.getOperand(0);
				auto rightOperand = llvmIrInstruction.getOperand(1);

				if (livenessState->killedVariables[&llvmIrBasicBlock].count(leftOperand) == 0 && !isa<llvm::Constant>(leftOperand))
				{
					livenessState->upwardExposedVariables[&llvmIrBasicBlock].insert(leftOperand);
				}

				if (livenessState->killedVariables[&llvmIrBasicBlock].count(rightOperand) == 0 && !isa<llvm::Constant>(rightOperand))
				{
					livenessState->upwardExposedVariables[&llvmIrBasicBlock].insert(rightOperand);
				}

				livenessState->killedVariables[&llvmIrBasicBlock].insert(&llvmIrInstruction);
			}
			case Instruction::SExt:
			case Instruction::ZExt:
			case Instruction::AShr:
			case Instruction::LShr:
			case Instruction::Shl:
			case Instruction::Trunc:
			case Instruction::SIToFP:
			case Instruction::FNeg:
			case Instruction::FPToUI:
			case Instruction::FPToSI:
			case Instruction::UIToFP:
			case Instruction::FPTrunc:
			case Instruction::FPExt:
			case Instruction::PtrToInt:
			case Instruction::IntToPtr:
			case Instruction::BitCast:
			case Instruction::AddrSpaceCast:
			case Instruction::ExtractElement:
			{
				auto operand = llvmIrInstruction.getOperand(0);

				if (livenessState->killedVariables[&llvmIrBasicBlock].count(operand) == 0 && !isa<llvm::Constant>(operand))
				{
					livenessState->upwardExposedVariables[&llvmIrBasicBlock].insert(operand);
				}

				livenessState->killedVariables[&llvmIrBasicBlock].insert(&llvmIrInstruction);
			}
			case Instruction::Call:
			{
				if (auto llvmIrCallInstruction = dyn_cast<CallInst>(&llvmIrInstruction)) {
					Function *calledFunction = llvmIrCallInstruction->getCalledFunction();
                    if (calledFunction == nullptr || !calledFunction->hasName() || calledFunction->getName().empty())
                        continue;
					if (calledFunction->getName().startswith("llvm.dbg.declare")) {
						continue;
					}
					for (auto &argumentIterator: llvmIrCallInstruction->args()) {
						if (livenessState->killedVariables[&llvmIrBasicBlock].count(argumentIterator) == 0 &&
							!isa<llvm::Constant>(*argumentIterator)) {
							livenessState->upwardExposedVariables[&llvmIrBasicBlock].insert(argumentIterator);
						}

						livenessState->killedVariables[&llvmIrBasicBlock].insert(llvmIrCallInstruction);
					}
				}
			}
		}
	}

	livenessState->liveOutVariables[&llvmIrBasicBlock].empty();
}

std::set<Value *>
computeLiveOurVariables(LivenessState *  livenessState, BasicBlock &  llvmIrBasicBlock)
{
	std::set<Value *>	computedLiveOutVariables;

	auto 	terminatorInstruction = llvmIrBasicBlock.getTerminator();
	auto	successorNumber = terminatorInstruction->getNumSuccessors();

	for (unsigned int i = 0; i < successorNumber; i++)
	{
		BasicBlock *	successorBasicBlock = terminatorInstruction->getSuccessor(i);

		std::set<Value *>	successorUpwardExposedVariables = livenessState->upwardExposedVariables[successorBasicBlock];
		std::set<Value *>	successorKilledVariables = livenessState->killedVariables[successorBasicBlock];
		std::set<Value *>	successorLiveOutVariables = livenessState->liveOutVariables[successorBasicBlock];

		std::set<Value *>	successorContributionToLiveOutVariables;
		std::set<Value *>	liveOutVariablesSetDifferenceKilledVariables;

		std::set_difference(successorLiveOutVariables.begin(), successorLiveOutVariables.end(),
							successorKilledVariables.begin(), successorKilledVariables.end(),
							std::inserter(liveOutVariablesSetDifferenceKilledVariables,
										  liveOutVariablesSetDifferenceKilledVariables.end()));

		std::set_union(successorUpwardExposedVariables.begin(), successorUpwardExposedVariables.end(),
					   liveOutVariablesSetDifferenceKilledVariables.begin(), liveOutVariablesSetDifferenceKilledVariables.end(),
					   std::inserter(successorContributionToLiveOutVariables,
									 successorContributionToLiveOutVariables.end()));

		std::set_union(computedLiveOutVariables.begin(), computedLiveOutVariables.end(),
					   successorContributionToLiveOutVariables.begin(), successorContributionToLiveOutVariables.end(),
					   std::inserter(computedLiveOutVariables,
									 computedLiveOutVariables.end()));
	}

	return computedLiveOutVariables;
}

void
livenessAnalysis(State *  N, LivenessState *  livenessState, Function &  llvmIrFunction)
{
	for (BasicBlock &  llvmIrBasicBlock : llvmIrFunction)
	{
		initBasicBlock(livenessState, llvmIrBasicBlock);
	}

	auto	changed = true;
	while (changed)
	{
		changed = false;
		for (BasicBlock &  llvmIrBasicBlock : llvmIrFunction)
		{
			auto 	computedLiveOutVariables = computeLiveOurVariables(livenessState, llvmIrBasicBlock);

			if (computedLiveOutVariables != livenessState->liveOutVariables[&llvmIrBasicBlock])
			{
				changed = true;
				livenessState->liveOutVariables[&llvmIrBasicBlock] = computedLiveOutVariables;
			}
		}
	}
}

void
irPassLLVMIRLivenessAnalysis(State *  N)
{
	if (N->llvmIR == nullptr)
	{
		flexprint(N->Fe, N->Fm, N->Fperr, "Please specify the LLVM IR input file\n");
		fatal(N, Esanity);
	}

	SMDiagnostic 	Err;
	LLVMContext 	Context;
	std::unique_ptr<Module>	Mod(parseIRFile(N->llvmIR, Err, Context));
	if (!Mod)
	{
		flexprint(N->Fe, N->Fm, N->Fperr, "Error: Couldn't parse IR file.");
		fatal(N, Esanity);
	}

	auto	livenessState = new LivenessState();

	for (auto & mi : *Mod)
	{
		livenessAnalysis(N, livenessState, mi);
	}

//    outs() << "kill: killedVariables\n";
//    printBasicBlockSets(livenessState->killedVariables);
//
//    outs() << "gen: liveOutVariables\n";
//	printBasicBlockSets(livenessState->liveOutVariables);
//
//    outs() << "update: upwardExposedVariables\n";
//    printBasicBlockSets(livenessState->upwardExposedVariables);
}

}
