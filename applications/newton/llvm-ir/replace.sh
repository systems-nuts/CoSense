#!/bin/bash

# Define the file path
file="MadgwickAHRS_opt.ll"

# Check if the file exists
if [ ! -f "$file" ]; then
    echo "Error: File does not exist."
    exit 1
fi

# Use sed to replace the text
#sed -i 's/declare dso_local i32 @printf(i8\*)/declare dso_local i32 @printf(i8\*, i32)/g' "$file"

#Replace float** with i32** first to avoid conflicting replacements
sed -i 's/float\*\*/i32**/g' "$file"
#
##Replace float* with i32*
sed -i 's/float\*/i32*/g' "$file"

# Now replace the call to invSqrt with the call to fixrsqrt
#sed -i 's/call i32 @invSqrt/call i32 @fixrsqrt/g' "$file"

# Replace the specific call to MadgwickAHRSupdateIMU
#sed -i 's/call void @MadgwickAHRSupdateIMU(.*)/call void @MadgwickAHRSupdateIMU(float %0, float %1, float %2, float %3, float %4, float %5, i32* %119, i32* %120, i32* %121, i32* %122)/g' "$file"
#sed -i 's/call void @MadgwickAHRSupdateIMU(.*)/call void @MadgwickAHRSupdateIMU(float %0, float %1, float %2, float %3, float %4, float %5)/g' "$file"
#sed -i 's/call void @MadgwickAHRSupdateIMU(.*)/call void @MadgwickAHRSupdateIMU(i32 %83, i32 %84, i32 %85, i32 %86, i32 %87, i32 %88, i32* %89, i32* %90, i32* %91, i32* %92)/g' "$file"

# Remove all occurrences of "_quantized" in the file
sed -i 's/_quantized//g' "$file"

echo "Replacement complete."
