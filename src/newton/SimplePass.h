#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/PassManager.h"

using namespace llvm;

namespace {
class SimplePass : public PassInfoMixin<SimplePass> {
	public:
	PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
		errs() << "SimplePass: " << F.getName() << "\n";
		return PreservedAnalyses::all();
	}
};

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
	return {
	    LLVM_PLUGIN_API_VERSION, "SimplePass", LLVM_VERSION_STRING,
	    [](PassBuilder &PB) {
		    PB.registerPipelineParsingCallback(
			[](StringRef Name, FunctionPassManager &FPM,
			   ArrayRef<PassBuilder::PipelineElement>) {
				if (Name == "simple-pass") {
					FPM.addPass(SimplePass());
					return true;
				}
				return false;
			});
	    }
	};
}
}
