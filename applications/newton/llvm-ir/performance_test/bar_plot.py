import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# Function to build a map of (benchmark, params_raw) -> precision_bits
# by parsing the primary data lines of the quant_file content.
def build_precision_map_from_quant_file(file_content):
    precision_map = {}
    current_benchmark_key = None
    lines = file_content.strip().split('\n')

    header_skipped = False
    if lines and "test case\tparam" in lines[0]:
        lines = lines[1:]
        header_skipped = True

    # parts[0]=test_case, parts[1]=param, parts[2]=precision_bits
    precision_bits_col_index = 2
    # Minimum parts needed: test_case, param, precision_bits ...
    min_parts_for_precision_line = precision_bits_col_index + 1

    for line in lines:
        parts = line.split('\t')
        if not parts: continue

        test_case = parts[0].strip()

        # Only process primary data lines, not "speed up..." lines
        if test_case.startswith("perf_j0"):
            current_benchmark_key = "j0"
        elif test_case.startswith("perf_y0"):
            current_benchmark_key = "y0"
        else: # Skip "speed up..." lines or other unrecognized lines
            continue

        if current_benchmark_key and len(parts) > min_parts_for_precision_line:
            params_str = parts[1].strip()
            # Validate params_str before splitting
            if len(params_str.split()) >= 2:
                try:
                    precision_val = parts[precision_bits_col_index].strip()
                    # Ensure precision_val is not empty and is a valid integer
                    if precision_val:
                        int(precision_val) # Test conversion
                        map_key = (current_benchmark_key, params_str)
                        if map_key not in precision_map: # Store first encountered for a given param
                            precision_map[map_key] = precision_val
                        # else:
                        #    print(f"Debug: Duplicate param '{params_str}' for benchmark '{current_benchmark_key}' in precision map building. Using first value.")
                except (IndexError, ValueError) as e:
                    print(f"Warning: Could not parse precision_bits from line: '{line[:80]}...' Error: {e}")
            # else:
            # print(f"Debug: Invalid params_str '{params_str}' for precision map building from line: '{line[:80]}...'")

    if not precision_map:
        print("Warning: Precision map is empty. Check 'perf_quant.log' format and 'precision_bits_col_index'.")
    # else:
    # print(f"Debug: Precision map built with {len(precision_map)} entries.")
    # for k, v in list(precision_map.items())[:5]: print(f"  Map sample: {k} -> {v}")

    return precision_map

# Helper function to parse the "speed up after optimization" lines
def parse_summary_perf_data(file_content, quantization_type):
    data = []
    current_benchmark_key = None
    lines = file_content.strip().split('\n')

    if lines and "test case\tparam" in lines[0]:
        lines = lines[1:]

    for line in lines:
        parts = line.split('\t')
        if not parts: continue

        test_case = parts[0].strip()

        if test_case.startswith("perf_j0"):
            current_benchmark_key = "j0"
        elif test_case.startswith("perf_y0"):
            current_benchmark_key = "y0"
        elif test_case == "speed up after optimization":
            if current_benchmark_key is None:
                print(f"Skipping 'speed up' line (no benchmark context): '{line[:60]}...'")
                continue

            # Standard "speed up after optimization" line structure:
            # test case(0)|param(1)|instr%(2)|time%(3)|ir%(4)|lib_size%(5)|compile_time%(6)
            if len(parts) < 7:
                print(f"Skipping 'speed up' line (insufficient columns): '{line[:60]}...'")
                continue

            try:
                params_str = parts[1].strip()
                param_values = params_str.split()
                if len(param_values) < 2:
                    print(f"Skipping 'speed up' line (invalid params format): '{line[:60]}...'")
                    continue
                formatted_params = f"[{float(param_values[0]):.1f}, {float(param_values[1]):.1f}]"

                time_speedup_pct = float(parts[3].replace('%', ''))
                library_size_reduction_pct = float(parts[5].replace('%', ''))
                speedup_factor = 1 + (time_speedup_pct / 100.0)

                entry = {
                    "benchmark": current_benchmark_key,
                    "params_raw": params_str,
                    "params_formatted": formatted_params,
                    "quant_type": quantization_type,
                    "speedup_factor": speedup_factor,
                    "library_size_reduction_pct": library_size_reduction_pct,
                    "precision_bits": None # Will be filled in later using the map
                }
                data.append(entry)
            except (IndexError, ValueError) as e:
                print(f"Error parsing 'speed up' data line: '{line[:60]}...' due to: {e}")
                continue
    return data

