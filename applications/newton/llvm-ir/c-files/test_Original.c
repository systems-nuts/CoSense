#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#define FRAC_Q 10
#define BIT_WIDTH 32
#define ITERATION 100000

extern volatile float q0, q1, q2, q3;
extern void MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz,
					 float* q0_ptr, float* q1_ptr, float* q2_ptr, float* q3_ptr);
extern void MadgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az,
					    float* q0_ptr, float* q1_ptr, float* q2_ptr, float* q3_ptr);

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

	float mag_x = -7.249095917;
	float mag_y = 26.43893433;
	float mag_z = -37.16656494;
	float gyr_x = -0.1184487343;
	float gyr_y = 0.001258035656;
	float gyr_z = 0.008988874033;
	float acc_x = -0.3570917249;
	float acc_y = 0.3941296637;
	float acc_z = 9.963726044;
	float q0 = 0.64306622;
	float q1 = 0.02828862;
	float q2 = -0.00567953;
	float q3 = -0.76526684;



	u_int64_t time_slots[ITERATION];

	for (size_t idx = 0; idx < ITERATION; idx++)
	{
		timespec timer = tic();
//		 for (size_t ts = 0; ts < DATA_SIZE; ts++)
//		{
//				MadgwickAHRSupdate(gyr_x[ts], gyr_y[ts], gyr_z[ts],
//						   acc_x[ts], acc_y[ts], acc_z[ts],
//						   mag_x[ts], mag_y[ts], mag_z[ts],
//						   &q0[ts], &q1[ts], &q2[ts], &q3[ts]);
//		}

		MadgwickAHRSupdate(gyr_x, gyr_y, gyr_z,
				   acc_x, acc_y, acc_z,
				   mag_x, mag_y, mag_z,
				   &q0, &q1, &q2, &q3);
		time_slots[idx] = toc(&timer, "computation delay").tv_nsec;
	}

	u_int64_t average_time = 0;
	for (size_t idx = 0; idx < ITERATION; idx++)
	{
		average_time += time_slots[idx];
	}
	average_time /= ITERATION;
	printf("average time = %lu nm\n", average_time);
}