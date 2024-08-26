#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>

#define FRAC_Q 10
#define FRAC_BASE (1 << FRAC_Q)
#define BIT_WIDTH 32
#define ITERATION 10
#define DATA_SIZE 10000

extern volatile float q0, q1, q2, q3;
extern void	      MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz,
					 float * q0_ptr, float * q1_ptr, float * q2_ptr, float * q3_ptr);
extern void	      MadgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az,
					    float * q0_ptr, float * q1_ptr, float * q2_ptr, float * q3_ptr);

/***************************************
 * Timer functions of the test framework
 ***************************************/
typedef struct {
	int quantized1;
	int quantized2;
	int quantized3;
	int quantized4;
} QuantizedValues;

typedef struct {
	float dequantized1;
	float dequantized2;
	float dequantized3;
	float dequantized4;
} DequantizedValues;

typedef struct timespec timespec;

typedef struct {
	uint64_t start;
	uint64_t current;
	uint64_t min;
	uint64_t max;
} ELAPSED_TIME;

ELAPSED_TIME elapsed_time_tbl[10]; // 示例，根据需要调整大小

static inline uint64_t rdtsc() {
	uint32_t lo, hi;
	__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
	return ((uint64_t)hi << 32) | lo;
}



void elapsed_time_start(uint32_t i) {
	elapsed_time_tbl[i].start = rdtsc();
}

void elapsed_time_stop(uint32_t i) {
	uint64_t stop = rdtsc();
	ELAPSED_TIME *p_tbl = &elapsed_time_tbl[i];
	p_tbl->current = stop - p_tbl->start;
	if (p_tbl->max < p_tbl->current) {
		p_tbl->max = p_tbl->current;
	}
	if (p_tbl->min == 0 || p_tbl->min > p_tbl->current) {
		p_tbl->min = p_tbl->current;
	}
}








timespec
diff(timespec start, timespec end)
{
	timespec temp;
	if ((end.tv_nsec - start.tv_nsec) < 0)
	{
		temp.tv_sec  = end.tv_sec - start.tv_sec - 1;
		temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
	}
	else
	{
		temp.tv_sec  = end.tv_sec - start.tv_sec;
		temp.tv_nsec = end.tv_nsec - start.tv_nsec;
	}
	return temp;
}

timespec
sum(timespec t1, timespec t2)
{
	timespec temp;
	if (t1.tv_nsec + t2.tv_nsec >= 1000000000)
	{
		temp.tv_sec  = t1.tv_sec + t2.tv_sec + 1;
		temp.tv_nsec = t1.tv_nsec + t2.tv_nsec - 1000000000;
	}
	else
	{
		temp.tv_sec  = t1.tv_sec + t2.tv_sec;
		temp.tv_nsec = t1.tv_nsec + t2.tv_nsec;
	}
	return temp;
}

void
printTimeSpec(timespec t, const char * prefix)
{
	printf("%s: %d.%09d\n", prefix, (int)t.tv_sec, (int)t.tv_nsec);
}

// Quantization function
int
quantize(float value, int frac_base)
{
	return round(value * frac_base);
}

// Dequantization function
float
dequantize(int quantized_value)
{
	return (float)(quantized_value >> FRAC_Q);
}

QuantizedValues
quantize4(float value1, float value2, float value3, float value4, int frac_base)
{
	QuantizedValues result;
	result.quantized1 = round(value1 * frac_base);
	result.quantized2 = round(value2 * frac_base);
	result.quantized3 = round(value3 * frac_base);
	result.quantized4 = round(value4 * frac_base);
	return result;
}

DequantizedValues
dequantize4(int quantized1, int quantized2, int quantized3, int quantized4)
{
	DequantizedValues result;
	result.dequantized1 = (float)(quantized1 >> FRAC_Q);
	result.dequantized2 = (float)(quantized2 >> FRAC_Q);
	result.dequantized3 = (float)(quantized3 >> FRAC_Q);
	result.dequantized4 = (float)(quantized4 >> FRAC_Q);
	return result;
}

extern int perform_addition(float a, float b);

