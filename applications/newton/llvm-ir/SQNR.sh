#!/bin/bash




while [[ $# -gt 0 ]]; do
    case $1 in
        --algo)
            if [[ "$2" == "MadgwickAHRS" || "$2" == "MahonyAHRS" ]]; then
                ALGO="$2"
                shift 2
            else
                echo "Error: Invalid algorithm '$2'. Use 'MadgwickAHRS' or 'MahonyAHRS'."
                exit 1
            fi
            ;;
        *)
            echo "Usage: $0 [--algo MadgwickAHRS|MahonyAHRS]"
            exit 1
            ;;
    esac
done

OUTPUT_FILE="snr_results_${ALGO,,}.txt"

# Clear old file contents
> "$OUTPUT_FILE"

# Directory where make is executed (source directory)
MAKE_DIR="/home/xyf/CoSense/src/newton"

# Current directory (where the test scripts are located)
TEST_DIR=$(pwd)

# Loop over MAX_PRECISION_BITS values from 13 to 16
for i in {8..16}; do
    echo "---------------------------" | tee -a "$OUTPUT_FILE"
    echo "Current compile parameter: MAX_PRECISION_BITS = $i" | tee -a "$OUTPUT_FILE"

    # Switch to the make directory for compilation
    pushd "$MAKE_DIR" > /dev/null
    # Uncomment the following line if a full clean is needed:
    # make clean
    # Remove object files for quantization and optimizeByRange to force recompilation
    rm -f newton-irPass-LLVMIR-quantization.o newton-irPass-LLVMIR-optimizeByRange.o
    # Compile with the specified MAX_PRECISION_BITS value
    make MAX_PRECISION_BITS=$i
    popd > /dev/null

    # Switch back to the test scripts directory (current directory)
    cd "$TEST_DIR" || exit

    # Run the test script
    ./testMadgwick.sh "$ALGO"

    # Run the Python script to extract the SNR result (adjust grep keyword if needed)
    SNR_LINE=$(python3 rms_error.py --filter "$ALGO" | grep "SQNR (dB)")
    echo "MAX_PRECISION_BITS=$i, $SNR_LINE" | tee -a "$OUTPUT_FILE"

    echo "Compilation and test complete, result saved." | tee -a "$OUTPUT_FILE"
done

echo "All tests completed, results saved in $OUTPUT_FILE."
