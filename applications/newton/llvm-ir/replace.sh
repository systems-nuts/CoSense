#!/bin/bash

# Define the file path
file="MadgwickAHRS_opt.ll"

# Check if the file exists
if [ ! -f "$file" ]; then
    echo "Error: File does not exist."
    exit 1
fi

# Use sed to replace the text
sed -i 's/declare dso_local i32 @printf(i8\*)/declare dso_local i32 @printf(i8\*, i32)/g' "$file"

# Replace float** with i32** first to avoid conflicting replacements
sed -i 's/float\*\*/i32**/g' "$file"

# Replace float* with i32*
sed -i 's/float\*/i32*/g' "$file"






echo "Replacement complete."
