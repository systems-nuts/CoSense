#include <stdint.h>
#include <stdbool.h>
#include "MadgwickAHRSDiff.h"

#define sampleFreq 28  // sample frequency in Hz
#define betaDef 0.1f   // 2 * proportional gain

typedef float bmx055xAcceleration;
typedef float bmx055yAcceleration;
typedef float bmx055zAcceleration;
typedef float bmx055xAngularRate;
typedef float bmx055yAngularRate;
typedef float bmx055zAngularRate;
typedef float bmx055xMagneto;
typedef float bmx055yMagneto;
typedef float bmx055zMagneto;


// Quantize a floating-point value to fixed-point format
int32_t quantize(float value, int32_t frac_base) {
	return (int32_t)round(value * frac_base);  // Quantize to fixed-point
}


// Align a smaller frac_q to the largest frac_q
int32_t align_to_frac_q(int32_t value, int32_t frac_q_value, int32_t target_frac_q) {
	if (frac_q_value < target_frac_q) {
		return value << (target_frac_q - frac_q_value);  // Left shift to align
	}
	return value;  // No shift needed if already at the target_frac_q
}






// Define function to multiply two fixed-point numbers with different precision
int32_t
mulfix(int32_t x, int32_t y, int32_t input_frac_q1, int32_t input_frac_q2, int32_t output_frac_q)
{
	int64_t result = (int64_t)x * y;
	int32_t shift  = input_frac_q1 + input_frac_q2 - output_frac_q;
	if (shift > 0)
	{
		return result >> shift;	 // Right shift to reduce value
	}
	else
	{
		return result << (-shift);  // Left shift to enlarge value
	}
}

// Convert one fixed-point number to another precision
int32_t
convert_frac_q(int32_t value, int32_t input_frac_q, int32_t output_frac_q)
{
	if (input_frac_q > output_frac_q)
	{
		return value >> (input_frac_q - output_frac_q);	 // Right shift to reduce value
	}
	else
	{
		return value << (output_frac_q - input_frac_q);	 // Left shift to enlarge value
	}
}

/*
 *	Compute square root of x and reciprocal with Goldschmidt's method
 */

int32_t sqrt_rsqrt(int32_t x, int recip, int32_t frac_q) {
	int32_t frac_base = (1 << frac_q);  // Calculate the base value dynamically from frac_q

	if (recip) {
		int32_t int_halfx = mulfix(0.5 * frac_base, x, frac_q, frac_q, frac_q);  // half * x
		float fp_y = (float)x / frac_base;  // Convert to floating-point for approximation
		long i = *(long*)&fp_y;
		i = 0x5f3759df - (i >> 1);  // Initial magic number approximation
		fp_y = *(float*)&i;
		int32_t int_y = fp_y * frac_base;  // Convert back to fixed-point
		int_y = mulfix(int_y,
					   ((int32_t)(1.5f * frac_base) - mulfix(mulfix(int_halfx, int_y, frac_q, frac_q, frac_q), int_y, frac_q, frac_q, frac_q)),
					   frac_q, frac_q, frac_q);
		return int_y;
	} else {
		int32_t res = (int32_t)sqrt((double)x) << (frac_q / 2);  // sqrt(x) in integer, shifted to frac_q/2

		if (frac_q % 2) {
			// If frac_q is odd, scale by sqrt(2) to adjust precision.
			return mulfix(res, 1.414213562 * frac_base, frac_q / 2, frac_q, frac_q);
		} else {
			return res;
		}
	}
}


