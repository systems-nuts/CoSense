#include <stdio.h>
#include <math.h>

#define FRAC_Q 10
#define FRAC_BASE (1 << FRAC_Q)
#define BIT_WIDTH 32



// Quantization function
int quantize(float value, int frac_base) {
	return round(value * frac_base);
}

// Dequantization function
float dequantize(int quantized_value, int frac_base) {
	return (float)quantized_value / frac_base;
}

extern int perform_addition(float a, float b);
extern int perform_subtraction(float a, float b);
//extern int perform_multiplication(int a, int b);

int main() {
	float num1 = 8.10;
	printf("num1: %f\n", num1);
	float num2 = 2.25;
	printf("num2: %f\n", num2);
	printf("FRAC_BASE: %d\n", FRAC_BASE);

	int a = 10;
	int b = 3;

	float num3 = 0;
	float num4 = 0;
	float num5 = 0;
	float num6 = 0;

	int quantized_num1 = quantize(num1, FRAC_BASE);
	int quantized_num2 = quantize(num2, FRAC_BASE);

	int addition_result = perform_addition(num1, num2);
	float actual_addition_result = dequantize(addition_result, FRAC_BASE);

	printf("Addition Result after quantization: %d\n", addition_result);
	printf("expected Addition Result after dequantization: %f\n",dequantize(quantized_num1 + quantized_num2, FRAC_BASE));
	printf("Actual Addition Result after dequantization: %f\n", actual_addition_result);


	// Perform original operations to compare results later
	float original_addition = num1 + num2;
	float original_subtraction = num1 - num2;
	float original_multiplication = num1 * num2;
	float original_division = num1 / num2;
	printf("Original Addition: %f\n", original_addition);
	printf("Original Subtraction: %f\n", original_subtraction);

	int subtraction_result = perform_subtraction(num1, num2);
	float actual_subtraction_result = dequantize(subtraction_result, FRAC_BASE);
	printf("Subtraction Result after quantization: %d\n", subtraction_result);
	printf("Actual Subtraction Result after dequantization: %f\n", actual_subtraction_result);

	// Quantization of input numbers


	//int subtraction_result = perform_subtraction(a, b);
	//printf("Subtraction Result: %d\n", subtraction_result);


//	// Perform operations on the quantized numbers
//	float addition = perform_addition(num1, num2);
//
//	float subtraction = perform_subtraction(num1, num2);
//
//	float multiplication = perform_multiplication(num1, num2);
//
//	float division = perform_division(num1, num2);
//
//
//	// Dequantization of actual results
//	float my_dequantized_addition = dequantize(addition, FRAC_BASE);
//	float my_dequantized_subtraction = dequantize(subtraction, FRAC_BASE);
//	float my_dequantized_multiplication = dequantize( multiplication, FRAC_BASE * FRAC_BASE);
//	float my_dequantized_division = division;
//
//
//	num3 = (my_dequantized_addition + my_dequantized_subtraction) / 2;
//	num4 = (my_dequantized_addition -my_dequantized_subtraction) / 2;
//	printf("num3: %f\n", num3);
//	printf("num4: %f\n", num4);
//
//
//	num5 = sqrt(my_dequantized_multiplication*my_dequantized_division);
//	printf("xdeduced : %f\n", num5);
//	num6 = sqrt(my_dequantized_multiplication/my_dequantized_division);
//	printf("num6: %f\n", num6);
//
//
//
//

//	printf("Expected after Dequantized Addition: %f\n", dequantize(quantized_num1 + quantized_num2, FRAC_BASE));
//	printf("Actual after Dequantized Addition: %f\n", my_dequantized_addition);
//
//	printf("Original Subtraction: %f\n", original_subtraction);
//	printf("Expected after Dequantized Subtraction: %f\n", dequantize(quantized_num1 - quantized_num2, FRAC_BASE));
//	printf("Actual after Dequantized Subtraction: %f\n", my_dequantized_subtraction);
//
//	printf("Original Multiplication: %f\n", original_multiplication);
//
//	printf("Expected after Dequantized Multiplication: %f\n", dequantize((int)((long long)quantized_num1 * quantized_num2), FRAC_BASE * FRAC_BASE));
//	//printf("Expected after Dequantized Multiplication: %f\n", dequantize((quantized_num1 * quantized_num2), (FRAC_BASE * FRAC_BASE)));
//	printf("Actual after Dequantized Multiplication: %f\n", my_dequantized_multiplication);
//
//	printf("Original Division: %f\n", original_division);
//	printf("Expected after Dequantized Division: %f\n", (float)quantized_num1 / quantized_num2 );
//	printf("Actual after Dequantized Division: %f\n", my_dequantized_division);

	return 0;
}