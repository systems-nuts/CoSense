import re


def replace_function_content(filepath, old_content, new_content):
    # 读取文件内容
    with open(filepath, 'r', encoding='utf-8') as file:
        content = file.read()

    # 替换函数定义
    #updated_content = re.sub(re.escape(old_content), new_content, content, flags=re.DOTALL)

    # 定义要匹配的模式
    pattern = re.compile(r'(define dso_local i32 @invSqrt\(i32 %0\) #1 .*?\{.*?^\})', re.DOTALL | re.MULTILINE)

    # 替换函数定义
    updated_content = pattern.sub(new_content, content)

    # 将更新后的内容写回文件
    with open(filepath, 'w', encoding='utf-8') as file:
        file.write(updated_content)




# 定义新函数内容
new_content = """define dso_local i32 @invSqrt(i32 %0) #0 !dbg !61 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca float, align 4
  %5 = alloca i64, align 8
  %6 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  call void @llvm.dbg.declare(metadata i32* %2, metadata !62, metadata !DIExpression()), !dbg !63
  call void @llvm.dbg.declare(metadata i32* %3, metadata !64, metadata !DIExpression()), !dbg !65
  %7 = load i32, i32* %2, align 4, !dbg !66
  %8 = call i32 @fixmul(i32 512, i32 %7), !dbg !67
  store i32 %8, i32* %3, align 4, !dbg !65
  call void @llvm.dbg.declare(metadata float* %4, metadata !68, metadata !DIExpression()), !dbg !69
  %9 = load i32, i32* %2, align 4, !dbg !70
  %10 = sitofp i32 %9 to float, !dbg !71
  %11 = fdiv float %10, 1.024000e+03, !dbg !72
  store float %11, float* %4, align 4, !dbg !69
  call void @llvm.dbg.declare(metadata i64* %5, metadata !73, metadata !DIExpression()), !dbg !74
  %12 = bitcast float* %4 to i64*, !dbg !75
  %13 = load i64, i64* %12, align 4, !dbg !75
  store i64 %13, i64* %5, align 8, !dbg !74
  %14 = load i64, i64* %5, align 8, !dbg !76
  %15 = ashr i64 %14, 1, !dbg !77
  %16 = sub nsw i64 1597463007, %15, !dbg !78
  store i64 %16, i64* %5, align 8, !dbg !79
  %17 = bitcast i64* %5 to float*, !dbg !80
  %18 = load float, float* %17, align 8, !dbg !80
  store float %18, float* %4, align 4, !dbg !81
  call void @llvm.dbg.declare(metadata i32* %6, metadata !82, metadata !DIExpression()), !dbg !83
  %19 = load float, float* %4, align 4, !dbg !84
  %20 = fmul float %19, 1.024000e+03, !dbg !85
  %21 = fptosi float %20 to i32, !dbg !84
  store i32 %21, i32* %6, align 4, !dbg !83
  %22 = load i32, i32* %6, align 4, !dbg !86
  %23 = load i32, i32* %3, align 4, !dbg !87
  %24 = load i32, i32* %6, align 4, !dbg !88
  %25 = call i32 @fixmul(i32 %23, i32 %24), !dbg !89
  %26 = load i32, i32* %6, align 4, !dbg !90
  %27 = call i32 @fixmul(i32 %25, i32 %26), !dbg !91
  %28 = sub nsw i32 1536, %27, !dbg !92
  %29 = call i32 @fixmul(i32 %22, i32 %28), !dbg !93
  store i32 %29, i32* %6, align 4, !dbg !94
  %30 = load i32, i32* %6, align 4, !dbg !95
  ret i32 %30, !dbg !96
}"""

# 文件路径
filepath = 'MadgwickAHRS_opt.ll'

replace_function_content(filepath, 'define dso_local i32 @invSqrt\(i32 %0\) #1', new_content)


