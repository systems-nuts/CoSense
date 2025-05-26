/**
*    ||          ____  _ __
* +------+      / __ )(_) /_______________ _____  ___
* | 0xBC |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
* +------+    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
*  ||  ||    /_____/_/\__/\___/_/   \__,_/ /___/\___/
*
* Crazyflie Firmware
*
* Copyright (C) 2011-2012 Bitcraze AB
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, in version 3.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
*
*/





#include <math.h>
#define CONFIG_IMU_MADGWICK_QUATERNION
//#define CONFIG_IMU_MAHONY_QUATERNION_IMU
#define dt (1.0f/100.0f)	// sample frequency in Hz

#ifdef CONFIG_IMU_MADGWICK_QUATERNION
#define BETA_DEF     0.01f    // 2 * proportional gain
#else // MAHONY_QUATERNION_IMU
#define TWO_KP_DEF  (2.0f * 0.4f) // 2 * proportional gain
#define TWO_KI_DEF  (2.0f * 0.001f) // 2 * integral gain
#endif

#ifdef CONFIG_IMU_MADGWICK_QUATERNION


volatile float beta = BETA_DEF;
#else // MAHONY_QUATERNION_IMU

float twoKp = TWO_KP_DEF;    // 2 * proportional gain (Kp)
float twoKi = TWO_KI_DEF;    // 2 * integral gain (Ki)
float integralFBx = 0.0f;
float integralFBy = 0.0f;
float integralFBz = 0.0f;  // integral error terms scaled by Ki
#endif

volatile float M_PI_F = 3.14159265358979323846f;


static float invSqrt(float x);