# --- Main script execution ---
file_path_wo_quant = "perf_woquant.log"
file_path_w_quant = "perf_quant.log"

# 1. Build precision_bits map from perf_quant.log
quant_precision_map = {}
try:
    with open(file_path_w_quant, 'r', encoding='utf-8') as f:
        perf_quant_content_for_map = f.read()
    quant_precision_map = build_precision_map_from_quant_file(perf_quant_content_for_map)
except FileNotFoundError:
    print(f"Error: File not found '{file_path_w_quant}'. Precision bits cannot be mapped.")
    # Continue without it, precision_bits will remain None for all entries
except Exception as e:
    print(f"Error building precision map from '{file_path_w_quant}': {e}")


# 2. Parse summary performance data for both files
all_data_collected = []
try:
    with open(file_path_wo_quant, 'r', encoding='utf-8') as f:
        content = f.read()
    parsed_data = parse_summary_perf_data(content, "w/o Auto Quantization")
    all_data_collected.extend(parsed_data)
except FileNotFoundError:
    print(f"Warning: File not found '{file_path_wo_quant}'.")
except Exception as e:
    print(f"Error reading or parsing '{file_path_wo_quant}': {e}")

try:
    with open(file_path_w_quant, 'r', encoding='utf-8') as f:
        content = f.read()
    # Note: perf_quant.log's "speed up..." lines are parsed like wo_quant's here
    parsed_data = parse_summary_perf_data(content, "with Auto Quantization")
    all_data_collected.extend(parsed_data)
except FileNotFoundError:
    print(f"Warning: File not found '{file_path_w_quant}'.")
except Exception as e:
    print(f"Error reading or parsing '{file_path_w_quant}': {e}")


if not all_data_collected:
    print("No summary data was successfully parsed from any file. Exiting.")
    exit()

# 3. Add precision_bits to all entries using the map
for item in all_data_collected:
    key = (item['benchmark'], item['params_raw'])
    item['precision_bits'] = quant_precision_map.get(key)

df_performance = pd.DataFrame(all_data_collected)

if df_performance.empty:
    print("DataFrame is empty after parsing and mapping. No plots will be generated.")
    exit()

# 4. Create the 'range_over_precision_bits' column for X-axis display
def create_range_precision_label(row):
    if pd.notna(row['precision_bits']) and str(row['precision_bits']).strip():
        return f"{row['params_formatted']}/{row['precision_bits']}"
    return row['params_formatted'] # Fallback if no precision_bits

df_performance['range_over_precision_bits'] = df_performance.apply(create_range_precision_label, axis=1)

