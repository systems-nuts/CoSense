//
// Created by 13862 on 2024/7/19.
//



#include "newton-irPass-LLVMIR-rangeAnalysis.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
extern std::vector<llvm::Function*> functionsToErase;
extern std::vector<llvm::GlobalVariable*> globalsToErase;
void
irPassLLVMIRAutoDequantization(State * N, llvm::Function & llvmIrFunction, std::vector<llvm::Function*>& functionsToInsert);



extern
    void eraseOldFunctions();
extern
    void eraseOldGlobals();
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */