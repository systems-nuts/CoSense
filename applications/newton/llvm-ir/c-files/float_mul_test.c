#include <stdio.h>
#include <stdint.h>
#include <time.h>

float float_mul(float x, float y) {
	return x * y;
}

int main() {
	float a = 32767.0f;  // 浮点测试也使用相同的数值范围
	float b = 32767.0f;
	for (long i = 0; i < 100000000; i++) {
		float_mul(a, b);
	}
	return 0;
}
