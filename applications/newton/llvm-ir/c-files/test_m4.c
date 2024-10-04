/*
 * Madgwick test case (run locally)
 * Compilation command in applications/newton/llvm-ir/performance_test/Makefile
 *
 * How to compile and run?
 * 1. `make perf_madgwick` FP hardware (by default)
 * 2. `SOFT_FLOAT=1 make perf_madgwick` FP software (clang -msoft-float and opt --float-abi=soft)
 * 3. `SOFT_FLOAT_LIB=1 make perf_madgwick` FP soft-float lib (from CHStone)
 * 4. `AUTO_QUANT=1 make perf_madgwick` INT fixed Q number format
 * */
#include "../Include/stm32f3xx.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#if defined(INT_DATA_TYPE)
extern volatile int32_t q0, q1, q2, q3;
#elif defined(FP_DATA_TYPE)
extern volatile float q0, q1, q2, q3;
#if defined(SOFT_FLOAT_LIB)
#include "MadgwickAHRS_softfloat.h"
#else
#include "MadgwickAHRS.h"
#endif

#else
#error "Must set data type: FP or INT"
#endif

#define DATA_SIZE 10
#define FRAC_Q 10
#define FRAC_BASE (1 << FRAC_Q)
#define ITERATION 10

//#define SYSTICK_FREQUENCY_HZ 1000  // 假设 SysTick 配置为 1ms (1000Hz)

static volatile uint32_t system_ticks = 0;  // 用于记录系统启动后的总滴答数
// 声明并定义全局变量 elapsed_time_tbl，用于存储计时信息

// 定义 ARM_CM_DWT_CYCCNT 宏
#define ARM_CM_DWT_CYCCNT (DWT->CYCCNT)

/***************************************
 * Timer functions of the test framework
 ***************************************/

typedef struct {
	uint32_t start;
	uint32_t current;
	uint32_t max;
	uint32_t min;
} ELAPSED_TIME;

ELAPSED_TIME elapsed_time_tbl[ITERATION];

void
elapsed_time_start(uint32_t i)
{
	elapsed_time_tbl[i].start = ARM_CM_DWT_CYCCNT;
}

void
elapsed_time_stop(uint32_t i)
{
	uint32_t       stop  = ARM_CM_DWT_CYCCNT;
	ELAPSED_TIME * p_tbl = &elapsed_time_tbl[i];
	p_tbl->current	     = stop - p_tbl->start;
	if (p_tbl->max < p_tbl->current)
	{
		p_tbl->max = p_tbl->current;
	}
	if (p_tbl->min > p_tbl->current)
	{
		p_tbl->min = p_tbl->current;
	}
}

// 启动 DWT 计数器
void
init_dwt_counter(void)
{
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;	 // Enable DWT
	DWT->CYCCNT = 0;				 // Clear DWT cycle counter
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;		 // Enable DWT cycle counter
}

// SysTick 中断处理程序，用于增加系统滴答计数
//void
//SysTick_Handler(void)
//{
//	system_ticks++;
//}
//
//typedef struct {
//	uint32_t tv_sec;   // 秒
//	uint32_t tv_nsec;  // 纳秒
//} timespec;
//
//void
//init_systick(void)
//{
//	// 配置 SysTick 以产生 1ms 的中断
//	if (SysTick_Config(SystemCoreClock / SYSTICK_FREQUENCY_HZ))
//	{
//		// 如果 SysTick 配置失败，处理错误
//		while (1)
//			;
//	}
//}
//
//timespec
//get_current_time(void)
//{
//	timespec current_time;
//	uint32_t ticks	     = system_ticks;
//	uint32_t systick_val = SysTick->VAL;
//
//	current_time.tv_sec  = ticks / SYSTICK_FREQUENCY_HZ;
//	current_time.tv_nsec = (1000000 - (systick_val * 1000 / (SystemCoreClock / SYSTICK_FREQUENCY_HZ))) * 1000;
//
//	return current_time;
//}
//
//timespec
//diff(timespec start, timespec end)
//{
//	timespec temp;
//	if ((end.tv_nsec - start.tv_nsec) < 0)
//	{
//		temp.tv_sec  = end.tv_sec - start.tv_sec - 1;
//		temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
//	}
//	else
//	{
//		temp.tv_sec  = end.tv_sec - start.tv_sec;
//		temp.tv_nsec = end.tv_nsec - start.tv_nsec;
//	}
//	return temp;
//}
//
//void
//printTimeSpec(timespec t, const char * prefix)
//{
//	printf("%s: %d.%09d\n", prefix, (int)t.tv_sec, (int)t.tv_nsec);
//}
//
//timespec
//tic(void)
//{
//	return get_current_time();
//}
//
//timespec
//toc(timespec * start_time, const char * prefix)
//{
//	timespec current_time = get_current_time();
//	timespec time_consump = diff(*start_time, current_time);
//	printTimeSpec(time_consump, prefix);
//	*start_time = current_time;
//	return time_consump;
//}

