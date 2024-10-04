#include <stdio.h>
#include "Kalman.h"

int main() {
	// 初始化Kalman滤波器实例
	Kalman kalman;
	kalman.setAngle(0.0f); // 设置初始角度为0度

	// 硬编码的数据集，模拟传感器输入
	// newAngle: 角度测量值, newRate: 角速度测量值, dt: 时间间隔
	float newAngle[] = {0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 2.5, 2.0, 1.5, 1.0};
	float newRate[] = {0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.25, 0.2, 0.15, 0.1};
	float dt = 0.1f; // 时间间隔 100毫秒

	int dataSize = sizeof(newAngle) / sizeof(newAngle[0]);

	// 打印表头
	printf("Time(s)\tMeasured Angle\tMeasured Rate\tFiltered Angle\n");

	// 使用硬编码数据测试Kalman滤波器
	for (int i = 0; i < dataSize; i++) {
		float filteredAngle = kalman.getAngle(newAngle[i], newRate[i], dt);
		printf("%0.1f\t%f\t%f\t%f\n", i * dt, newAngle[i], newRate[i], filteredAngle);
	}

	return 0;
}
