import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# Helper function to parse the text data from file content
def parse_perf_data(file_content, quantization_type):
    data = []
    current_benchmark_key = None
    lines = file_content.strip().split('\n')

    # Skip header line if present
    if lines and "test case\tparam" in lines[0]:
        lines = lines[1:]

    for line in lines:
        parts = line.split('\t')
        if not parts or len(parts) < 2: # Basic check for enough parts
            if line.strip():
                print(f"Skipping line due to insufficient parts: '{line[:60]}...'")
            continue

        test_case = parts[0].strip()

        if test_case.startswith("perf_j0"):
            current_benchmark_key = "j0"
        elif test_case.startswith("perf_y0"):
            current_benchmark_key = "y0"
        elif test_case == "speed up after optimization":
            if current_benchmark_key is None:
                print(f"Skipping 'speed up' line due to missing benchmark context: '{line[:60]}...'")
                continue

            try:
                # Ensure enough parts for "speed up after optimization" line
                if len(parts) < 6:
                    print(f"Skipping 'speed up' line due to insufficient data columns: '{line[:60]}...'")
                    continue

                params_str = parts[1].strip()
                param_values = params_str.split()
                if len(param_values) < 2:
                    print(f"Skipping 'speed up' line due to invalid params format: '{line[:60]}...'")
                    continue
                # Format params for display: "[val1, val2]"
                formatted_params = f"[{float(param_values[0]):.1f}, {float(param_values[1]):.1f}]"

                # Time consumption speedup is at index 3 (4th item based on 0-indexing)
                time_speedup_pct = float(parts[3].replace('%', ''))
                # Library size reduction is at index 5 (6th item based on 0-indexing)
                library_size_reduction_pct = float(parts[5].replace('%', ''))

                # Speedup factor: 1 + (percentage / 100)
                # e.g., 4% speedup -> 1.04x; 0% speedup -> 1.00x
                speedup_factor = 1 + (time_speedup_pct / 100.0)

                data.append({
                    "benchmark": current_benchmark_key,
                    "params_raw": params_str, # For sorting
                    "params_formatted": formatted_params, # For display
                    "quant_type": quantization_type,
                    "speedup_factor": speedup_factor,
                    "library_size_reduction_pct": library_size_reduction_pct
                })
            except (IndexError, ValueError) as e:
                print(f"Error parsing 'speed up' data line: '{line[:60]}...' due to: {e}")
                continue
    return data

# --- Main script execution ---
file_path_wo_quant = "perf_woquant.log"
file_path_w_quant = "perf_quant.log"
all_data_collected = []

# Process data without quantization
try:
    with open(file_path_wo_quant, 'r', encoding='utf-8') as f:
        perf_woquant_content_from_file = f.read()
    data_wo_quant_parsed = parse_perf_data(perf_woquant_content_from_file, "w/o Auto Quantization")
    all_data_collected.extend(data_wo_quant_parsed)
except FileNotFoundError:
    print(f"Error: File not found '{file_path_wo_quant}'. Please ensure it's in the correct location.")
    exit()
except Exception as e:
    print(f"Error reading or parsing '{file_path_wo_quant}': {e}")
    exit()

# Process data with quantization
try:
    with open(file_path_w_quant, 'r', encoding='utf-8') as f:
        perf_quant_content_from_file = f.read()
    data_w_quant_parsed = parse_perf_data(perf_quant_content_from_file, "with Auto Quantization")
    all_data_collected.extend(data_w_quant_parsed)
except FileNotFoundError:
    print(f"Error: File not found '{file_path_w_quant}'. Please ensure it's in the correct location.")
    exit()
except Exception as e:
    print(f"Error reading or parsing '{file_path_w_quant}': {e}")
    exit()

if not all_data_collected:
    print("No data was successfully parsed from the files. Exiting.")
    exit()

df_performance = pd.DataFrame(all_data_collected)

if df_performance.empty:
    print("DataFrame is empty after parsing. No plots will be generated. Check parsing logs if any.")
    exit()

