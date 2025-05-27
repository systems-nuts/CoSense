import matplotlib.pyplot as plt
import sys
import re
import os

def parse_sqnr_file(filename):
    bits = []
    sqnrs = []
    with open(filename, 'r') as f:
        for line in f:
            match = re.match(r"\|\s*(\d+)\s*\|\s*([\d.]+)\s*\|", line)
            if match:
                bits.append(int(match.group(1)))
                sqnrs.append(float(match.group(2)))
    return bits, sqnrs

def extract_filter_name(filename):
    base = os.path.basename(filename)
    if "madgwickahrs" in base:
        return "MadgwickAHRS"
    elif "mahonyahrs" in base:
        return "MahonyAHRS"
    elif "sensfusion6" in base:
        return "Sensfusion6"
    else:
        return "Unknown"

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 plot_sqnr.py <sqnr_result_file>")
        sys.exit(1)

    filename = sys.argv[1]
    bits, sqnrs = parse_sqnr_file(filename)
    filter_name = extract_filter_name(filename)

    plt.figure()
    plt.plot(bits, sqnrs, marker='o')

    plt.xticks(bits)

    plt.title(f'SQNR vs. MAX_PRECISION_BITS for {filter_name} Filter')
    plt.xlabel('MAX_PRECISION_BITS (FRAC_Q)')
    plt.ylabel('SQNR (dB)')
    plt.grid(True)
    plt.tight_layout()

    output_img = filename.replace(".txt", ".png")
    plt.savefig(output_img)
    print(f"Plot saved to {output_img}")