timespec
tic()
{
	timespec start_time;
	clock_gettime(CLOCK_REALTIME, &start_time);
	return start_time;
}

timespec
toc(timespec * start_time, const char * prefix)
{
	timespec current_time;
	clock_gettime(CLOCK_REALTIME, &current_time);
	timespec time_consump = diff(*start_time, current_time);
	printTimeSpec(time_consump, prefix);
	*start_time = current_time;
	return time_consump;
}
int
main()
{
	FILE * fp = fopen("input.csv", "r");

	if (!fp)
	{
		printf("Can't open file\n");
		return -1;
	}
	double time[DATA_SIZE];

	float q0[DATA_SIZE], q1[DATA_SIZE], q2[DATA_SIZE], q3[DATA_SIZE];
	// quaternion of sensor frame relative to auxiliary frame
	for (int i = 0; i < DATA_SIZE; i++)
	{
		q0[i] = 0.64306622f;
		q1[i] = 0.02828862f;
		q2[i] = -0.00567953f;
		q3[i] = -0.76526684f;
	}

	float mag_x[DATA_SIZE], mag_y[DATA_SIZE], mag_z[DATA_SIZE],
	    gyr_x[DATA_SIZE], gyr_y[DATA_SIZE], gyr_z[DATA_SIZE],
	    acc_x[DATA_SIZE], acc_y[DATA_SIZE], acc_z[DATA_SIZE];

	char buffer[1024];
	int  row = 0, column = 0;

	while (fgets(buffer, 1024, fp))
	{
		column = 0;
		row++;

		if (row == 1)
			continue;

		char * value = strtok(buffer, ", ");

		while (value)
		{
			switch (column)
			{
				case 0:
					time[row - 2] = atof(value);
					break;

				case 1:
					mag_x[row - 2] = atof(value);
					break;
				case 2:
					mag_y[row - 2] = atof(value);
					break;
				case 3:
					mag_z[row - 2] = atof(value);
					break;
				case 4:
					gyr_x[row - 2] = atof(value) * 61;
					break;
				case 5:
					gyr_y[row - 2] = atof(value) * 61;
					break;
				case 6:
					gyr_z[row - 2] = atof(value) * 61;
					break;
				case 7:
					acc_x[row - 2] = atof(value) * 2;
					break;
				case 8:
					acc_y[row - 2] = atof(value) * 2;
					break;
				case 9:
					acc_z[row - 2] = atof(value) * 2;
					break;
				default:
					break;
			}
			value = strtok(NULL, ", ");
			column++;
		}
	}
	fclose(fp);

	u_int64_t time_slots[ITERATION];

	for (size_t idx = 0; idx < ITERATION; idx++)
	{
		elapsed_time_start(idx);  // 开始计时
		timespec timer = tic();
		for (size_t ts = 0; ts < DATA_SIZE; ts++)
		{
			MadgwickAHRSupdate(gyr_x[ts], gyr_y[ts], gyr_z[ts],
					   acc_x[ts], acc_y[ts], acc_z[ts],
					   mag_x[ts], mag_y[ts], mag_z[ts],
					   &q0[ts], &q1[ts], &q2[ts], &q3[ts]);


		}

		//								MadgwickAHRSupdate(gyr_x, gyr_y, gyr_z,
		//										   acc_x, acc_y, acc_z,
		//										   mag_x, mag_y, mag_z,
		//										   &q0, &q1, &q2, &q3);
		elapsed_time_stop(idx);  // 结束计时
		time_slots[idx] = toc(&timer, "computation delay").tv_nsec;
	}



	u_int64_t average_time = 0;
	for (size_t idx = 0; idx < ITERATION; idx++)
	{
		average_time += time_slots[idx];
	}
	average_time /= ITERATION;
	printf("average time = %lu nm\n", average_time);

	// 打印出每次迭代的最大、最小和当前时间
	for (size_t idx = 0; idx < ITERATION; idx++) {
		printf("Iteration %zu: Current Time = %lu, Max Time = %lu, Min Time = %lu\n",
		       idx, elapsed_time_tbl[idx].current, elapsed_time_tbl[idx].max, elapsed_time_tbl[idx].min);
	}

	FILE * fptr = fopen("fp_result.txt", "w");
	for (size_t ts = 0; ts < DATA_SIZE; ts++)
	{
		//        printf("Original: q0[%d]=%f, q1[%d]=%f, q2[%d]=%f, q3[%d]=%f\n",
		//               ts, q0[ts], ts, q1[ts], ts, q2[ts], ts, q3[ts]);
		fprintf(fptr, "Original: q0[%d]=%f, q1[%d]=%f, q2[%d]=%f, q3[%d]=%f\n",
			ts, q0[ts], ts, q1[ts], ts, q2[ts], ts, q3[ts]);
	}
	fclose(fptr);
	return 0;
}


