#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define FRAC_Q 10  // 假设定点小数使用16位

int32_t fixmul(int32_t x, int32_t y) {
	return ((int64_t)x * y) >> FRAC_Q;
}

int main() {
	int32_t a = 32767;
	int32_t b = 32767;
	for (long i = 0; i < 100000000; i++) {
		fixmul(a, b);
	}
	return 0;
}
