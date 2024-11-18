import math

# Function to parse the data from the files after removing the prefix
def parse_data(file_path, prefix_to_remove):
    """
    Reads a file and extracts q0, q1, q2, q3 values from each line after removing the specified prefix.
    Returns a list of tuples containing the extracted values.
    """
    with open(file_path, 'r') as file:
        data = []
        for line in file:
            if line.startswith(prefix_to_remove):
                # Remove the prefix and strip the line
                clean_line = line.replace(prefix_to_remove, "").strip()
                values = clean_line.split(", ")
                # Extract q0, q1, q2, q3 values and convert them to floats
                parsed_values = [float(value.split("=")[1]) for value in values]
                data.append(parsed_values)
    return data

# Calculate RMS errors for q0, q1, q2, q3
def calculate_rms_errors(fp_data, int_data):
    errors_q0 = []
    errors_q1 = []
    errors_q2 = []
    errors_q3 = []
    overall_errors = []

    for fp, integer in zip(fp_data, int_data):
        q0_error = fp[0] - integer[0]
        q1_error = fp[1] - integer[1]
        q2_error = fp[2] - integer[2]
        q3_error = fp[3] - integer[3]

        # Square the errors
        errors_q0.append(q0_error ** 2)
        errors_q1.append(q1_error ** 2)
        errors_q2.append(q2_error ** 2)
        errors_q3.append(q3_error ** 2)

        # Accumulate the overall squared error for all components
        overall_errors.append(q0_error ** 2 + q1_error ** 2 + q2_error ** 2 + q3_error ** 2)

    rms_q0 = math.sqrt(sum(errors_q0) / len(errors_q0))
    rms_q1 = math.sqrt(sum(errors_q1) / len(errors_q1))
    rms_q2 = math.sqrt(sum(errors_q2) / len(errors_q2))
    rms_q3 = math.sqrt(sum(errors_q3) / len(errors_q3))

    # Overall RMS error
    rms_overall = math.sqrt(sum(overall_errors) / len(overall_errors))

    # Average of RMS values
    avg_rms = (rms_q0 + rms_q1 + rms_q2 + rms_q3) / 4

    return rms_q0, rms_q1, rms_q2, rms_q3, rms_overall, avg_rms

# Main function to compute RMS errors
def main():
    fp_data = parse_data('fp_result.txt', 'Original: ')
    int_data = parse_data('int_result.txt', 'FIX: ')

    if len(fp_data) != len(int_data):
        print("Mismatch in the number of quaternions between files!")
        return

    # Calculate RMS errors
    rms_q0, rms_q1, rms_q2, rms_q3, rms_overall, avg_rms = calculate_rms_errors(fp_data, int_data)

    print(f"RMS Error (q0): {rms_q0:.6f}")
    print(f"RMS Error (q1): {rms_q1:.6f}")
    print(f"RMS Error (q2): {rms_q2:.6f}")
    print(f"RMS Error (q3): {rms_q3:.6f}")
    print(f"Overall RMS Error: {rms_overall:.6f}")
    print(f"Average of RMS Values: {avg_rms:.6f}")

# Run the main function
if __name__ == "__main__":
    main()