//int main ()
//{
//	/*
//	 * Time (s),Magnetic field x (µT),Magnetic field y (µT),Magnetic field z (µT),Gyroscope x (rad/s),Gyroscope y (rad/s),Gyroscope z (rad/s),Acceleration x (m/s^2),Acceleration y (m/s^2),Acceleration z (m/s^2)
// 0.07969094,-7.249095917,26.43893433,-37.16656494,-0.1184487343,0.001258035656,0.008988874033,-0.3570917249,0.3941296637,9.963726044
//	 */
//
//	float num1 = -7.249095917;
//	float num2 = 26.43893433;
//	float num3 = -37.16656494;
//	float num4 = -0.1184487343;
//	float num5 = 0.001258035656;
//	float num6 = 0.008988874033;
//	float num7 = -0.3570917249;
//	float num8 = 0.3941296637;
//	float num9 = 9.963726044;
//
//	float num10 = 0.64306622;
//	float num11 = 0.02828862;
//	float num12 = -0.00567953;
//	float num13 = -0.76526684;
//
//	// result
//	int num14 = 657;
//	int num15 = 28;
//	int num16 = -6;
//	int num17 = -785;
//
////	int32_t mag_x = quantize(num1, FRAC_BASE);
////	int32_t mag_y = quantize(num2, FRAC_BASE);
////	int32_t mag_z = quantize(num3, FRAC_BASE);
////	int32_t gyr_x = quantize(num4, FRAC_BASE);
////	int32_t gyr_y = quantize(num5, FRAC_BASE);
////	int32_t gyr_z = quantize(num6, FRAC_BASE);
////	int32_t acc_x = quantize(num7, FRAC_BASE);
////	int32_t acc_y = quantize(num8, FRAC_BASE);
////	int32_t acc_z = quantize(num9, FRAC_BASE);
////	int32_t q0    = quantize(num10, FRAC_BASE);
////	int32_t q1    = quantize(num11, FRAC_BASE);
////	int32_t q2    = quantize(num12, FRAC_BASE);
////	int32_t q3    = quantize(num13, FRAC_BASE);
//
//	float mag_x = num1;
//	float mag_y = num2;
//	float mag_z = num3;
//	float gyr_x = num4;
//	float gyr_y = num5;
//	float gyr_z = num6;
//	float acc_x = num7;
//	float acc_y = num8;
//	float acc_z = num9;
//	float q0    = num10;
//	float q1    = num11;
//	float q2    = num12;
//	float q3    = num13;
//
//
//	u_int64_t time_slots[ITERATION];
//
//	for (size_t idx = 0; idx < ITERATION; idx++)
//	{
//		timespec timer = tic();
//
//
//		MadgwickAHRSupdate(gyr_x, gyr_y, gyr_z,
//				   acc_x, acc_y, acc_z,
//				   mag_x, mag_y, mag_z,
//				   &q0, &q1, &q2, &q3);
//		time_slots[idx] = toc(&timer, "computation delay").tv_nsec;
//	}
//
//
//
//
//
//	u_int64_t average_time = 0;
//	for (size_t idx = 0; idx < ITERATION; idx++)
//	{
//		average_time += time_slots[idx];
//	}
//	average_time /= ITERATION;
//	printf("average time = %lu nm\n", average_time);
//
//
//}