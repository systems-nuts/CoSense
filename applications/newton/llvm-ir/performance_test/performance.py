# Authored 2022. Pei Mu.
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# *	Redistributions of source code must retain the above
# copyright notice, this list of conditions and the following
# disclaimer.
#
# *	Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following
# disclaimer in the documentation and/or other materials
# provided with the distribution.
#
# *	Neither the name of the author nor the names of its
# contributors may be used to endorse or promote products
# derived from this software without specific prior written
# permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

import matplotlib.pyplot as plt
import os
import shutil

performance_data = []
with open('perf.log', 'r') as f:
    for line in f.readlines():
        line_list = line.strip('\n').split('\t')
        performance_data.append(line_list)

# [instruction counts (million)", "time consumption (s)", "IR lines", "size of library (bytes)"]
y_units = [1000, 1000, 1, 1]

params_num = 10
test_case_num = 11
merit_num = 4
range_extend_num = 6

# changed with auto_test.cpp
range_extend_list = [1, 10, 100, 1000, 10000, 100000]

# prepare data
name_list = []
for i in range(1, len(performance_data), 3 * params_num):
    name_list.append(performance_data[i][0])
opt_name_list = []
for i in range(2, len(performance_data), 3):
    opt_name_list.append(performance_data[i][0])

param_list = []
for i in range(1, 3 * params_num * range_extend_num, 3):
    param_list.append(performance_data[i][1])

inst_count = []
for i in range(1, len(performance_data), 3):
    inst_count.append(float(performance_data[i][2]) / y_units[0])
opt_inst_count = []
for i in range(2, len(performance_data), 3):
    opt_inst_count.append(float(performance_data[i][2]) / y_units[0])

time_consumption = []
for i in range(1, len(performance_data), 3):
    time_consumption.append(float(performance_data[i][3]) / y_units[1])
opt_time_consumption = []
for i in range(2, len(performance_data), 3):
    opt_time_consumption.append(float(performance_data[i][3]) / y_units[1])

ir_lines = []
for i in range(1, len(performance_data), 3):
    ir_lines.append(float(performance_data[i][4]) / y_units[2])
opt_ir_lines = []
for i in range(2, len(performance_data), 3):
    opt_ir_lines.append(float(performance_data[i][4]) / y_units[2])

lib_size = []
for i in range(1, len(performance_data), 3):
    lib_size.append(float(performance_data[i][5]) / y_units[3])
opt_lib_size = []
for i in range(2, len(performance_data), 3):
    opt_lib_size.append(float(performance_data[i][5]) / y_units[3])

ori_perf_data = [inst_count, time_consumption, ir_lines, lib_size]
opt_perf_data = [opt_inst_count, opt_time_consumption, opt_ir_lines, opt_lib_size]

fig = plt.figure(1)

y_labels = ["instruction counts (million)", "time consumption (s)", "IR lines", "size of library (bytes)"]
# y_ranges = [(min(opt_inst_count), max(inst_count)), (min(opt_time_consumption), max(time_consumption)),
#             (min(opt_ir_lines), max(ir_lines)), (min(opt_lib_size), max(lib_size))]

fig_path = "fig/"
folder = os.path.exists(fig_path)
if folder:
    shutil.rmtree(fig_path, ignore_errors=True)
os.mkdir(fig_path)

for merit_id in range(merit_num):
    for test_case_id in range(test_case_num):
        for range_extend_id in range(range_extend_num):
            ax1 = plt.figure(merit_id * test_case_num * range_extend_num +
                             test_case_id * range_extend_num + range_extend_id + 1)

            x = list(range(params_num))
            total_width, n = 0.5, 2
            width = total_width / n

            plt_y_begin = params_num * (test_case_id * range_extend_num + range_extend_id)
            plt_y_end = plt_y_begin + params_num
            y_ranges = (min(opt_perf_data[merit_id][plt_y_begin:plt_y_end]) * 0.8,
                        max(ori_perf_data[merit_id][plt_y_begin:plt_y_end]) * 1.2)

            plt.ylim(y_ranges)
            plt.ylabel(y_labels[merit_id])

            plt.bar(x, ori_perf_data[merit_id][plt_y_begin:plt_y_end],
                    width=width, label='basic performance', fc='y')
            for i in range(len(x)):
                x[i] = x[i] + width
            plt.bar(x, opt_perf_data[merit_id][plt_y_begin:plt_y_end],
                    width=width, label='optimized performance', fc='r')
            plt.legend()
            file_name = fig_path + name_list[test_case_id * range_extend_num + range_extend_id] + "-" + \
                        str(range_extend_list[range_extend_id]) + "-" + y_labels[merit_id] + ".pdf"
            file_name = file_name.replace(" ", "_")
            plt.savefig(file_name)
            plt.close()
            if test_case_id == 9:
                break
