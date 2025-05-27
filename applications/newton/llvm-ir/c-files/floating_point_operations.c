
#include <stdio.h>
#include "floating_point_operations.h"
#include <math.h>
#include <complex.h>
#include <stdint.h>

//  Function to perform basic floating point operations
//  Function to perform addition
//  float perform_addition(float a, float b) {
//
//	return  a + b;
// }
//
//
//
////// Function to perform subtraction
// float perform_subtraction(float a, float b) {
//	return a - b;
// }
//
// float perform_constant_subtraction(float a) {
//	return 1.5f - a;
// }
//
// float perform_constant_subtraction2(float a) {
//	 return 0.5 - a;
// }

////////
////////// Function to perform multiplication
// float perform_multiplication(float a, float b) {
//	return a * b;
// }
////
// float perform_division(float a, float b) {
//	 return a / b;
// }c

// float
// perform_constant_multiplication(float a)
//{
//	return 0.5*a;
// }
////
// float
// perform_constant_multiplication2(float a)
// {
//	 return 0.51*a;
// }
//
// float
// perform_constant_multiplication3(float a)
// {
//	 return 2.0f*a;
// }

// float
// perform_constant_multiplication4(float a)
// {
//	 return a*a;
// }
//
//
//
// float
// perform_constant_multiplication3(float a)
// {
//	 return 2*a;
// }

// float
// perform_constant_multiplication(float a)
// {
//	 return 0.5 *a;
// }

// 2x2矩阵乘法
// void multiply_matrices(float A[2][2], float B[2][2], float C[2][2]) {
//	 // 简化的矩阵乘法实现，不使用循环
//	 C[0][0] = A[0][0] * B[0][0] + A[0][1] * B[1][0];
//	 C[0][1] = A[0][0] * B[0][1] + A[0][1] * B[1][1];
//	 C[1][0] = A[1][0] * B[0][0] + A[1][1] * B[1][0];
//	 C[1][1] = A[1][0] * B[0][1] + A[1][1] * B[1][1];
// }

// 2x2矩阵加法
// void add_matrices() {
//	 // 直接对应元素相加
//	 float A[2][2] = {{1, 2}, {3, 4}};
//	 float B[2][2] = {{5, 6}, {7, 8}};
//	 float C[2][2]; // 结果矩阵
//
//	 //矩阵加法
//	 C[0][0] = A[0][0] + B[0][0];
//	 C[0][1] = A[0][1] + B[0][1];
//	 C[1][0] = A[1][0] + B[1][0];
//	 C[1][1] = A[1][1] + B[1][1];
// }

//  float
//  perform_constant_division(float a)
//{
//	return a / 0.21;
// }

// float compute_sin(float angle) {
//	 return sinf(angle);
// }

// float compute_cos(float angle) {
//	 return cosf(angle);
// }

// float compute_sqrt(float number) {
//	 return sqrtf(number);
// }

// double compute_pow_double( double base, double exponent) {
//      return powf(base, exponent);
//  }

// float compute_pow(float base, float exponent) {
//	 return powf(base, exponent);
//  }

// double compute_sqrt_double(double number) {
//	double  number2 = number +2.14;
//	return sqrt(number2);
//
//  }

// 交换两个指针指向的浮点数值
// void swap_float_values(float *a, float *b) {
//	float temp = *a;
//	*a = *b;
//	*b = temp;
//}

//
// float
// perform_operations(float a, float b, float c, float d)
//{
//	float result = a * b;	       // First multiplication
//	result	     = result + c;     // Addition
//	result	     = result - d;     // Subtraction
//	result	     = result * 1.5f;  // Second multiplication with a constant
//	return result;
// }

// float computePolynomial(float x) {
//	float result = 0.0;
//	for (int i = 0; i < 1000; ++i) {
//		result += 0.3 * x * x * x + 0.2 * x * x + 0.5 * x + 0.7;
//	}
//	return result;
// }
// #define PI 3.14159265358979323846

// void fft(float* real, float* imag, int n) {
//	int i, j, k, m, m_max;
//	float t_real, t_imag, u_real, u_imag, w_real, w_imag, theta;
//
//	// Bit-reverse
//	for (i = 0, j = 0; i < n; ++i) {
//		if (j > i) {
//			t_real = real[j];
//			real[j] = real[i];
//			real[i] = t_real;
//			t_imag = imag[j];
//			imag[j] = imag[i];
//			imag[i] = t_imag;
//		}
//		for (k = n >> 1; (j ^= k) < k; k >>= 1);
//	}
//
//	// FFT
//	for (m = 1; (m_max = m << 1) <= n; m = m_max) {
//		theta = -2.0 * PI / m_max;
//		w_real = 1.0;
//		w_imag = 0.0;
//		u_real = cos(theta);
//		u_imag = sin(theta);
//
//		for (k = 0; k < m; ++k) {
//			for (i = k; i < n; i += m_max) {
//				j = i + m;
//				t_real = w_real * real[j] - w_imag * imag[j];
//				t_imag = w_real * imag[j] + w_imag * real[j];
//				real[j] = real[i] - t_real;
//				imag[j] = imag[i] - t_imag;
//				real[i] += t_real;
//				imag[i] += t_imag;
//			}
//			t_real = w_real;
//			w_real = t_real * u_real - w_imag * u_imag;
//			w_imag = t_real * u_imag + w_imag * u_real;
//		}
//	}
// }

