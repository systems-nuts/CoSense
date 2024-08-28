import re


def replace_invSqrt_call(filepath):
    # 读取文件内容
    with open(filepath, 'r', encoding='utf-8') as file:
        content = file.read()

    # 替换 invSqrt 调用为 fixrsqrt
    updated_content = re.sub(r'call i32 @invSqrt', 'call i32 @fixrsqrt', content)

    # 将更新后的内容写回文件
    with open(filepath, 'w', encoding='utf-8') as file:
        file.write(updated_content)




# 文件路径
filepath = 'MadgwickAHRS_opt.ll'



# 替换 invSqrt 函数的调用
replace_invSqrt_call(filepath)

