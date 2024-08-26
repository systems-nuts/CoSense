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
new_content = """define dso_local i32 @invSqrt(i32 %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca float, align 4
  %5 = alloca i64, align 8
  %6 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  %7 = load i32, i32* %2, align 4
  %8 = call i32 @fixmul(i32 512, i32 %7)
  store i32 %8, i32* %3, align 4
  %9 = load i32, i32* %2, align 4
  %10 = sitofp i32 %9 to float
  %11 = fdiv float %10, 1.024000e+03
  store float %11, float* %4, align 4
  %12 = bitcast float* %4 to i64*
  %13 = load i64, i64* %12, align 4
  store i64 %13, i64* %5, align 8
  %14 = load i64, i64* %5, align 8
  %15 = ashr i64 %14, 1
  %16 = sub nsw i64 1597463007, %15
  store i64 %16, i64* %5, align 8
  %17 = bitcast i64* %5 to float*
  %18 = load float, float* %17, align 8
  store float %18, float* %4, align 4
  %19 = load float, float* %4, align 4
  %20 = fmul float %19, 1.024000e+03
  %21 = fptosi float %20 to i32
  store i32 %21, i32* %6, align 4
  %22 = load i32, i32* %6, align 4
  %23 = load i32, i32* %3, align 4
  %24 = load i32, i32* %6, align 4
  %25 = call i32 @fixmul(i32 %23, i32 %24)
  %26 = load i32, i32* %6, align 4
  %27 = call i32 @fixmul(i32 %25, i32 %26)
  %28 = sub nsw i32 1536, %27
  %29 = call i32 @fixmul(i32 %22, i32 %28)
  store i32 %29, i32* %6, align 4
  %30 = load i32, i32* %6, align 4
  ret i32 %30
}"""

# 文件路径
filepath = 'MadgwickAHRS_opt.ll'

replace_function_content(filepath, 'define dso_local i32 @invSqrt\(i32 %0\) #1', new_content)