# Function to create and save grouped bar charts
def create_and_save_plot(df_benchmark_data, benchmark_id, metric_column, y_axis_label, plot_title_suffix, output_filename):
    if df_benchmark_data.empty:
        print(f"No data available for {benchmark_id} {plot_title_suffix.lower()}, skipping plot.")
        return

    try:
        pivot_table_df = df_benchmark_data.pivot_table(index="params_formatted",
                                                       columns="quant_type",
                                                       values=metric_column)
    except Exception as e:
        print(f"Could not create pivot table for {benchmark_id} {plot_title_suffix.lower()}: {e}")
        return

    # Define the desired order of columns for plotting
    ordered_quant_types = [qt for qt in ["w/o Auto Quantization", "with Auto Quantization"] if qt in pivot_table_df.columns]

    if not ordered_quant_types:
        print(f"Pivot table for {benchmark_id} {plot_title_suffix.lower()} has no quantization type columns. Skipping.")
        return
    pivot_table_df = pivot_table_df[ordered_quant_types]

    # Sort index (params_formatted) based on the numerical values of params_raw for consistent plotting order
    unique_params_temp_df = df_benchmark_data[['params_raw', 'params_formatted']].drop_duplicates().copy()
    unique_params_temp_df['sort_key_internal'] = unique_params_temp_df['params_raw'].apply(lambda x: tuple(map(float, x.split())))
    unique_params_temp_df = unique_params_temp_df.sort_values('sort_key_internal')
    final_ordered_params = unique_params_temp_df['params_formatted'].tolist()

    pivot_table_df = pivot_table_df.reindex(final_ordered_params).dropna(axis=0, how='all')

    if pivot_table_df.empty:
        print(f"Pivot table became empty after reindexing for {benchmark_id} {plot_title_suffix.lower()}. Skipping.")
        return

    num_param_groups = len(pivot_table_df.index)
    bar_item_width = 0.35  # Width of individual bars
    x_positions = np.arange(num_param_groups) # Base positions for groups

    # Determine overall figure width dynamically
    fig_width = max(12, num_param_groups * 0.7)
    fig, ax = plt.subplots(figsize=(fig_width, 7))

    # Plotting bars for each quantization type
    num_quant_types = len(ordered_quant_types)
    for i, quant_type in enumerate(ordered_quant_types):
        # Calculate offset for each bar within a group to achieve grouped bar chart
        offset = (i - (num_quant_types - 1) / 2) * bar_item_width
        ax.bar(x_positions + offset, pivot_table_df[quant_type].fillna(0), bar_item_width,
               label=quant_type, color=['#C3B1E1', '#6A0DAD'][i % 2]) # Light purple, Dark purple

    ax.set_xlabel("Parameters (Range)")
    ax.set_ylabel(y_axis_label)
    ax.set_title(f"{benchmark_id.upper()} {plot_title_suffix}")
    ax.set_xticks(x_positions)
    ax.set_xticklabels(pivot_table_df.index, rotation=45, ha="right")
    ax.legend(title="Quantization Type")
    ax.grid(True, linestyle='--', alpha=0.7, axis='y') # Horizontal grid lines only

    # Adjust y-axis limits for better visualization
    if "Speedup Factor" in y_axis_label:
        ax.axhline(1.0, color='grey', linestyle='-.', linewidth=0.8) # Reference line at 1.0x speedup
        min_data_val = pivot_table_df[ordered_quant_types].min().min()
        max_data_val = pivot_table_df[ordered_quant_types].max().max()
        if pd.notna(min_data_val) and pd.notna(max_data_val):
            ax.set_ylim(bottom=min(0.9, min_data_val - 0.05), top=max_data_val + 0.05)
    elif "Reduction (%)" in y_axis_label:
        min_data_val = pivot_table_df[ordered_quant_types].min().min()
        max_data_val = pivot_table_df[ordered_quant_types].max().max()
        if pd.notna(min_data_val) and pd.notna(max_data_val):
            ax.set_ylim(bottom=min(0, min_data_val - 5), top=max_data_val + 5) # Give some padding

    plt.tight_layout() # Adjust layout to prevent labels from overlapping
    plt.savefig(output_filename)
    print(f"Plot saved: {output_filename}")
    plt.close(fig) # Close the figure to free memory


# --- Generate and Save Plots ---

# For e_j0 benchmark
df_j0_data = df_performance[df_performance["benchmark"] == "j0"].copy()
if not df_j0_data.empty:
    create_and_save_plot(df_j0_data, "e_j0", "speedup_factor",
                         "Speedup Factor (x)", "Speedup Comparison", "e_j0_speedup_comparison.png")
    create_and_save_plot(df_j0_data, "e_j0", "library_size_reduction_pct",
                         "Library Size Reduction (%)", "Library Size Reduction Comparison", "e_j0_size_reduction_comparison.png")
else:
    print("No data processed for e_j0 benchmark.")

# For e_y0 benchmark
df_y0_data = df_performance[df_performance["benchmark"] == "y0"].copy()
if not df_y0_data.empty:
    create_and_save_plot(df_y0_data, "e_y0", "speedup_factor",
                         "Speedup Factor (x)", "Speedup Comparison", "e_y0_speedup_comparison.png")
    create_and_save_plot(df_y0_data, "e_y0", "library_size_reduction_pct",
                         "Library Size Reduction (%)", "Library Size Reduction Comparison", "e_y0_size_reduction_comparison.png")
else:
    print("No data processed for e_y0 benchmark.")

print("--- Plot generation complete ---")