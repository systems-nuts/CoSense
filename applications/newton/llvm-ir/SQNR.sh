#!/bin/bash

# Parse input argument for algorithm
while [[ $# -gt 0 ]]; do
    case $1 in
        --algo)
            if [[ "$2" == "MadgwickAHRS" || "$2" == "MahonyAHRS" || "$2" == "sensfusion6" ]]; then
                ALGO="$2"
                shift 2
            else
                echo "Error: Invalid algorithm '$2'. Use 'MadgwickAHRS', 'MahonyAHRS', or 'sensfusion6'."
                exit 1
            fi
            ;;
        *)
            echo "Usage: $0 [--algo MadgwickAHRS|MahonyAHRS|sensfusion6]"
            exit 1
            ;;
    esac
done

OUTPUT_FILE="sqnr_results_${ALGO,,}.txt"

# Clear old file contents
> "$OUTPUT_FILE"

# Directory where make is executed (source directory)
MAKE_DIR="/home/xyf/CoSense/src/newton"

# Current directory (where the test scripts are located)
TEST_DIR=$(pwd)

#Run fp test
./testOriginal.sh "$ALGO"

# Print header into file
echo "| MAX_PRECISION_BITS | SQNR (dB) |" >> "$OUTPUT_FILE"
echo "|--------------------|-----------|" >> "$OUTPUT_FILE"



# Loop over MAX_PRECISION_BITS values from 8 to 18
for i in {12..13}; do
    echo "Compiling and testing with MAX_PRECISION_BITS = $i ..."

    # Switch to the make directory for compilation
    pushd "$MAKE_DIR" > /dev/null
    rm -f newton-irPass-LLVMIR-quantization.o newton-irPass-LLVMIR-optimizeByRange.o
    make MAX_PRECISION_BITS=$i
    popd > /dev/null

    # Switch back to the test scripts directory (current directory)
    cd "$TEST_DIR" || exit

    # Run the test
    ./testMadgwick.sh "$ALGO"

    # Extract sqnr line
    sqnr_VALUE=$(python3 rms_error.py --filter "$ALGO" | grep "SQNR (dB)" | awk -F': ' '{print $2}')
    echo "MAX_PRECISION_BITS=$i, SQNR (dB)=$sqnr_VALUE"

    # Append as a clean table row
    printf "| %-18s | %-9s |\n" "$i" "$sqnr_VALUE" >> "$OUTPUT_FILE"
done

echo "All tests completed. Results saved to $OUTPUT_FILE."
python3 plot_sqnr.py "$OUTPUT_FILE"