// double polynomial(double x) {
//	// Evaluate a polynomial: P(x) = 3x^4 - 5x^3 + 2x^2 - x + 7
//	double result = 5.51*x*x*x*x + 0.21*x*x*x + 0.3*x*x + x;
////	return result;
//	//double result = x*x*x*x;
//	return result;
//}

// double
// polynomial(double x)
//{
//	// Evaluate a more complex polynomial
//	double result = 0.0;
//
//	// High-order terms of the polynomial
//	for (int i = 1; i <= 10; i++)
//	{
//		result += 8.51 * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x;
//		result += 7.51 * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x;
//		result += pow(x, 16) * i;
//		result += 8.51 * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x;
//		result += 7.51 * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x;
//		result += 8.51 * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x;
//	}
//
//	// Nested operations to increase complexity
//	for (int i = 1; i <= 10; i++)
//	{
//		for (int j = 1; j <= 100; j++)
//		{
//			result += 8.51 * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x;
//			result += 7.51 * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x * x;
//			result += pow(x, 4) * i * j;
//			result += 8.51 * x * x * x * x * x * x ;
//		}
//	}
//	return result;
// }
//

//
//	return result;
//}

// 计算线性回归参数 a (斜率) 和 b (截距)
// void linear_regression(int n, double x[], double y[], double *a, double *b) {
//	double sum_x = 0, sum_y = 0, sum_xy = 0, sum_xx = 0;
//	int i;
//
//	for (i = 0; i < n; i++) {
//		sum_x += x[i];
//		sum_y += y[i];
//		sum_xy += x[i] * y[i];
//		sum_xx += x[i] * x[i];
//	}
//
//	// 计算斜率 (a) 和截距 (b)
//	*a = (n * sum_xy - sum_x * sum_y) / (n * sum_xx - sum_x * sum_x);
//	*b = (sum_y - (*a) * sum_x) / n;
//}

//// 对两个 double 进行加法
// double add(double a, double b) {
//	return a + b;
// }
//
//// 对两个 double 进行乘法
// double multiply(double a, double b) {
//	return a * b;
// }
//
//// 对 double 进行幂运算
// double power(double base, double exponent) {
//	return pow(base, exponent);
// }
//
//// double_add_test 函数的重构版本
// void double_add_test(double* leftOp, double* rightOp, double* result, size_t iteration_num) {
//	for (size_t idx = 0; idx < iteration_num; idx++) {
//		double sum = add(leftOp[idx], rightOp[idx]);
//		result[idx] = sum;
//
//		double x = add(leftOp[idx], 12.789);
//		double y = add(rightOp[idx], 15.653);
//		double z = add(x, y);
//		double pow_result = power(z, 2.5);
//		result[idx] = multiply(result[idx], pow_result);
//	}
//	return;
// }

