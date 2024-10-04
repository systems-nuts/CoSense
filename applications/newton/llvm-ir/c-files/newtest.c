#include <stdio.h>
#include "MadgwickAHRS.h"

#define FRAC_BASE 1024
//volatile float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;
extern volatile float	q0, q1, q2, q3;
// 测试函数
void testMadgwickUpdate() {
	// 初始化四元数
	q0 = 1.0f;
	q1 = 0.0f;
	q2 = 0.0f;
	q3 = 0.0f;

//	q0 = 0.64306622f;
//	q1= 0.02828862f;
//	q2 = -0.00567953f;
//	q3 = -0.76526684f;

	// 模拟的传感器数据 (这些值可以根据实际情况调整)
	// 使用提取的数据替换模拟传感器数据
	float gx = -0.1184487343f * 61; // 陀螺仪 x 轴角速度 (乘以 61)
	float gy = 0.001258035656f * 61; // 陀螺仪 y 轴角速度 (乘以 61)
	float gz = 0.008988874033f * 61; // 陀螺仪 z 轴角速度 (乘以 61)

	float ax = -0.3570917249f * 2; // 加速度计 x 轴加速度 (乘以 2)
	float ay = 0.3941296637f * 2; // 加速度计 y 轴加速度 (乘以 2)
	float az = 9.963726044f * 2; // 加速度计 z 轴加速度 (乘以 2)


	float mx = -7.249095917f; // 磁力计 x 轴磁场强度 (单位：µT)
	float my = 26.43893433f; // 磁力计 y 轴磁场强度
	float mz = -37.16656494f; // 磁力计 z 轴磁场强度


	// 调用 MadgwickAHRSupdate 函数
	MadgwickAHRSupdate(gx, gy, gz, ax, ay, az, mx, my, mz);

	// 打印四元数结果
	q0= (double)q0/FRAC_BASE;
	q1= (double)q1/FRAC_BASE;
	q2= (double)q2/FRAC_BASE;
	q3= (double)q3/FRAC_BASE;


	printf("q0 = %f\n", q0);
	printf("q1 = %f\n", q1);
	printf("q2 = %f\n", q2);
	printf("q3 = %f\n", q3);
}

int main() {
	// 调用测试函数
	testMadgwickUpdate();
	return 0;
}
