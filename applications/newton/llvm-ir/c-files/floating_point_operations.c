
#include <stdio.h>
#include "floating_point_operations.h"
#include <math.h>


 //Function to perform basic floating point operations
 //Function to perform addition
 float perform_addition(float a, float b) {

	return  a + b;
}



//// Function to perform subtraction
 float perform_subtraction(float a, float b) {
	return a - b;
 }
////
////// Function to perform multiplication
 float perform_multiplication(float a, float b) {
	//printf("Performing multiplication\ a: %f, b: %f\n", a, b);
	return a * b;
 }

 float perform_division(float a, float b) {
	 return a / b;
 }

//float
//perform_constant_multiplication(float a)
//{
//	return a * 0.21;
//}
//
// float
// perform_constant_division(float a)
//{
//	return a / 0.21;
//}
//
//float
//perform_operations(float a, float b, float c, float d)
//{
//	float result = a * b;	       // First multiplication
//	result	     = result + c;     // Addition
//	result	     = result - d;     // Subtraction
//	result	     = result * 1.5f;  // Second multiplication with a constant
//	return result;
//}


//float computePolynomial(float x) {
//	float result = 0.0;
//	for (int i = 0; i < 1000; ++i) {
//		result += 0.3 * x * x * x + 0.2 * x * x + 0.5 * x + 0.7;
//	}
//	return result;
//}
#define PI 3.14159265358979323846


//void fft(float* real, float* imag, int n) {
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
//}

double polynomial(double x) {
	// Evaluate a polynomial: P(x) = 3x^4 - 5x^3 + 2x^2 - x + 7
//	double result = 5.51*x*x*x*x - 0.21*x*x*x + 0.3*x*x + x;
//	return result;
	double result = x*x*x*x;
	return result;
}