void
MadgwickAHRSupdate(bmx055xAngularRate gx, bmx055yAngularRate gy, bmx055zAngularRate gz,
		   bmx055xAcceleration ax, bmx055yAcceleration ay, bmx055zAcceleration az,
		   bmx055xMagneto mx, bmx055yMagneto my, bmx055zMagneto mz,
		   int32_t * q0_ptr, int32_t * q1_ptr, int32_t * q2_ptr, int32_t * q3_ptr)
{
	// Read the current quaternion values
	int32_t gx_fixed = quantize(gx, FRAC_BASE_GYRO_X);
	int32_t gy_fixed = quantize(gy, FRAC_BASE_GYRO_Y);
	int32_t gz_fixed = quantize(gz, FRAC_BASE_GYRO_Z);

	int32_t ax_fixed = quantize(ax, FRAC_BASE_ACCEL_X);
	int32_t ay_fixed = quantize(ay, FRAC_BASE_ACCEL_Y);
	int32_t az_fixed = quantize(az, FRAC_BASE_ACCEL_Z);

	int32_t mx_fixed = quantize(mx, FRAC_BASE_MAG_X);
	int32_t my_fixed = quantize(my, FRAC_BASE_MAG_Y);
	int32_t mz_fixed = quantize(mz, FRAC_BASE_MAG_Z);



	//


















	int32_t q0 = *q0_ptr;
	int32_t q1 = *q1_ptr;
	int32_t q2 = *q2_ptr;
	int32_t q3 = *q3_ptr;

	int32_t recipNorm;
	int32_t s0, s1, s2, s3;
	int32_t qDot1, qDot2, qDot3, qDot4;
	int32_t hx, hy;
	int32_t _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz;
	int32_t _2q0, _2q1, _2q2, _2q3, _2q0q2, _2q2q3;
	int32_t q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;

	// Convert gyroscope data to a common precision
	gx = convert_frac_q(gx, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);	// Keeping it as FRAC_Q_GYRO_X
	gy = convert_frac_q(gy, FRAC_Q_GYRO_Y, FRAC_Q_GYRO_X);	// Converting to common precision
	gz = convert_frac_q(gz, FRAC_Q_GYRO_Z, FRAC_Q_GYRO_X);	// Converting to common precision

	// Use IMU algorithm if magnetometer measurement is invalid (avoids NaN in magnetometer normalization)
	if ((mx == 0x0) && (my == 0x0) && (mz == 0x0))
	{
		MadgwickAHRSupdateIMU(gx, gy, gz, ax, ay, az, q0_ptr, q1_ptr, q2_ptr, q3_ptr);
		return;
	}

	// Rate of change of quaternion from gyroscope
	qDot1 = (-mulfix(q1, gx, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - mulfix(q2, gy, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - mulfix(q3, gz, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X)) / 2;
	qDot2 = (mulfix(q0, gx, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(q2, gz, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - mulfix(q3, gy, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X)) / 2;
	qDot3 = (mulfix(q0, gy, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - mulfix(q1, gz, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(q3, gx, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X)) / 2;
	qDot4 = (mulfix(q0, gz, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(q1, gy, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - mulfix(q2, gx, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X)) / 2;

	// Compute feedback only if accelerometer measurement is valid (avoids NaN in accelerometer normalization)
	if (!((ax == 0x0) && (ay == 0x0) && (az == 0x0)))
	{
		// Normalize accelerometer measurement to common precision
		recipNorm = sqrt_rsqrt(mulfix(ax, ax, FRAC_Q_ACCEL_X, FRAC_Q_ACCEL_X, FRAC_Q_GYRO_X) +
					   mulfix(ay, ay, FRAC_Q_ACCEL_Y, FRAC_Q_ACCEL_Y, FRAC_Q_GYRO_X) +
					   mulfix(az, az, FRAC_Q_ACCEL_Z, FRAC_Q_ACCEL_Z, FRAC_Q_GYRO_X),
				       true, FRAC_Q_GYRO_X);

		ax = mulfix(ax, recipNorm, FRAC_Q_ACCEL_X, FRAC_Q_GYRO_X, FRAC_Q_ACCEL_X);
		ay = mulfix(ay, recipNorm, FRAC_Q_ACCEL_Y, FRAC_Q_GYRO_X, FRAC_Q_ACCEL_Y);
		az = mulfix(az, recipNorm, FRAC_Q_ACCEL_Z, FRAC_Q_GYRO_X, FRAC_Q_ACCEL_Z);

		// Normalize magnetometer measurement to common precision
		recipNorm = sqrt_rsqrt(mulfix(mx, mx, FRAC_Q_MAG_X, FRAC_Q_MAG_X, FRAC_Q_GYRO_X) +
					   mulfix(my, my, FRAC_Q_MAG_Y, FRAC_Q_MAG_Y, FRAC_Q_GYRO_X) +
					   mulfix(mz, mz, FRAC_Q_MAG_Z, FRAC_Q_MAG_Z, FRAC_Q_GYRO_X),
				       true, FRAC_Q_GYRO_X);

		mx = mulfix(mx, recipNorm, FRAC_Q_MAG_X, FRAC_Q_GYRO_X, FRAC_Q_MAG_X);
		my = mulfix(my, recipNorm, FRAC_Q_MAG_Y, FRAC_Q_GYRO_X, FRAC_Q_MAG_Y);
		mz = mulfix(mz, recipNorm, FRAC_Q_MAG_Z, FRAC_Q_GYRO_X, FRAC_Q_MAG_Z);

		// Auxiliary variables to avoid repeated arithmetic
		_2q0mx = 2 * mulfix(q0, mx, FRAC_Q_GYRO_X, FRAC_Q_MAG_X, FRAC_Q_GYRO_X);
		_2q0my = 2 * mulfix(q0, my, FRAC_Q_GYRO_X, FRAC_Q_MAG_Y, FRAC_Q_GYRO_X);
		_2q1mx = 2 * mulfix(q1, mx, FRAC_Q_GYRO_X, FRAC_Q_MAG_X, FRAC_Q_GYRO_X);
		_2q0   = 2 * q0;
		_2q1   = 2 * q1;
		_2q2   = 2 * q2;
		_2q3   = 2 * q3;
		_2q0q2 = 2 * mulfix(q0, q2, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);
		_2q2q3 = 2 * mulfix(q2, q3, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);
		q0q0   = mulfix(q0, q0, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);
		q0q1   = mulfix(q0, q1, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);
		q0q2   = mulfix(q0, q2, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);
		q0q3   = mulfix(q0, q3, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);
		q1q1   = mulfix(q1, q1, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);
		q1q2   = mulfix(q1, q2, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);
		q1q3   = mulfix(q1, q3, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);
		q2q2   = mulfix(q2, q2, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);
		q2q3   = mulfix(q2, q3, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);
		q3q3   = mulfix(q3, q3, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);

		// Reference direction of Earth's magnetic field
		hx = mulfix(mx, q0q0, FRAC_Q_MAG_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - mulfix(_2q0my, q3, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(_2q0mz, q2, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(mx, q1q1, FRAC_Q_MAG_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(mulfix(_2q1, my, FRAC_Q_GYRO_X, FRAC_Q_MAG_Y, FRAC_Q_GYRO_X), q2, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(mulfix(_2q1, mz, FRAC_Q_GYRO_X, FRAC_Q_MAG_Z, FRAC_Q_GYRO_X), q3, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - mulfix(mx, q2q2, FRAC_Q_MAG_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - mulfix(mx, q3q3, FRAC_Q_MAG_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);

		hy = mulfix(_2q0mx, q3, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(my, q0q0, FRAC_Q_MAG_Y, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - mulfix(_2q0mz, q1, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(_2q1mx, q2, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - mulfix(my, q1q1, FRAC_Q_MAG_Y, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(my, q2q2, FRAC_Q_MAG_Y, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(mulfix(_2q2, mz, FRAC_Q_GYRO_X, FRAC_Q_MAG_Z, FRAC_Q_GYRO_X), q3, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - mulfix(my, q3q3, FRAC_Q_MAG_Y, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);

		_2bx = sqrt_rsqrt(mulfix(hx, hx, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) +
				      mulfix(hy, hy, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X),
				  false, FRAC_Q_GYRO_X);

		_2bz = -mulfix(_2q0mx, q2, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(_2q0my, q1, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(mz, q0q0, FRAC_Q_MAG_Z, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(_2q1mx, q3, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - mulfix(mz, q1q1, FRAC_Q_MAG_Z, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(mulfix(_2q2, my, FRAC_Q_GYRO_X, FRAC_Q_MAG_Y, FRAC_Q_GYRO_X), q3, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - mulfix(mz, q2q2, FRAC_Q_MAG_Z, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(mz, q3q3, FRAC_Q_MAG_Z, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);

		_4bx = 2 * _2bx;
		_4bz = 2 * _2bz;

		// Gradient descent algorithm corrective step
		s0 = -mulfix(_2q2, (2 * q1q3 - _2q0q2 - ax), FRAC_Q_GYRO_X, FRAC_Q_ACCEL_X, FRAC_Q_GYRO_X) + mulfix(_2q1, (2 * q0q1 + _2q2q3 - ay), FRAC_Q_GYRO_X, FRAC_Q_ACCEL_Y, FRAC_Q_GYRO_X) - mulfix(mulfix(_2bz, q2, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X), (mulfix(_2bx, (DEC2FRAC(0.5) - q2q2 - q3q3), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(_2bz, (q1q3 - q0q2), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - mx), FRAC_Q_GYRO_X, FRAC_Q_MAG_X, FRAC_Q_GYRO_X) + mulfix((-mulfix(_2bx, q3, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(_2bz, q1, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X)), (mulfix(_2bx, (q1q2 - q0q3), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(_2bz, (q0q1 + q2q3), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - my), FRAC_Q_GYRO_X, FRAC_Q_MAG_Y, +mulfix(mulfix(_2bx, q2, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X), (mulfix(_2bx, (q0q2 + q1q3), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(_2bz, (DEC2FRAC(0.5) - q1q1 - q2q2), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - mz), FRAC_Q_GYRO_X, FRAC_Q_MAG_Z, FRAC_Q_GYRO_X); FRAC_Q_GYRO_X);

		s1 = mulfix(_2q3, (2 * q1q3 - _2q0q2 - ax), FRAC_Q_GYRO_X, FRAC_Q_ACCEL_X, FRAC_Q_GYRO_X) + mulfix(_2q0, (2 * q0q1 + _2q2q3 - ay), FRAC_Q_GYRO_X, FRAC_Q_ACCEL_Y, FRAC_Q_GYRO_X) - 4 * mulfix(q1, (FRAC_BASE_GYRO_X - 2 * q1q1 - 2 * q2q2 - az), FRAC_Q_GYRO_X, FRAC_Q_ACCEL_Z, FRAC_Q_GYRO_X) + mulfix(mulfix(_2bz, q3, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X), (mulfix(_2bx, (DEC2FRAC(0.5) - q2q2 - q3q3), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(_2bz, (q1q3 - q0q2), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - mx), FRAC_Q_GYRO_X, FRAC_Q_MAG_X, FRAC_Q_GYRO_X) + mulfix(mulfix(_2bx, q2, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(_2bz, q0, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X), (mulfix(_2bx, (q1q2 - q0q3), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(_2bz, (q0q1 + q2q3), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - my), FRAC_Q_GYRO_X, FRAC_Q_MAG_Y, FRAC_Q_GYRO_X) + mulfix(mulfix(_2bx, q3, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - mulfix(_4bz, q1, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X), (mulfix(_2bx, (q0q2 + q1q3), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(_2bz, (DEC2FRAC(0.5) - q1q1 - q2q2), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - mz), FRAC_Q_GYRO_X, FRAC_Q_MAG_Z, FRAC_Q_GYRO_X);

		s2 = -mulfix(_2q0, (2 * q1q3 - _2q0q2 - ax), FRAC_Q_GYRO_X, FRAC_Q_ACCEL_X, FRAC_Q_GYRO_X) + mulfix(_2q3, (2 * q0q1 + _2q2q3 - ay), FRAC_Q_GYRO_X, FRAC_Q_ACCEL_Y, FRAC_Q_GYRO_X) - 4 * mulfix(q2, (FRAC_BASE_GYRO_X - 2 * q1q1 - 2 * q2q2 - az), FRAC_Q_GYRO_X, FRAC_Q_ACCEL_Z, FRAC_Q_GYRO_X) + mulfix((-mulfix(_4bx, q2, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - mulfix(_2bz, q0, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X)), (mulfix(_2bx, (DEC2FRAC(0.5) - q2q2 - q3q3), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(_2bz, (q1q3 - q0q2), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - mx), FRAC_Q_GYRO_X, FRAC_Q_MAG_X, FRAC_Q_GYRO_X) + mulfix((mulfix(_2bx, q1, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(_2bz, q3, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X)), (mulfix(_2bx, (q1q2 - q0q3), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(_2bz, (q0q1 + q2q3), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - my), FRAC_Q_GYRO_X, FRAC_Q_MAG_Y, FRAC_Q_GYRO_X) + mulfix((mulfix(_2bx, q0, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - mulfix(_4bz, q2, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X)), (mulfix(_2bx, (q0q2 + q1q3), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) + mulfix(_2bz, (DEC2FRAC(0.5) - q1q1 - q2q2), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) - mz), FRAC_Q_GYRO_X, FRAC_Q_MAG_Z, FRAC_Q_GYRO_X);

		s3 =   mulfix(_2q1, (2 * q1q3 - _2q0q2 - ax), FRAC_Q_GYRO_X, FRAC_Q_ACCEL_X, FRAC_Q_GYRO_X)
+ mulfix(_2q2, (2 * q0q1 + _2q2q3 - ay), FRAC_Q_GYRO_X, FRAC_Q_ACCEL_Y, FRAC_Q_GYRO_X)
+ mulfix((-mulfix(_4bx, q3, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) +
	mulfix(_2bz, q1, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X)),
       (mulfix(_2bx, (DEC2FRAC(0.5) - q2q2 - q3q3), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X)
	+ mulfix(_2bz, (q1q3 - q0q2), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X)
	- mx), FRAC_Q_GYRO_X, FRAC_Q_MAG_X, FRAC_Q_GYRO_X)
+ mulfix((-mulfix(_2bx, q0, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) +
	mulfix(_2bz, q2, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X,  FRAC_Q_GYRO_X)),(mulfix(_2bx, (q1q2 - q0q3), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X)
+ mulfix(_2bz, (q0q1 + q2q3), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X)
- my), FRAC_Q_GYRO_X, FRAC_Q_MAG_Y, FRAC_Q_GYRO_X) + mulfix(mulfix(_2bx, q1, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X),
			    (mulfix(_2bx, (q0q2 + q1q3), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X)
			     + mulfix(_2bz, (DEC2FRAC(0.5) - q1q1 - q2q2), FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X)
			     - mz), FRAC_Q_GYRO_X, FRAC_Q_MAG_Z, FRAC_Q_GYRO_X);


		// Normalize the step magnitude
		recipNorm = sqrt_rsqrt(mulfix(s0, s0, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) +
					   mulfix(s1, s1, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) +
					   mulfix(s2, s2, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) +
					   mulfix(s3, s3, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X), true, FRAC_Q_GYRO_X);

		// Apply normalization
		s0 = mulfix(s0, recipNorm, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);
		s1 = mulfix(s1, recipNorm, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);
		s2 = mulfix(s2, recipNorm, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);
		s3 = mulfix(s3, recipNorm, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);

		// Apply feedback step
		qDot1 -= mulfix(beta, s0, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);
		qDot2 -= mulfix(beta, s1, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);
		qDot3 -= mulfix(beta, s2, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);
		qDot4 -= mulfix(beta, s3, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);
	}


	// Integrate rate of change of quaternion to yield quaternion
	q0 += qDot1 / sampleFreq;
	q1 += qDot2 / sampleFreq;
	q2 += qDot3 / sampleFreq;
	q3 += qDot4 / sampleFreq;

	// Normalize quaternion
	recipNorm = sqrt_rsqrt(mulfix(q0, q0, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) +
				   mulfix(q1, q1, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) +
				   mulfix(q2, q2, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X) +
				   mulfix(q3, q3, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X), true, FRAC_Q_GYRO_X);

	q0 = mulfix(q0, recipNorm, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);
	q1 = mulfix(q1, recipNorm, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);
	q2 = mulfix(q2, recipNorm, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);
	q3 = mulfix(q3, recipNorm, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X, FRAC_Q_GYRO_X);

	// Update the output quaternion pointers
	*q0_ptr = q0;
	*q1_ptr = q1;
	*q2_ptr = q2;
	*q3_ptr = q3;
}