# Function to create and save grouped bar charts (largely same as before)
def create_and_save_plot(df_benchmark_data, benchmark_id, metric_column, y_axis_label, plot_title_suffix, output_filename):
    if df_benchmark_data.empty:
        print(f"No data available for {benchmark_id} {plot_title_suffix.lower()}, skipping plot.")
        return

    if 'range_over_precision_bits' not in df_benchmark_data.columns:
        print(f"Critical error: 'range_over_precision_bits' column missing in data for {benchmark_id}. Skipping plot.")
        return

    try:
        pivot_table_df = df_benchmark_data.pivot_table(index="range_over_precision_bits",
                                                       columns="quant_type",
                                                       values=metric_column)
    except Exception as e:
        print(f"Could not create pivot table for {benchmark_id} {plot_title_suffix.lower()}: {e}")
        return

    ordered_quant_types = [qt for qt in ["w/o Auto Quantization", "with Auto Quantization"] if qt in pivot_table_df.columns]

    if not ordered_quant_types:
        print(f"Pivot table for {benchmark_id} {plot_title_suffix.lower()} has no quantization type columns (e.g. all NaNs for this metric). Skipping.")
        # print(pivot_table_df.head()) # For debugging
        return
    pivot_table_df = pivot_table_df[ordered_quant_types]

    temp_sort_df = df_benchmark_data[['params_raw', 'range_over_precision_bits']].drop_duplicates().copy()
    temp_sort_df['sort_key_params_numeric'] = temp_sort_df['params_raw'].apply(lambda x: tuple(map(float, x.split())))
    temp_sort_df = temp_sort_df.sort_values(by='sort_key_params_numeric')
    final_ordered_x_labels = temp_sort_df['range_over_precision_bits'].unique().tolist()

    pivot_table_df = pivot_table_df.reindex(final_ordered_x_labels).dropna(axis=0, how='all')

    if pivot_table_df.empty:
        print(f"Pivot table became empty after reindexing for {benchmark_id} {plot_title_suffix.lower()}. Skipping.")
        return

    num_param_groups = len(pivot_table_df.index)
    bar_item_width = 0.35
    x_positions = np.arange(num_param_groups)

    fig_width = max(12, num_param_groups * 0.8)
    fig, ax = plt.subplots(figsize=(fig_width, 7))

    num_quant_types = len(ordered_quant_types)
    for i, quant_type in enumerate(ordered_quant_types):
        offset = (i - (num_quant_types - 1) / 2) * bar_item_width
        # Ensure data for the bar is numeric, default to 0 if NaN
        bar_data = pd.to_numeric(pivot_table_df[quant_type], errors='coerce').fillna(0)
        ax.bar(x_positions + offset, bar_data, bar_item_width,
               label=quant_type, color=['#C3B1E1', '#6A0DAD'][i % 2])

    ax.set_xlabel("Range/PrecisionBits")
    ax.set_ylabel(y_axis_label)
    ax.set_title(f"{benchmark_id.upper()} {plot_title_suffix}")
    ax.set_xticks(x_positions)
    ax.set_xticklabels(pivot_table_df.index, rotation=45, ha="right")
    ax.legend(title="Quantization Type")
    ax.grid(True, linestyle='--', alpha=0.7, axis='y')

    # Dynamically set Y limits based on actual plotted data
    all_bar_values = []
    for qt in ordered_quant_types:
        all_bar_values.extend(pd.to_numeric(pivot_table_df[qt], errors='coerce').fillna(0).tolist())

    if not all_bar_values: # Should not happen if pivot_table_df is not empty
        min_data_val, max_data_val = (0,1) if "Speedup" in y_axis_label else (0,100)
    else:
        min_data_val = min(all_bar_values)
        max_data_val = max(all_bar_values)

    if "Speedup Factor" in y_axis_label:
        ax.axhline(1.0, color='grey', linestyle='-.', linewidth=0.8)
        y_bottom = min(0.9, min_data_val - 0.05) if min_data_val < 5 else 0.9 # Ensure 0.9 is a potential floor
        y_top = max_data_val + 0.05
        if y_bottom >= y_top : y_bottom = y_top - 0.1 # Ensure valid range
        ax.set_ylim(bottom=y_bottom, top=y_top)

    elif "Reduction (%)" in y_axis_label:
        y_bottom = min(0, min_data_val - 5) if min_data_val < 100 else 0
        y_top = max_data_val + 5
        if y_bottom >= y_top : y_bottom = y_top - 10
        ax.set_ylim(bottom=y_bottom, top=y_top)


    plt.tight_layout()
    plt.savefig(output_filename)
    print(f"Plot saved: {output_filename}")
    plt.close(fig)

# --- Generate and Save Plots ---
df_j0_data = df_performance[df_performance["benchmark"] == "j0"].copy()
if not df_j0_data.empty:
    create_and_save_plot(df_j0_data, "e_j0", "speedup_factor",
                         "Speedup Factor (x)", "Speedup Comparison", "e_j0_speedup_comparison.png")
    create_and_save_plot(df_j0_data, "e_j0", "library_size_reduction_pct",
                         "Library Size Reduction (%)", "Library Size Reduction Comparison", "e_j0_size_reduction_comparison.png")
else:
    print("No data processed for e_j0 benchmark.")

df_y0_data = df_performance[df_performance["benchmark"] == "y0"].copy()
if not df_y0_data.empty:
    create_and_save_plot(df_y0_data, "e_y0", "speedup_factor",
                         "Speedup Factor (x)", "Speedup Comparison", "e_y0_speedup_comparison.png")
    create_and_save_plot(df_y0_data, "e_y0", "library_size_reduction_pct",
                         "Library Size Reduction (%)", "Library Size Reduction Comparison", "e_y0_size_reduction_comparison.png")
else:
    print("No data processed for e_y0 benchmark.")

print("--- Plot generation complete ---")