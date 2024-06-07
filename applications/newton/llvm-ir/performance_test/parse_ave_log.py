import pandas as pd
import os

def parse_log_file(input_file, output_file):
    if input_file == "aarch64_average_speedup.log":
        sheet_name = "arm64"
    else:
        sheet_name = "x86"
    # Read the log file into a pandas DataFrame
    df = pd.read_csv(input_file, delimiter='\t', skipinitialspace=True, header=None)

    # Initialize lists to store data for each column
    exp_names = []
    time_consumptions = []
    lib_sizes = []
    lib_size_reduces = []

    # Iterate through each row in the DataFrame
    for index, row in df.iterrows():
        exp_name = f"{row[0]} with time speed up"
        time_consumption = row[2]
        lib_size = f"lib size reduce"
        lib_size_reduce = row[4]

        # Append data to respective lists
        exp_names.append(exp_name)
        time_consumptions.append(time_consumption)
        lib_sizes.append(lib_size)
        lib_size_reduces.append(lib_size_reduce)

    # Create a DataFrame from the list of rows
    result_df = pd.DataFrame({'exp_names': exp_names,
                              'time_consumptions': time_consumptions,
                              'lib_sizes': lib_sizes,
                              'lib_size_reduces': lib_size_reduces})
    # Delete the first redundant row
    result_df = result_df.iloc[1:]

    # Append to existing Excel file or create a new one
    if os.path.exists(output_file):
        mode = "a"
    else:
        mode = "w"
    with pd.ExcelWriter(output_file, engine='openpyxl', mode=mode) as writer:
        # Write the result to an Excel file without row headers
        result_df.to_excel(writer, index=False, header=False, sheet_name=sheet_name)

if __name__ == "__main__":
    output_file = "overall_speedup_microbenchmarks.xlsx"
    if os.path.exists(output_file):
        os.remove(output_file)

    arm_input_file = "aarch64_average_speedup.log"
    parse_log_file(arm_input_file, output_file)
    x86_input_file = "x86_64_average_speedup.log"
    parse_log_file(x86_input_file, output_file)

