#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#define FRAC_Q 10
#define FRAC_BASE (1 << FRAC_Q)
#define BIT_WIDTH 32
#define ITERATION 1
/*
 * void MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz,
float* q0_ptr, float* q1_ptr, float* q2_ptr, float* q3_ptr);
void MadgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az,
   float* q0_ptr, float* q1_ptr, float* q2_ptr, float* q3_ptr);
 */
//#include "MadgwickAHRS.h"
extern volatile int32_t q0, q1, q2, q3;
//extern volatile float q0, q1, q2, q3;
//extern void MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz,
//			  float* q0_ptr, float* q1_ptr, float* q2_ptr, float* q3_ptr);
//extern void MadgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az,float * q0_ptr, float * q1_ptr, float * q2_ptr, float * q3_ptr);

extern void MadgwickAHRSupdate(int32_t gx, int32_t gy, int32_t gz, int32_t ax, int32_t ay, int32_t az, int32_t mx, int32_t my, int32_t mz, int32_t * q0_ptr, int32_t * q1_ptr, int32_t * q2_ptr, int32_t * q3_ptr);
extern void MadgwickAHRSupdateIMU(int32_t gx, int32_t gy, int32_t gz, int32_t ax, int32_t ay, int32_t az, int32_t * q0_ptr, int32_t * q1_ptr, int32_t * q2_ptr, int32_t * q3_ptr);

/***************************************
 * Timer functions of the test framework
 ***************************************/
typedef struct {
	int quantized1;
	int quantized2;
	int quantized3;
	int quantized4;
} QuantizedValues;

typedef struct{
	float dequantized1;
	float dequantized2;
	float dequantized3;
	float dequantized4;
} DequantizedValues;

typedef struct timespec timespec;
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

QuantizedValues quantize4(float value1, float value2, float value3, float value4, int frac_base) {
	QuantizedValues result;
	result.quantized1 = round(value1 * frac_base);
	result.quantized2 = round(value2 * frac_base);
	result.quantized3 = round(value3 * frac_base);
	result.quantized4 = round(value4 * frac_base);
	return result;
}

DequantizedValues dequantize4(int quantized1, int quantized2, int quantized3, int quantized4) {
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
	/*
	 * Time (s),Magnetic field x (µT),Magnetic field y (µT),Magnetic field z (µT),Gyroscope x (rad/s),Gyroscope y (rad/s),Gyroscope z (rad/s),Acceleration x (m/s^2),Acceleration y (m/s^2),Acceleration z (m/s^2)
0.07969094,-7.249095917,26.43893433,-37.16656494,-0.1184487343,0.001258035656,0.008988874033,-0.3570917249,0.3941296637,9.963726044
	 */

	float num1 = -7.249095917;
	float num2 = 26.43893433;
	float num3 = -37.16656494;
	float num4 = -0.1184487343;
	float num5 = 0.001258035656;
	float num6 = 0.008988874033;
	float num7 = -0.3570917249;
	float num8 = 0.3941296637;
	float num9 = 9.963726044;

	float num10 = 0.64306622;
	float num11 = 0.02828862;
	float num12 = -0.00567953;
	float num13 = -0.76526684;

	int32_t mag_x = quantize(num1, FRAC_BASE);
	int32_t mag_y = quantize(num2, FRAC_BASE);
	int32_t mag_z = quantize(num3, FRAC_BASE);
	int32_t gyr_x = quantize(num4, FRAC_BASE);
	int32_t gyr_y = quantize(num5, FRAC_BASE);
	int32_t gyr_z = quantize(num6, FRAC_BASE);
	int32_t acc_x = quantize(num7, FRAC_BASE);
	int32_t acc_y = quantize(num8, FRAC_BASE);
	int32_t acc_z = quantize(num9, FRAC_BASE);
	int32_t q0 = quantize(num10, FRAC_BASE);
	int32_t q1 = quantize(num11, FRAC_BASE);
	int32_t q2 = quantize(num12, FRAC_BASE);
	int32_t q3 = quantize(num13, FRAC_BASE);
//	float q0 = num10;
//	float q1 = num11;
//	float q2 = num12;
//	float q3 = num13;


	//result
	int num14 = 657;
	int num15 = 28;
	int num16 = -6;
	int num17 = -785;

	u_int64_t time_slots[ITERATION];

	for (size_t idx = 0; idx < ITERATION; idx++)
	{
		timespec timer = tic();


		MadgwickAHRSupdate(gyr_x, gyr_y, gyr_z,
				   acc_x, acc_y, acc_z,
				   mag_x, mag_y, mag_z,
				   &q0, &q1, &q2, &q3);
		time_slots[idx] = toc(&timer, "computation delay").tv_nsec;
	}


	//tic toc quantize4耗时
	timespec timer = tic();
	QuantizedValues quantizedValues = quantize4(num10, num11, num12, num13, FRAC_BASE);
	timespec durationQuantize = toc(&timer, "Quantize");
	printf("quantization time = %lu nm\n", durationQuantize.tv_nsec);


	//tic toc dequantize4耗时
	timer = tic();
	DequantizedValues dequantizedValues = dequantize4(num14, num15, num16, num17);
	timespec durationDequantize = toc(&timer, "Dequantize");
	printf("dequantization time = %lu nm\n", durationDequantize.tv_nsec);



	u_int64_t average_time = 0;
	for (size_t idx = 0; idx < ITERATION; idx++)
	{
		average_time += time_slots[idx];
	}
	average_time /= ITERATION;
	printf("average time = %lu nm\n", average_time);

	//计算total time, quantize+dequantize+average
	u_int64_t totalTime = durationQuantize.tv_nsec + durationDequantize.tv_nsec + average_time;
	printf("total time = %lu nm\n", totalTime);
}