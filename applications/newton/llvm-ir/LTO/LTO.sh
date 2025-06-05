#!/bin/bash

# 定义路径
BASE_PATH="$HOME/CoSense/applications/newton/llvm-ir"
SRC_PATH="$BASE_PATH/LTO"
OUTPUT_PATH="$BASE_PATH/LTO/ir"


echo "Step 1: Generate LLVM IR files and bitcode files for all source files"
for file in "$SRC_PATH"/*.c; do
    filename=$(basename -- "$file" .c)
    ir_file="$OUTPUT_PATH/${filename}.ll"
    bc_file="$OUTPUT_PATH/${filename}.bc"

    echo "Generating LLVM IR for $file -> $ir_file"
    clang -g0 -O0 -Xclang -disable-O0-optnone -S -emit-llvm -Wall -Wextra -o "$ir_file" "$file"
    if [ $? -ne 0 ]; then
        echo "Error generating LLVM IR for $file"
        exit 1
    fi

    echo "Optimizing $ir_file with mem2reg"
    opt --mem2reg -S -o "$ir_file" "$ir_file"
    if [ $? -ne 0 ]; then
        echo "Error optimizing $ir_file with mem2reg"
        exit 1
    fi

    echo "Converting $ir_file to $bc_file"
    llvm-as "$ir_file" -o "$bc_file"



    if [ $? -ne 0 ]; then
        echo "Error converting $ir_file to bitcode"
        exit 1
    fi
done

    llvm-nm $OUTPUT_PATH/*.bc



echo "Step 2: Merge LLVM bitcode files into merged.bc"
llvm-link --only-needed "$OUTPUT_PATH"/*.bc -o "$OUTPUT_PATH/merged.bc"
if [ $? -ne 0 ]; then
    echo "Error merging bitcode files"
    exit 1
fi
echo "Merged bitcode files into $OUTPUT_PATH/merged.bc"

echo "Step 3: Convert merged.bc back to LLVM IR (merged.ll)"
llvm-dis "$OUTPUT_PATH/merged.bc" -o "$OUTPUT_PATH/merged.ll"
if [ $? -ne 0 ]; then
    echo "Error converting merged.bc to LLVM IR"
    exit 1
fi
echo "Converted $OUTPUT_PATH/merged.bc to $OUTPUT_PATH/merged.ll"


echo "Step 3: Use newton for optimization and quantization"
cd $HOME/CoSense/src/newton && ./newton-linux-EN \
    --llvm-ir=$OUTPUT_PATH/merged.ll \
    --llvm-ir-liveness-check \
    --llvm-ir-auto-quantization $HOME/CoSense/applications/newton/sensors/BMX055.nt

#echo "Step 4: Optimize the quantized LLVM IR file"
#opt -inline -S -o $OUTPUT_PATH/out.ll $OUTPUT_PATH/merged_output.ll


#echo "Step 5: Compile the optimized LLVM IR file to bitcode"
#llvm-as $OUTPUT_PATH/out.ll -o $OUTPUT_PATH/out.bc
#
#
#echo "Step 6: Compile the bitcode file to assembly"
#llc $OUTPUT_PATH/out.bc -o $OUTPUT_PATH/out.s
#
#
#echo "Step 7: Compile the assembly file to object file"
#clang -c $OUTPUT_PATH/out.s -o $OUTPUT_PATH/out.o
#
#
#echo "Step 8: Package the object file into a static library"
#ar -rc $OUTPUT_PATH/libout.a $OUTPUT_PATH/out.o
#
#
#echo "Step 9: Compile the test file and link with the static library"
#clang $OUTPUT_PATH/test_madgwick.c -D INT_DATA_TYPE -no-pie -L$OUTPUT_PATH -lout -O3 -Os -g -o $OUTPUT_PATH/main_out -lm
#
#
#echo "Step 10: Run the test executable"
#$OUTPUT_PATH/main_out
