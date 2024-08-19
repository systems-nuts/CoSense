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

#include "MadgwickAHRS.h"
/***************************************
 * Timer functions of the test framework
 ***************************************/

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

//extern int perform_addition(float a, float b);
extern int perform_addition(int a ,int b);
// extern int perform_subtraction(float a, float b);
//extern int perform_multiplication(int a, int b);
//extern int perform_multiplication(float a, float b);
//extern void MadgwickAHRSupdate(int32_t gx, int32_t gy, int32_t gz, int32_t ax, int32_t ay, int32_t az, int32_t mx, int32_t my, int32_t mz, int32_t * q0_ptr, int32_t * q1_ptr, int32_t * q2_ptr, int32_t * q3_ptr);
//extern void MadgwickAHRSupdateIMU(int32_t gx, int32_t gy, int32_t gz, int32_t ax, int32_t ay, int32_t az, int32_t * q0_ptr, int32_t * q1_ptr, int32_t * q2_ptr, int32_t * q3_ptr);

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
	int num1 = 10;
	int num2 = 3;
	//extern int perform_multiplication(int a, int b);
	//int num3 = perform_multiplication(num1, num2);
	int num4 = perform_addition(num1, num2);
	//printf("num3: %d\n", num3);
	printf("num4: %d\n", num4);




	//	float num1 = 8.10;
	//	printf("num1: %f\n", num1);
	//	float num2 = 2.25;
	//	printf("num2: %f\n", num2);
	//	printf("FRAC_BASE: %d\n", FRAC_BASE);
	//
	//	int a = 10;
	//	int b = 3;
	//
	//	float num3 = 0;
	//	float num4 = 0;
	//	float num5 = 0;
	//	float num6 = 0;
	//
	//	int quantized_num1 = quantize(num1, FRAC_BASE);
	//	int quantized_num2 = quantize(num2, FRAC_BASE);
	//
	//	int addition_result = perform_addition(num1, num2);
	//	float actual_addition_result = dequantize(addition_result);
	//
	//	printf("Addition Result after quantization: %d\n", addition_result);
	//	printf("expected Addition Result after dequantization: %f\n",dequantize(quantized_num1 + quantized_num2));
	//	printf("Actual Addition Result after dequantization: %f\n", actual_addition_result);
	//
	//
	//	// Perform original operations to compare results later
	//	float original_addition = num1 + num2;
	//	float original_subtraction = num1 - num2;
	//	float original_multiplication = num1 * num2;
	//	float original_division = num1 / num2;
	//	printf("Original Addition: %f\n", original_addition);
	//	printf("Original Subtraction: %f\n", original_subtraction);
	//
	//	int subtraction_result = perform_subtraction(num1, num2);
	//	float actual_subtraction_result = dequantize(subtraction_result);
	//	printf("Subtraction Result after quantization: %d\n", subtraction_result);
	//	printf("Actual Subtraction Result after dequantization: %f\n", actual_subtraction_result);

	// Quantization of input numbers

	// int subtraction_result = perform_subtraction(a, b);
	// printf("Subtraction Result: %d\n", subtraction_result);

	//	// Perform operations on the quantized numbers
	//	float addition = perform_addition(num1, num2);
	//
	//	float subtraction = perform_subtraction(num1, num2);
	//
	//	float multiplication = perform_multiplication(num1, num2);
	//
	//	float division = perform_division(num1, num2);
	//
	//
	//	// Dequantization of actual results
	//	float my_dequantized_addition = dequantize(addition, FRAC_BASE);
	//	float my_dequantized_subtraction = dequantize(subtraction, FRAC_BASE);
	//	float my_dequantized_multiplication = dequantize( multiplication, FRAC_BASE * FRAC_BASE);
	//	float my_dequantized_division = division;
	//
	//
	//	num3 = (my_dequantized_addition + my_dequantized_subtraction) / 2;
	//	num4 = (my_dequantized_addition -my_dequantized_subtraction) / 2;
	//	printf("num3: %f\n", num3);
	//	printf("num4: %f\n", num4);
	//
	//
	//	num5 = sqrt(my_dequantized_multiplication*my_dequantized_division);
	//	printf("xdeduced : %f\n", num5);
	//	num6 = sqrt(my_dequantized_multiplication/my_dequantized_division);
	//	printf("num6: %f\n", num6);
	//
	//
	//
	//

	//	printf("Expected after Dequantized Addition: %f\n", dequantize(quantized_num1 + quantized_num2, FRAC_BASE));
	//	printf("Actual after Dequantized Addition: %f\n", my_dequantized_addition);
	//
	//	printf("Original Subtraction: %f\n", original_subtraction);
	//	printf("Expected after Dequantized Subtraction: %f\n", dequantize(quantized_num1 - quantized_num2, FRAC_BASE));
	//	printf("Actual after Dequantized Subtraction: %f\n", my_dequantized_subtraction);
	//
	//	printf("Original Multiplication: %f\n", original_multiplication);
	//
	//	printf("Expected after Dequantized Multiplication: %f\n", dequantize((int)((long long)quantized_num1 * quantized_num2), FRAC_BASE * FRAC_BASE));
	//	//printf("Expected after Dequantized Multiplication: %f\n", dequantize((quantized_num1 * quantized_num2), (FRAC_BASE * FRAC_BASE)));
	//	printf("Actual after Dequantized Multiplication: %f\n", my_dequantized_multiplication);
	//
	//	printf("Original Division: %f\n", original_division);
	//	printf("Expected after Dequantized Division: %f\n", (float)quantized_num1 / quantized_num2 );
	//	printf("Actual after Dequantized Division: %f\n", my_dequantized_division);

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
//	int32_t mag_x = quantize(num1, FRAC_BASE);
//	int32_t mag_y = quantize(num2, FRAC_BASE);
//	int32_t mag_z = quantize(num3, FRAC_BASE);
//	int32_t gyr_x = quantize(num4, FRAC_BASE);
//	int32_t gyr_y = quantize(num5, FRAC_BASE);
//	int32_t gyr_z = quantize(num6, FRAC_BASE);
//	int32_t acc_x = quantize(num7, FRAC_BASE);
//	int32_t acc_y = quantize(num8, FRAC_BASE);
//	int32_t acc_z = quantize(num9, FRAC_BASE);
//	int32_t q0    = quantize(num10, FRAC_BASE);
//	int32_t q1    = quantize(num11, FRAC_BASE);
//	int32_t q2    = quantize(num12, FRAC_BASE);
//	int32_t q3    = quantize(num13, FRAC_BASE);
//
//	u_int64_t time_slots[ITERATION];
//
//	for (size_t idx = 0; idx < ITERATION; idx++)
//	{
//		timespec timer = tic();
//		// for (size_t ts = 0; ts < DATA_SIZE; ts++)
//		//{
//		//		MadgwickAHRSupdate(gyr_x[ts], gyr_y[ts], gyr_z[ts],
//		//				   acc_x[ts], acc_y[ts], acc_z[ts],
//		//				   mag_x[ts], mag_y[ts], mag_z[ts],
//		//				   &q0[ts], &q1[ts], &q2[ts], &q3[ts]);
//		//}
//
//		MadgwickAHRSupdate(gyr_x, gyr_y, gyr_z,
//				   acc_x, acc_y, acc_z,
//				   mag_x, mag_y, mag_z,
//				   &q0, &q1, &q2, &q3);
//		time_slots[idx] = toc(&timer, "computation delay").tv_nsec;
//	}
//
//	u_int64_t average_time = 0;
//	for (size_t idx = 0; idx < ITERATION; idx++)
//	{
//		average_time += time_slots[idx];
//	}
//	average_time /= ITERATION;
//	printf("average time = %lu nm\n", average_time);

	return 0;
}