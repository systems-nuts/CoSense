import re
import os

def replace_text_in_file(filepath, patterns_replacements):
    # 检查文件是否存在
    if not os.path.isfile(filepath):
        print(f"Error: File '{filepath}' does not exist.")
        return

    # 读取文件内容
    with open(filepath, 'r', encoding='utf-8') as file:
        content = file.read()

    # 对所有模式和替换进行操作
    for pattern, replacement in patterns_replacements:
        updated_content = re.sub(pattern, replacement, content)

    # 将更新后的内容写回文件
    with open(filepath, 'w', encoding='utf-8') as file:
        file.write(updated_content)

# 定义文件路径和替换规则
filepath = 'MadgwickAHRS_opt.ll'
patterns_replacements = [
    (r'declare dso_local i32 @printf\(i8\*\)', 'declare dso_local i32 @printf(i8*, i32)'),
    (r'float\*\*', 'i32**'),
    (r'float\*', 'i32*'),
    (r'call i32 @invSqrt', 'call i32 @fixrsqrt')
]

# 执行替换
replace_text_in_file(filepath, patterns_replacements)

print("Replacement complete.")