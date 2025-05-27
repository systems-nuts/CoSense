#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// 复数结构体
typedef struct {
	double real;
	double imag;
} Complex;

// 复数相加
Complex complex_add(Complex a, Complex b) {
	Complex result;
	result.real = a.real + b.real;
	result.imag = a.imag + b.imag;
	return result;
}

// 复数相减
Complex complex_sub(Complex a, Complex b) {
	Complex result;
	result.real = a.real - b.real;
	result.imag = a.imag - b.imag;
	return result;
}

// 复数相乘
Complex complex_mul(Complex a, Complex b) {
	Complex result;
	result.real = a.real * b.real - a.imag * b.imag;
	result.imag = a.real * b.imag + a.imag * b.real;
	return result;
}

// 快速傅里叶变换
void fft(Complex *x, int n) {
	if (n <= 1) return;

	// 将输入数组分成偶数和奇数部分
	Complex *even = (Complex *)malloc(n / 2 * sizeof(Complex));
	Complex *odd = (Complex *)malloc(n / 2 * sizeof(Complex));
	for (int i = 0; i < n / 2; i++) {
		even[i] = x[i * 2];
		odd[i] = x[i * 2 + 1];
	}

	// 递归计算子部分
	fft(even, n / 2);
	fft(odd, n / 2);

	// 合并子部分结果
	for (int k = 0; k < n / 2; k++) {
		double t = -2 * M_PI * k / n;
		Complex w = { cos(t), sin(t) };
		Complex t_mul = complex_mul(w, odd[k]);
		x[k] = complex_add(even[k], t_mul);
		x[k + n / 2] = complex_sub(even[k], t_mul);
	}

	free(even);
	free(odd);
}

// 打印复数数组
void print_complex_array(Complex *x, int n) {
	for (int i = 0; i < n; i++) {
		printf("(%f, %f)\n", x[i].real, x[i].imag);
	}
}

int main() {
	// 输入序列的长度（必须是2的幂次）
	int n = 8;

	// 创建复数数组并初始化为输入信号
	Complex x[n];
	for (int i = 0; i < n; i++) {
		x[i].real = i;
		x[i].imag = 0.0;
	}

	// 打印输入信号
	printf("Input:\n");
	print_complex_array(x, n);

	// 计算FFT
	fft(x, n);

	// 打印FFT结果
	printf("Output:\n");
	print_complex_array(x, n);

	return 0;
}
