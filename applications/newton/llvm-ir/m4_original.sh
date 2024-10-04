#!/bin/bash

# 定义目标 CPU 变量
TARGET_CPU="cortex-m4"

# 定义交叉编译前缀变量
CROSS_COMPILE_PREFIX="arm-none-eabi-"

# 正确使用变量来设置目标 CPU 的 clang 命令
CLANG_CMD="$CROSS_COMPILE_PREFIX clang"

# 确保 HOME 变量被正确定义
if [ -z "$HOME" ]; then
  HOME=$HOME
fi

# Step 1: Generate LLVM IR file
echo "Step 1: Generate LLVM IR file"

clang -g0 -O0 -Xclang -disable-O0-optnone -target armv7e-m -mcpu=cortex-m4


 -mthumb -S -emit-llvm -Wall -Wextra -o $HOME/CoSense/applications/newton/llvm-ir/MadgwickAHRS.ll $HOME/CoSense/applications/newton/llvm-ir/c-files/MadgwickAHRS.c


echo "Step 2: Optimize the generated LLVM IR file"

opt  $HOME/CoSense/applications/newton/llvm-ir/MadgwickAHRS.ll -S -o $HOME/CoSense/applications/newton/llvm-ir/out.ll

echo "Step 3: Compile the optimized LLVM IR file to bitcode"

llvm-as $HOME/CoSense/applications/newton/llvm-ir/out.ll -o $HOME/CoSense/applications/newton/llvm-ir/out.bc

echo "Step 4: Compile the bitcode file to assembly"

llc -march=arm -mcpu=cortex-m4 -mattr=+vfp4,+soft-float $HOME/CoSense/applications/newton/llvm-ir/out.bc -o $HOME/CoSense/applications/newton/llvm-ir/out.s

echo "Step 5: Compile the assembly file to object file"


clang -target armv7e-m -mcpu=cortex-m4 -mfloat-abi=soft -mthumb -c $HOME/CoSense/applications/newton/llvm-ir/out.s -o $HOME/CoSense/applications/newton/llvm-ir/out.o


echo "Step 6: Package the object file into a static library"

ar -rc $HOME/CoSense/applications/newton/llvm-ir/libout.a $HOME/CoSense/applications/newton/llvm-ir/out.o

echo "Step 7: Compile the test file and link with the static library"


arm-none-eabi-gcc syscalls.c \
  $HOME/CoSense/applications/newton/llvm-ir/c-files/test_m4.c \
  system_stm32f3xx.c \
  -DSTM32F303xC -DFP_DATA_TYPE -mcpu=cortex-m4 -mfloat-abi=soft -mthumb \
  -L$HOME/CoSense/applications/newton/llvm-ir -lout -lm -O3 -Os -g -fno-builtin \
  -o $HOME/CoSense/applications/newton/llvm-ir/main_out.elf





# Step 8: Generate HEX or BIN file for flashing
echo "Step 8: Generate HEX and BIN files"
arm-none-eabi-objcopy -O ihex $HOME/CoSense/applications/newton/llvm-ir/main_out.elf $HOME/CoSense/applications/newton/llvm-ir/main_out.hex
arm-none-eabi-objcopy -O binary $HOME/CoSense/applications/newton/llvm-ir/main_out.elf $HOME/CoSense/applications/newton/llvm-ir/main_out.bin
#
#echo "Step 8: Run the test executable"
#
#$HOME/CoSense/applications/newton/llvm-ir/main_out
