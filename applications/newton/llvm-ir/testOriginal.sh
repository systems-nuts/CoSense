# Step 1: Generate LLVM IR file
clang -g -O0 -Xclang -disable-O0-optnone -S -emit-llvm -Wall -Wextra -o /home/xyf/CoSense/applications/newton/llvm-ir/MadgwickAHRS.ll /home/xyf/CoSense/applications/newton/llvm-ir/c-files/MadgwickAHRS.c


# Step 4: Optimize the generated LLVM IR file
opt /home/xyf/CoSense/applications/newton/llvm-ir/MadgwickAHRS.ll --simplifycfg --instsimplify -O3 -Os -S -o /home/xyf/CoSense/applications/newton/llvm-ir/out.llperformace

# Step 5: Compile the optimized LLVM IR file to bitcode
llvm-as /home/xyf/CoSense/applications/newton/llvm-ir/out.llperformace -o /home/xyf/CoSense/applications/newton/llvm-ir/out.bc

# Step 6: Compile the bitcode file to assembly
llc /home/xyf/CoSense/applications/newton/llvm-ir/out.bc -o /home/xyf/CoSense/applications/newton/llvm-ir/out.s

# Step 7: Compile the assembly file to object file
clang -c /home/xyf/CoSense/applications/newton/llvm-ir/out.s -o /home/xyf/CoSense/applications/newton/llvm-ir/out.o

# Step 8: Package the object file into a static library
ar -rc /home/xyf/CoSense/applications/newton/llvm-ir/libout.a /home/xyf/CoSense/applications/newton/llvm-ir/out.o

# Step 9: Compile the test file and link with the static library
clang /home/xyf/CoSense/applications/newton/llvm-ir/c-files/test_Original.c -no-pie -L/home/xyf/CoSense/applications/newton/llvm-ir -lout -O3 -Os -g -fno-builtin -o /home/xyf/CoSense/applications/newton/llvm-ir/main_out -lm

# Step 10: Run the test executable
/home/xyf/CoSense/applications/newton/llvm-ir/main_out
