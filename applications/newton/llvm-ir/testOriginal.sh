USER_HOME=$HOME


# Step 1: Generate LLVM IR file
clang -g0 -O0 -Xclang   -disable-O0-optnone -S -emit-llvm -Wall -Wextra  -o $HOME/CoSense/applications/newton/llvm-ir/MadgwickAHRS.ll $HOME/CoSense/applications/newton/llvm-ir/c-files/MadgwickAHRS.c


# Step 4: Optimize the generated LLVM IR file
#opt $HOME/CoSense/applications/newton/llvm-ir/MadgwickAHRS.ll  -O3 -Os -S -o $HOME/CoSense/applications/newton/llvm-ir/out.ll
opt $HOME/CoSense/applications/newton/llvm-ir/MadgwickAHRS.ll   -S -o $HOME/CoSense/applications/newton/llvm-ir/out.ll

# Step 5: Compile the optimized LLVM IR file to bitcode
llvm-as $HOME/CoSense/applications/newton/llvm-ir/out.ll -o $HOME/CoSense/applications/newton/llvm-ir/out.bc

# Step 6: Compile the bitcode file to assembly
llc $HOME/CoSense/applications/newton/llvm-ir/out.bc -o $HOME/CoSense/applications/newton/llvm-ir/out.s

# Step 7: Compile the assembly file to object file
clang -c $HOME/CoSense/applications/newton/llvm-ir/out.s -o $HOME/CoSense/applications/newton/llvm-ir/out.o

# Step 8: Package the object file into a static library
ar -rc $HOME/CoSense/applications/newton/llvm-ir/libout.a $HOME/CoSense/applications/newton/llvm-ir/out.o

# Step 9: Compile the test file and link with the static library
clang $HOME/CoSense/applications/newton/llvm-ir/c-files/test_Original.c -no-pie -L$HOME/CoSense/applications/newton/llvm-ir -lout -O3 -Os -g -fno-builtin -o $HOME/CoSense/applications/newton/llvm-ir/main_out -lm

# Step 10: Run the test executable
$HOME/CoSense/applications/newton/llvm-ir/main_out