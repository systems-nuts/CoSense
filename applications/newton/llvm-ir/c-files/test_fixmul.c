#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define DATA_SIZE 1000
#define ITERATIONS 10000
#define FRAC_Q 10
#define FRAC_BASE (1 << FRAC_Q)

/***************************************
* Timer functions of the test framework
***************************************/
typedef struct timespec timespec;

timespec diff(timespec start, timespec end) {
	timespec temp;
	if ((end.tv_nsec - start.tv_nsec) < 0) {
		temp.tv_sec = end.tv_sec - start.tv_sec - 1;
		temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec - start.tv_sec;
		temp.tv_nsec = end.tv_nsec - start.tv_nsec;
	}
	return temp;
}

void printTimeSpec(timespec t, const char* prefix) {
	printf("%s: %ld.%09ld seconds\n", prefix, t.tv_sec, t.tv_nsec);
}

timespec sum(timespec t1, timespec t2) {
	timespec temp;
	if (t1.tv_nsec + t2.tv_nsec >= 1000000000) {
		temp.tv_sec = t1.tv_sec + t2.tv_sec + 1;
		temp.tv_nsec = t1.tv_nsec + t2.tv_nsec - 1000000000;
	} else {
		temp.tv_sec = t1.tv_sec + t2.tv_sec;
		temp.tv_nsec = t1.tv_nsec + t2.tv_nsec;
	}
	return temp;
}

/***************************************
* Multiplication functions
***************************************/
int32_t fixmul(int32_t x, int32_t y) {
	return ((int64_t)x * y) >> FRAC_Q;
}

float fpmul(float x, float y) {
	return x * y;
}

/***************************************
* Main function to test performance
***************************************/
int main() {
	srand(time(NULL));

	int32_t int_a, int_b;
	float fp_a, fp_b;

	timespec start_time, end_time, total_time_int, total_time_fp;
	total_time_int.tv_sec = total_time_int.tv_nsec = 0;
	total_time_fp.tv_sec = total_time_fp.tv_nsec = 0;

	for (int i = 0; i < ITERATIONS; i++) {
		int_a = rand() % FRAC_BASE;
		int_b = rand() % FRAC_BASE;
		fp_a = (float)int_a / FRAC_BASE;
		fp_b = (float)int_b / FRAC_BASE;

		// Test fixed-point multiplication
		clock_gettime(CLOCK_MONOTONIC, &start_time);
		int32_t result_int = fixmul(int_a, int_b);
		clock_gettime(CLOCK_MONOTONIC, &end_time);
		total_time_int = sum(total_time_int, diff(start_time, end_time));

		// Test floating-point multiplication
		clock_gettime(CLOCK_MONOTONIC, &start_time);
		float result_fp = fpmul(fp_a, fp_b);
		clock_gettime(CLOCK_MONOTONIC, &end_time);
		total_time_fp = sum(total_time_fp, diff(start_time, end_time));
	}

	printTimeSpec(total_time_int, "Total time for fixed-point multiplication");
	printTimeSpec(total_time_fp, "Total time for floating-point multiplication");

	return 0;
}
