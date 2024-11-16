import math

# Function to convert quaternions to Euler angles
def quaternion_to_euler(q0, q1, q2, q3):
    roll = math.atan2(2 * (q0 * q1 + q2 * q3), 1 - 2 * (q1**2 + q2**2))
    pitch = math.asin(max(-1.0, min(1.0, 2 * (q0 * q2 - q3 * q1))))  # Clamp to avoid numerical errors
    yaw = math.atan2(2 * (q0 * q3 + q1 * q2), 1 - 2 * (q2**2 + q3**2))
    return roll, pitch, yaw

# Parse the quaternion values from a file
def parse_quaternions(filename, prefix):
    quaternions = []
    with open(filename, 'r') as file:
        for line in file:
            if line.startswith(prefix):
                parts = line.split(',')
                q0 = float(parts[0].split('=')[1].strip())
                q1 = float(parts[1].split('=')[1].strip())
                q2 = float(parts[2].split('=')[1].strip())
                q3 = float(parts[3].split('=')[1].strip())
                quaternions.append((q0, q1, q2, q3))
    return quaternions

# Calculate RMS error between two lists of Euler angles
def calculate_rms_error(fp_euler, int_euler):
    errors_roll = []
    errors_pitch = []
    errors_yaw = []
    overall_errors = []

    for fp, integer in zip(fp_euler, int_euler):
        roll_error = math.degrees(fp[0] - integer[0])  # Convert to degrees
        pitch_error = math.degrees(fp[1] - integer[1])
        yaw_error = math.degrees(fp[2] - integer[2])

        errors_roll.append(roll_error**2)
        errors_pitch.append(pitch_error**2)
        errors_yaw.append(yaw_error**2)

        # Accumulate the overall squared error for all components
        overall_errors.append(roll_error**2 + pitch_error**2 + yaw_error**2)

    rms_roll = math.sqrt(sum(errors_roll) / len(errors_roll))
    rms_pitch = math.sqrt(sum(errors_pitch) / len(errors_pitch))
    rms_yaw = math.sqrt(sum(errors_yaw) / len(errors_yaw))

    # Overall RMS error
    rms_overall = math.sqrt(sum(overall_errors) / len(overall_errors))

    return rms_roll, rms_pitch, rms_yaw, rms_overall

# Main function to compute the average Euler angle errors
def main():
    fp_quaternions = parse_quaternions('fp_result.txt', 'Original')
    int_quaternions = parse_quaternions('int_result.txt', 'FIX')

    if len(fp_quaternions) != len(int_quaternions):
        print("Mismatch in the number of quaternions between files!")
        return

    # Convert quaternions to Euler angles
    fp_euler_angles = [quaternion_to_euler(*q) for q in fp_quaternions]
    int_euler_angles = [quaternion_to_euler(*q) for q in int_quaternions]

    # Calculate RMS error for roll, pitch, yaw, and overall
    rms_roll, rms_pitch, rms_yaw, rms_overall = calculate_rms_error(fp_euler_angles, int_euler_angles)

    print(f"RMS Error (φₑ - Roll): {rms_roll:.6f} degrees")
    print(f"RMS Error (θₑ - Pitch): {rms_pitch:.6f} degrees")
    print(f"RMS Error (ψₑ - Yaw): {rms_yaw:.6f} degrees")
    print(f"Overall RMS Error: {rms_overall:.6f} degrees")

# Run the main function
if __name__ == "__main__":
    main()
