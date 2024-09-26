import numpy as np

# Function to parse the data from the files after removing the prefix
def parse_data(file_path, prefix_to_remove):
    """
    Reads a file and extracts q0, q1, q2, q3 values from each line after removing the specified prefix.
    Returns a numpy array of the extracted values.
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

    # Debug: Print the number of parsed lines and the first few entries
    if len(data) == 0:
        print(f"No data found in file: {file_path}")
    else:
        print(f"Parsed {len(data)} lines from {file_path}")
        print("First few entries:", data[:3])

    return np.array(data)

# Load and clean the data from both files
fp_data = parse_data('fp_result.txt', prefix_to_remove="Original: ")
int_data = parse_data('int_result.txt', prefix_to_remove="FIX: ")

# Check if data is loaded correctly
if fp_data.size == 0 or int_data.size == 0:
    print("Error: One of the datasets is empty. Please check the input files.")
else:
    # Calculate the absolute difference between the floating point and integer results
    error = np.abs(fp_data - int_data)

    # Calculate the mean and max errors for each q value (q0, q1, q2, q3)
    mean_error = np.mean(error, axis=0)
    max_error = np.max(error, axis=0)

    # Display the results
    print("Mean Error for each q value:")
    print(f"q0: {mean_error[0]:.6f}, q1: {mean_error[1]:.6f}, q2: {mean_error[2]:.6f}, q3: {mean_error[3]:.6f}")

    print("\nMax Error for each q value:")
    print(f"q0: {max_error[0]:.6f}, q1: {max_error[1]:.6f}, q2: {max_error[2]:.6f}, q3: {max_error[3]:.6f}")

    # Display the summary of errors in a dictionary
    error_summary = {
        "Mean Error": mean_error,
        "Max Error": max_error
    }

    # Print the summary dictionary
    print("\nError Summary:")
    print(error_summary)