int
main()
{
#if defined(INT_DATA_TYPE)
	int32_t q0[DATA_SIZE], q1[DATA_SIZE], q2[DATA_SIZE], q3[DATA_SIZE];
	// quaternion of sensor frame relative to auxiliary frame
	for (int i = 0; i < DATA_SIZE; i++)
	{
		q0[i] = (0.64306622f * FRAC_BASE);
		q1[i] = (0.02828862f * FRAC_BASE);
		q2[i] = (-0.00567953f * FRAC_BASE);
		q3[i] = (-0.76526684f * FRAC_BASE);
	}

#elif defined(FP_DATA_TYPE)
	float q0[DATA_SIZE], q1[DATA_SIZE], q2[DATA_SIZE], q3[DATA_SIZE];
	// quaternion of sensor frame relative to auxiliary frame
	for (int i = 0; i < DATA_SIZE; i++)
	{
		q0[i] = 0.64306622f;
		q1[i] = 0.02828862f;
		q2[i] = -0.00567953f;
		q3[i] = -0.76526684f;
	}

#else
#error "Must set data type: FP or INT"
#endif

#if defined(FP_DATA_TYPE)
	double time[DATA_SIZE] = {
	    0.07969094, 0.08969094, 0.09969094, 0.10969094, 0.11969094,
	    0.12969094, 0.13969094, 0.14969094, 0.15969094, 0.16969094};

	float mag_x[DATA_SIZE] = {
	    -7.249095917, -6.654678345, -7.249095917, -7.548351288, -7.311084747,
	    -5.958099365, -8.100597382, -6.394374847, -7.610450745, -6.729293823};

	float mag_y[DATA_SIZE] = {
	    26.43893433, 26.09465027, 26.43893433, 24.87819672, 23.94067001,
	    24.8057251, 25.37778282, 25.43608284, 26.11058998, 25.38845825};

	float mag_z[DATA_SIZE] = {
	    -37.16656494, -37.29600525, -37.16656494, -39.49668884, -40.19360352,
	    -37.20587158, -37.85224915, -37.70759583, -38.66853333, -38.77174377};

	float gyr_x[DATA_SIZE] = {
	    -0.1184487343, -0.1184487343, -0.1184487343, -0.1184487343, -0.1184487343,
	    -0.1184487343, -0.1184487343, -0.1151283234, -0.08246348053, -0.03473320603};

	float gyr_y[DATA_SIZE] = {
	    0.001258035656, 0.001258035656, 0.001258035656, 0.001258035656, 0.001258035656,
	    0.001258035656, 0.001258035656, -0.00251355581, 0.00447106408, 0.04428362101};

	float gyr_z[DATA_SIZE] = {
	    0.008988874033, 0.008988874033, 0.008988874033, 0.008988874033, 0.008988874033,
	    0.008988874033, 0.008988874033, 0.01010152325, 0.004323757719, 0.0009580431506};

	float acc_x[DATA_SIZE] = {
	    -0.3570917249, -0.3570917249, -0.3570917249, -0.3570917249, -0.3570917249,
	    -0.3570917249, -0.3570917249, -0.2793728709, -0.2367789149, -0.2995947599};

	float acc_y[DATA_SIZE] = {
	    0.3941296637, 0.3941296637, 0.3941296637, 0.3941296637, 0.3941296637,
	    0.3941296637, 0.3941296637, 0.4129949808, 0.4112356901, 0.416141957};

	float acc_z[DATA_SIZE] = {
	    9.963726044, 9.963726044, 9.963726044, 9.963726044, 9.963726044,
	    9.963726044, 9.963726044, 10.27848434, 10.43229961, 10.24356842};

#elif defined(INT_DATA_TYPE)
	int32_t time[DATA_SIZE] = {
	    round(0.07969094 * FRAC_BASE), round(0.08969094 * FRAC_BASE),
	    round(0.09969094 * FRAC_BASE), round(0.10969094 * FRAC_BASE),
	    round(0.11969094 * FRAC_BASE), round(0.12969094 * FRAC_BASE),
	    round(0.13969094 * FRAC_BASE), round(0.14969094 * FRAC_BASE),
	    round(0.15969094 * FRAC_BASE), round(0.16969094 * FRAC_BASE)};

	int32_t mag_x[DATA_SIZE] = {
	    round(-7.249095917 * FRAC_BASE), round(-6.654678345 * FRAC_BASE),
	    round(-7.249095917 * FRAC_BASE), round(-7.548351288 * FRAC_BASE),
	    round(-7.311084747 * FRAC_BASE), round(-5.958099365 * FRAC_BASE),
	    round(-8.100597382 * FRAC_BASE), round(-6.394374847 * FRAC_BASE),
	    round(-7.610450745 * FRAC_BASE), round(-6.729293823 * FRAC_BASE)};

	int32_t mag_y[DATA_SIZE] = {
	    round(26.43893433 * FRAC_BASE), round(26.09465027 * FRAC_BASE),
	    round(26.43893433 * FRAC_BASE), round(24.87819672 * FRAC_BASE),
	    round(23.94067001 * FRAC_BASE), round(24.8057251 * FRAC_BASE),
	    round(25.37778282 * FRAC_BASE), round(25.43608284 * FRAC_BASE),
	    round(26.11058998 * FRAC_BASE), round(25.38845825 * FRAC_BASE)};

	int32_t mag_z[DATA_SIZE] = {
	    round(-37.16656494 * FRAC_BASE), round(-37.29600525 * FRAC_BASE),
	    round(-37.16656494 * FRAC_BASE), round(-39.49668884 * FRAC_BASE),
	    round(-40.19360352 * FRAC_BASE), round(-37.20587158 * FRAC_BASE),
	    round(-37.85224915 * FRAC_BASE), round(-37.70759583 * FRAC_BASE),
	    round(-38.66853333 * FRAC_BASE), round(-38.77174377 * FRAC_BASE)};

	int32_t gyr_x[DATA_SIZE] = {
	    round(-0.1184487343 * FRAC_BASE) * 61, round(-0.1184487343 * FRAC_BASE) * 61,
	    round(-0.1184487343 * FRAC_BASE) * 61, round(-0.1184487343 * FRAC_BASE) * 61,
	    round(-0.1184487343 * FRAC_BASE) * 61, round(-0.1184487343 * FRAC_BASE) * 61,
	    round(-0.1184487343 * FRAC_BASE) * 61, round(-0.1151283234 * FRAC_BASE) * 61,
	    round(-0.08246348053 * FRAC_BASE) * 61, round(-0.03473320603 * FRAC_BASE) * 61};

	int32_t gyr_y[DATA_SIZE] = {
	    round(0.001258035656 * FRAC_BASE) * 61, round(0.001258035656 * FRAC_BASE) * 61,
	    round(0.001258035656 * FRAC_BASE) * 61, round(0.001258035656 * FRAC_BASE) * 61,
	    round(0.001258035656 * FRAC_BASE) * 61, round(0.001258035656 * FRAC_BASE) * 61,
	    round(0.001258035656 * FRAC_BASE) * 61, round(-0.00251355581 * FRAC_BASE) * 61,
	    round(0.00447106408 * FRAC_BASE) * 61, round(0.04428362101 * FRAC_BASE) * 61};

	int32_t gyr_z[DATA_SIZE] = {
	    round(0.008988874033 * FRAC_BASE) * 61, round(0.008988874033 * FRAC_BASE) * 61,
	    round(0.008988874033 * FRAC_BASE) * 61, round(0.008988874033 * FRAC_BASE) * 61,
	    round(0.008988874033 * FRAC_BASE) * 61, round(0.008988874033 * FRAC_BASE) * 61,
	    round(0.008988874033 * FRAC_BASE) * 61, round(0.01010152325 * FRAC_BASE) * 61,
	    round(0.004323757719 * FRAC_BASE) * 61, round(0.0009580431506 * FRAC_BASE) * 61};

	int32_t acc_x[DATA_SIZE] = {
	    round(-0.3570917249 * FRAC_BASE) * 2, round(-0.3570917249 * FRAC_BASE) * 2,
	    round(-0.3570917249 * FRAC_BASE) * 2, round(-0.3570917249 * FRAC_BASE) * 2,
	    round(-0.3570917249 * FRAC_BASE) * 2, round(-0.3570917249 * FRAC_BASE) * 2,
	    round(-0.3570917249 * FRAC_BASE) * 2, round(-0.2793728709 * FRAC_BASE) * 2,
	    round(-0.2367789149 * FRAC_BASE) * 2, round(-0.2995947599 * FRAC_BASE) * 2};

	int32_t acc_y[DATA_SIZE] = {
	    round(0.3941296637 * FRAC_BASE) * 2, round(0.3941296637 * FRAC_BASE) * 2,
	    round(0.3941296637 * FRAC_BASE) * 2, round(0.3941296637 * FRAC_BASE) * 2,
	    round(0.3941296637 * FRAC_BASE) * 2, round(0.3941296637 * FRAC_BASE) * 2,
	    round(0.3941296637 * FRAC_BASE) * 2, round(0.4129949808 * FRAC_BASE) * 2,
	    round(0.4112356901 * FRAC_BASE) * 2, round(0.416141957 * FRAC_BASE) * 2};

	int32_t acc_z[DATA_SIZE] = {
	    round(9.963726044 * FRAC_BASE) * 2, round(9.963726044 * FRAC_BASE) * 2,
	    round(9.963726044 * FRAC_BASE) * 2, round(9.963726044 * FRAC_BASE) * 2,
	    round(9.963726044 * FRAC_BASE) * 2, round(9.963726044 * FRAC_BASE) * 2,
	    round(9.963726044 * FRAC_BASE) * 2, round(10.27848434 * FRAC_BASE) * 2,
	    round(10.43229961 * FRAC_BASE) * 2, round(10.24356842 * FRAC_BASE) * 2};
#endif

	init_dwt_counter();  // 初始化 DWT 计数器

	u_int64_t time_slots[ITERATION];

	for (size_t idx = 0; idx < ITERATION; idx++)
	{
		elapsed_time_start(idx);  // 开始计时
		// timespec timer = tic();
		for (size_t ts = 0; ts < DATA_SIZE; ts++)
		{
			MadgwickAHRSupdate(gyr_x[ts], gyr_y[ts], gyr_z[ts],
					   acc_x[ts], acc_y[ts], acc_z[ts],
					   mag_x[ts], mag_y[ts], mag_z[ts],
					   &q0[ts], &q1[ts], &q2[ts], &q3[ts]);
		}

		//time_slots[idx] = toc(&timer, "computation delay").tv_nsec;

		elapsed_time_stop(idx); // 停止计时
		time_slots[idx] = elapsed_time_tbl[idx].current;
		printf("Elapsed cycles for iteration %lu: %lu cycles\n", idx, time_slots[idx]);
	}

	u_int64_t average_time = 0;
	for (size_t idx = 0; idx < ITERATION; idx++)
	{
		average_time += time_slots[idx];
	}
	average_time /= ITERATION;
	printf("average time = %lu nm\n", average_time);

	return 0;
}
