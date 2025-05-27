# Step 1: Generate LLVM IR file from the C file
echo "Step 1: Generate LLVM IR file from the C file"
clang -g -O0 -Xclang -disable-O0-optnone -S -emit-llvm -Wall -Wextra -o /home/xyf/CoSense/applications/newton/llvm-ir/original.ll /home/xyf/CoSense/applications/newton/llvm-ir/c-files/original.c

# Step 2: Convert generated LLVM IR file back to bitcode
echo "Step 2: Convert generated LLVM IR file back to bitcode"
llvm-as /home/xyf/CoSense/applications/newton/llvm-ir/original.ll -o /home/xyf/CoSense/applications/newton/llvm-ir/original.bc

# Step 3: Optimize the LLVM bitcode file
echo "Step 3: Optimize the LLVM bitcode file"
opt /home/xyf/CoSense/applications/newton/llvm-ir/original.bc --simplifycfg --instsimplify -O3 -Os -o /home/xyf/CoSense/applications/newton/llvm-ir/original_opt.bc

# Step 4: Convert optimized bitcode back to LLVM IR
echo "Step 4: Convert optimized bitcode back to LLVM IR"
llvm-dis /home/xyf/CoSense/applications/newton/llvm-ir/original_opt.bc -o /home/xyf/CoSense/applications/newton/llvm-ir/original_opt.ll

# Step 5: Compile the optimized LLVM IR file to assembly
echo "Step 5: Compile the optimized LLVM IR file to assembly"
llc /home/xyf/CoSense/applications/newton/llvm-ir/original_opt.ll -o /home/xyf/CoSense/applications/newton/llvm-ir/original.s

# Step 6: Compile the assembly file to object file
echo "Step 6: Compile the assembly file to object file"
clang -c /home/xyf/CoSense/applications/newton/llvm-ir/original.s -o /home/xyf/CoSense/applications/newton/llvm-ir/original.o

# Step 7: Package the object file into a static library
echo "Step 7: Package the object file into a static library"
ar -rc /home/xyf/CoSense/applications/newton/llvm-ir/liboriginal.a /home/xyf/CoSense/applications/newton/llvm-ir/original.o
