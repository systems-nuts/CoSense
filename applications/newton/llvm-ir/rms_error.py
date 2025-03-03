import math
import argparse

def parse_data(file_path, prefix_to_remove):
    """
    Reads a file and extracts q0, q1, q2, q3 values from each line after removing the specified prefix.
    Returns a list of lists containing the extracted values.
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

def calculate_rms_errors(fp_data, int_data):
    """
    Calculate RMS errors for each quaternion component (q0, q1, q2, q3) and overall RMS.
    Returns (rms_q0, rms_q1, rms_q2, rms_q3, rms_overall, avg_rms).
    """
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

    # RMS for each component
    rms_q0 = math.sqrt(sum(errors_q0) / len(errors_q0))
    rms_q1 = math.sqrt(sum(errors_q1) / len(errors_q1))
    rms_q2 = math.sqrt(sum(errors_q2) / len(errors_q2))
    rms_q3 = math.sqrt(sum(errors_q3) / len(errors_q3))

    # Overall RMS error (combined four components)
    rms_overall = math.sqrt(sum(overall_errors) / len(overall_errors))

    # Average of RMS values (average of q0, q1, q2, q3 RMS)
    avg_rms = (rms_q0 + rms_q1 + rms_q2 + rms_q3) / 4

    return rms_q0, rms_q1, rms_q2, rms_q3, rms_overall, avg_rms

def compute_snr(fp_data, int_data, assume_signal_power=1.0):
    """
    Calculate SNR (in dB). Assumes:
    1) The original signal power = assume_signal_power (default 1)
    2) Noise power = (RMS Error)^2 (overall RMS)
    """
    _, _, _, _, rms_overall, _ = calculate_rms_errors(fp_data, int_data)
    p_noise = rms_overall ** 2
    p_signal = assume_signal_power
    if p_noise == 0:
        return float('inf')
    snr_db = 10 * math.log10(p_signal / p_noise)
    return snr_db

def compute_real_signal_power(fp_data):
    """
    If you want to compute the real signal power from FP data.
    P_signal = mean( q0^2 + q1^2 + q2^2 + q3^2 ) over the dataset.
    """
    total = 0.0
    for fp in fp_data:
        q0, q1, q2, q3 = fp
        total += (q0**2 + q1**2 + q2**2 + q3**2)
    return total / len(fp_data)

def main():
    parser = argparse.ArgumentParser(description="Calculate RMS error and SNR for quaternion data.")
    parser.add_argument("--filter", choices=["madgwick", "mahony"], default="madgwick",
                        help="Select which algorithm's data to process (default: madgwick)")
    args = parser.parse_args()

    if args.filter == "madgwick":
        fp_data = parse_data('fp_result.txt', 'Original: ')
        int_data = parse_data('int_result.txt', 'FIX: ')
    else:  # mahony
        fp_data = parse_data('fp_mahony_result.txt', 'Original: ')
        int_data = parse_data('int_mahony_result.txt', 'FIX: ')

    if len(fp_data) != len(int_data):
        print("Mismatch in the number of quaternions between files!")
        return

    # Use only the first 50 data lines
    fp_data = fp_data[:50]
    int_data = int_data[:50]

    # Calculate RMS errors
    rms_q0, rms_q1, rms_q2, rms_q3, rms_overall, avg_rms = calculate_rms_errors(fp_data, int_data)
    print(f"RMS Error (q0): {rms_q0:.6f}")
    print(f"RMS Error (q1): {rms_q1:.6f}")
    print(f"RMS Error (q2): {rms_q2:.6f}")
    print(f"RMS Error (q3): {rms_q3:.6f}")
    print(f"Overall RMS Error: {rms_overall:.6f}")
    print(f"Average of RMS Values: {avg_rms:.6f}")

    # Calculate SNR (assuming original signal power = 1)
    snr_db = compute_snr(fp_data, int_data, assume_signal_power=1.0)
    print(f"SNR (dB) [Assume P_signal=1]: {snr_db:.2f}")

    # For debugging: print first line error values if needed
    first_fp = fp_data[0]
    first_int = int_data[0]
    first_error = [first_fp[i] - first_int[i] for i in range(4)]
    print(f"First line FP: {first_fp}")
    print(f"First line INT: {first_int}")
    print(f"First line errors (q0, q1, q2, q3): {first_error}")

if __name__ == "__main__":
    main()
