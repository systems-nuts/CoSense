#!/bin/bash

# Get the current user's home directory
USER_HOME=$HOME

#opt $USER_HOME/CoSense/applications/newton/llvm-ir/MadgwickAHRS_opt.ll  -O3 -Os -S -o $USER_HOME/CoSense/applications/newton/llvm-ir/out.ll
#opt $USER_HOME/CoSense/applications/newton/llvm-ir/MadgwickAHRS_opt.ll   -S -o $USER_HOME/CoSense/applications/newton/llvm-ir/out.ll

# Step 5: Compile the optimized LLVM IR file to bitcode
echo "Step 5: Compile the optimized LLVM IR file to bitcode"
llvm-as $USER_HOME/CoSense/applications/newton/llvm-ir/out.ll -o $USER_HOME/CoSense/applications/newton/llvm-ir/out.bc

# Step 6: Compile the bitcode file to assembly
echo "Step 6: Compile the bitcode file to assembly"
llc $USER_HOME/CoSense/applications/newton/llvm-ir/out.bc -o $USER_HOME/CoSense/applications/newton/llvm-ir/out.s

# Step 7: Compile the assembly file to object file
echo "Step 7: Compile the assembly file to object file"
clang -c $USER_HOME/CoSense/applications/newton/llvm-ir/out.s -o $USER_HOME/CoSense/applications/newton/llvm-ir/out.o

# Step 8: Package the object file into a static library
echo "Step 8: Package the object file into a static library"
ar -rc $USER_HOME/CoSense/applications/newton/llvm-ir/libout.a $USER_HOME/CoSense/applications/newton/llvm-ir/out.o

# Step 9: Compile the test file and link with the static library
echo "Step 9: Compile the test file and link with the static library"
clang $USER_HOME/CoSense/applications/newton/llvm-ir/c-files/test_MadgwickAHRSfix.c  -no-pie -L$USER_HOME/CoSense/applications/newton/llvm-ir -lout -O3 -Os -g -fno-builtin -o $USER_HOME/CoSense/applications/newton/llvm-ir/main_out -lm

# Step 10: Run the test executable
echo "Step 10: Run the test executable"
$HOME/CoSense/applications/newton/llvm-ir/main_out