// float updateQuaternion_w(float q0, float q1, float q2, float q3, float ax, float ay, float az, float gx, float gy, float gz, float beta, float deltaT) {
//	float norm = sqrt(ax * ax + ay * ay + az * az);
//	if (norm == 0.0f) return q0;
//	norm = 1.0f / norm;
//	ax *= norm;
//	ay *= norm;
//	az *= norm;
//
//	float f1 = 2.0f * (q1 * q3 - q0 * q2) - ax;
//	float f2 = 2.0f * (q0 * q1 + q2 * q3) - ay;
//	float f3 = 1.0f - 2.0f * (q1 * q1 + q2 * q2) - az;
//	float J_14or21 = 2.0f * q1;
//
//	float step0 = J_14or21 * f2 - 2.0f * q2 * f1;
//	norm = sqrt(step0 * step0);
//	norm = 1.0f / norm;
//	step0 *= norm;
//
//	float qDot0 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz) - beta * step0;
//	q0 += qDot0 * deltaT;
//
//	norm = sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
//	norm = 1.0f / norm;
//	return q0 * norm;
// }
//
// float updateQuaternion_x(float q0, float q1, float q2, float q3, float ax, float ay, float az, float gx, float gy, float gz, float beta, float deltaT) {
//	float norm = sqrt(ax * ax + ay * ay + az * az);
//	if (norm == 0.0f) return q1;
//	norm = 1.0f / norm;
//	ax *= norm;
//	ay *= norm;
//	az *= norm;
//
//	float f1 = 2.0f * (q1 * q3 - q0 * q2) - ax;
//	float f2 = 2.0f * (q0 * q1 + q2 * q3) - ay;
//	float f3 = 1.0f - 2.0f * (q1 * q1 + q2 * q2) - az;
//	float J_14or21 = 2.0f * q1;
//
//	float step1 = 2.0f * q3 * f1 + J_14or21 * f2 - 2.0f * (q0 * f2 + q2 * f1);
//	norm = sqrt(step1 * step1);
//	norm = 1.0f / norm;
//	step1 *= norm;
//
//	float qDot1 = 0.5f * (q0 * gx + q2 * gz - q3 * gy) - beta * step1;
//	q1 += qDot1 * deltaT;
//
//	norm = sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
//	norm = 1.0f / norm;
//	return q1 * norm;
// }
//
// float normalizeAccelerometer(float ax, float ay, float az) {
//	float norm = sqrt(ax * ax + ay * ay + az * az);
//	return norm == 0.0f ? 0.0f : 1.0f / norm;
// }
//
// float computeGradientStep0(float q0, float q1, float q2, float q3, float ax, float ay, float az) {
//	float f1 = 2.0f * (q1 * q3 - q0 * q2) - ax;
//	float f2 = 2.0f * (q0 * q1 + q2 * q3) - ay;
//	float f3 = 1.0f - 2.0f * (q1 * q1 + q2 * q2) - az;
//	float J_14or21 = 2.0f * q1;
//
//	float step0 = J_14or21 * f2 - 2.0f * q2 * f1;
//	float norm = sqrt(step0 * step0);
//	norm = 1.0f / norm;
//	return step0 * norm;
// }

// typedef uint64_t float64;

// void float64_add(float64 a, float64 b, float64 *result) {
//	// Extract components of first number
//	int sign1 = (a >> 63) & 1;
//	int exponent1 = (a >> 52) & 0x7FF;
//	uint64_t mantissa1 = a & 0xFFFFFFFFFFFFF;
//
//	// Normalize mantissa to have an implicit leading one if it's not zero
//	if (exponent1 != 0) mantissa1 |= 0x10000000000000;
//
//	// Extract components of second number
//	int sign2 = (b >> 63) & 1;
//	int exponent2 = (b >> 52) & 0x7FF;
//	uint64_t mantissa2 = b & 0xFFFFFFFFFFFFF;
//
//	if (exponent2 != 0) mantissa2 |= 0x10000000000000;
//
//	// Align mantissas
//	if (exponent1 > exponent2) {
//		mantissa2 >>= (exponent1 - exponent2);
//		exponent2 = exponent1;
//	} else if (exponent1 < exponent2) {
//		mantissa1 >>= (exponent2 - exponent1);
//		exponent1 = exponent2;
//	}
//
//	// Add mantissas (ignore signs for simplicity in this example)
//	uint64_t mantissa = mantissa1 + mantissa2;
//
//	// Normalize result
//	while (mantissa >= (1ULL << 53)) {
//		mantissa >>= 1;
//		exponent1++;
//	}
//
//	// Avoid double normalization
//	if (exponent1 >= 0x7FF) {
//		exponent1 = 0x7FF;
//		mantissa = 0; // Produce infinity if overflow
//	} else if (mantissa >= (1ULL << 52)) {
//		mantissa &= ~(1ULL << 52);
//	}
//
//	// Combine components back into a double
//	*result = ((uint64_t)sign1 << 63) | ((uint64_t)exponent1 << 52) | (mantissa & 0xFFFFFFFFFFFFF);
// }

// float invSqrt(float x) {
//	float halfx = 0.5f * x;
//	float y = x;
//	//#pragma unsupported
//	long i = *(long*)&y;
//	i = 0x5f3759df - (i>>1);
//	y = *(float*)&i;
//	//#end
//	y = y * (1.5f - (halfx * y * y));
//	return y;
// }

// void
// MadgwickAHRSupdateIMU(float ax,
//		      float * q0_ptr)
//{
//	float q0 = *q0_ptr;
// }
//
// void
// MadgwickAHRSupdate(float gx, float ax,
//		   float * q0_ptr)
//{
//	float q0 = *q0_ptr;
//	if((ax == 0.0f)) {
//		MadgwickAHRSupdateIMU(ax, q0_ptr);
//		return;
//	}
// }

// void MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz,
//		   float* q0_ptr, float* q1_ptr, float* q2_ptr, float* q3_ptr) {

void
MadgwickAHRSupdate(float   mx,
		   float * q0_ptr)
{
	float q0 = *q0_ptr;
	float _2q0mx;

	_2q0mx = 2.0f * q0 * mx;
}