#ifdef CONFIG_IMU_MADGWICK_QUATERNION
//void sensfusion6UpdateQImpl(float gx, float gy, float gz, float ax, float ay, float az, float dt,float* qw_ptr, float* qx_ptr, float* qy_ptr, float* qz_ptr)
void sensfusion6UpdateQImpl(float gx, float gy, float gz, float ax, float ay, float az,float* qw_ptr, float* qx_ptr, float* qy_ptr, float* qz_ptr)
{
	float qw = *qw_ptr;
	float qx = *qx_ptr;
	float qy = *qy_ptr;
	float qz = *qz_ptr;

	float recipNorm;
	float s0, s1, s2, s3;
	float qDot1, qDot2, qDot3, qDot4;
	float _2qw, _2qx, _2qy, _2qz, _4qw, _4qx, _4qy ,_8qx, _8qy, qwqw, qxqx, qyqy, qzqz;

	// Rate of change of quaternion from gyroscope
	qDot1 = 0.5f * (-qx * gx - qy * gy - qz * gz);
	qDot2 = 0.5f * (qw * gx + qy * gz - qz * gy);
	qDot3 = 0.5f * (qw * gy - qx * gz + qz * gx);
	qDot4 = 0.5f * (qw * gz + qx * gy - qy * gx);

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
	{
		// Normalise accelerometer measurement
		recipNorm = invSqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;

		// Auxiliary variables to avoid repeated arithmetic
		_2qw = 2.0f * qw;
		_2qx = 2.0f * qx;
		_2qy = 2.0f * qy;
		_2qz = 2.0f * qz;
		_4qw = 4.0f * qw;
		_4qx = 4.0f * qx;
		_4qy = 4.0f * qy;
		_8qx = 8.0f * qx;
		_8qy = 8.0f * qy;
		qwqw = qw * qw;
		qwqw = qw * qw;
		qxqx = qx * qx;
		qyqy = qy * qy;
		qzqz = qz * qz;

		// Gradient decent algorithm corrective step
		s0 = _4qw * qyqy + _2qy * ax + _4qw * qxqx - _2qx * ay;
		s1 = _4qx * qzqz - _2qz * ax + 4.0f * qwqw * qx - _2qw * ay - _4qx + _8qx * qxqx + _8qx * qyqy + _4qx * az;
		s2 = 4.0f * qwqw * qy + _2qw * ax + _4qy * qzqz - _2qz * ay - _4qy + _8qy * qxqx + _8qy * qyqy + _4qy * az;
		s3 = 4.0f * qxqx * qz - _2qx * ax + 4.0f * qyqy * qz - _2qy * ay;
		recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
		s0 *= recipNorm;
		s1 *= recipNorm;
		s2 *= recipNorm;
		s3 *= recipNorm;

		// Apply feedback step
		qDot1 -= beta * s0;
		qDot2 -= beta * s1;
		qDot3 -= beta * s2;
		qDot4 -= beta * s3;
	}
	// Integrate rate of change of quaternion to yield quaternion
	qw += qDot1 * dt;
	qx += qDot2 * dt;
	qy += qDot3 * dt;
	qz += qDot4 * dt;

	// Normalise quaternion
	recipNorm = invSqrt(qw*qw + qx*qx + qy*qy + qz*qz);
	qw *= recipNorm;
	qx *= recipNorm;
	qy *= recipNorm;
	qz *= recipNorm;

	// Store results in the output pointers
	*qw_ptr = qw;
	*qx_ptr = qx;
	*qy_ptr = qy;
	*qz_ptr = qz;
}
#else
// Madgwick's implementation of Mahony's AHRS algorithm.
// See: http://www.x-io.co.uk/open-source-ahrs-with-x-imu
//
// Date     Author      Notes
// 29/09/2011 SOH Madgwick    Initial release
// 02/10/2011 SOH Madgwick  Optimised for reduced CPU load
void sensfusion6UpdateQImpl(float gx, float gy, float gz, float ax, float ay, float az,float* qw_ptr, float* qx_ptr, float* qy_ptr, float* qz_ptr)
{

	float qw = *qw_ptr;
	float qx = *qx_ptr;
	float qy = *qy_ptr;
	float qz = *qz_ptr;

	float recipNorm;
	float halfvx, halfvy, halfvz;
	float halfex, halfey, halfez;
	float qa, qb, qc;

	gx = gx * M_PI_F / 180;
	gy = gy * M_PI_F / 180;
	gz = gz * M_PI_F / 180;

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
	{
		// Normalise accelerometer measurement
		recipNorm = invSqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;

		// Estimated direction of gravity and vector perpendicular to magnetic flux
		halfvx = qx * qz - qw * qy;
		halfvy = qw * qx + qy * qz;
		halfvz = qw * qw - 0.5f + qz * qz;

		// Error is sum of cross product between estimated and measured direction of gravity
		halfex = (ay * halfvz - az * halfvy);
		halfey = (az * halfvx - ax * halfvz);
		halfez = (ax * halfvy - ay * halfvx);

		// Compute and apply integral feedback if enabled
		if(twoKi > 0.0f)
		{
			integralFBx += twoKi * halfex * dt;  // integral error scaled by Ki
			integralFBy += twoKi * halfey * dt;
			integralFBz += twoKi * halfez * dt;
			gx += integralFBx;  // apply integral feedback
			gy += integralFBy;
			gz += integralFBz;
		}
		else
		{
			integralFBx = 0.0f; // prevent integral windup
			integralFBy = 0.0f;
			integralFBz = 0.0f;
		}

		// Apply proportional feedback
		gx += twoKp * halfex;
		gy += twoKp * halfey;
		gz += twoKp * halfez;
	}

	// Integrate rate of change of quaternion
	gx *= (0.5f * dt);   // pre-multiply common factors
	gy *= (0.5f * dt);
	gz *= (0.5f * dt);
	qa = qw;
	qb = qx;
	qc = qy;
	qw += (-qb * gx - qc * gy - qz * gz);
	qx += (qa * gx + qc * gz - qz * gy);
	qy += (qa * gy - qb * gz + qz * gx);
	qz += (qa * gz + qb * gy - qc * gx);

	// Normalise quaternion
	recipNorm = invSqrt(qw * qw + qx * qx + qy * qy + qz * qz);
	qw *= recipNorm;
	qx *= recipNorm;
	qy *= recipNorm;
	qz *= recipNorm;

	// Store results in the output pointers
	*qw_ptr = qw;
	*qx_ptr = qx;
	*qy_ptr = qy;
	*qz_ptr = qz;

}
#endif

//---------------------------------------------------------------------------------------------------
// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
float invSqrt(float x)
{
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}