
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/IR/LegacyPassManager.h"

// 包含你的 RangeAnalysisPass 头文件
#include "myRangeAnalysis.h"

using namespace llvm;

int main(int argc, char **argv) {
    if (argc < 2) {
        errs() << "Usage: " << argv[0] << " <IR file>\n";
        return 1;
    }

    // 解析输入文件
    LLVMContext Context;
    SMDiagnostic Err;
    std::unique_ptr<Module> M = parseIRFile(argv[1], Err, Context);

    if (!M) {
        Err.print(argv[0], errs());
        return 1;
    }

    // 创建 PassManager
    legacy::PassManager PM;

    // 添加自定义 Pass
    PM.add(new RangeAnalysisPass());

    // 运行 Pass
    PM.run(*M);

    // 输出结果
    M->print(outs(), nullptr);

    return 0;
}
