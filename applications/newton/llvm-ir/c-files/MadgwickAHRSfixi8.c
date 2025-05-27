#include "MadgwickAHRSfixi8.h"

#define sampleFreq    28           // sample frequency in Hz
#define betaDef       0.1f         // 2 * proportional gain
#define FRAC_BASE     16          // 定点数基准
#define FRAC_Q        4           // 定点数小数部分位数

typedef int8_t bmx055xAcceleration;
typedef int8_t bmx055yAcceleration;
typedef int8_t bmx055zAcceleration;
typedef int8_t bmx055xAngularRate;
typedef int8_t bmx055yAngularRate;
typedef int8_t bmx055zAngularRate;
typedef int8_t bmx055xMagneto;
typedef int8_t bmx055yMagneto;
typedef int8_t bmx055zMagneto;

volatile int8_t beta = (int8_t)(betaDef * FRAC_BASE);  // 缩放后的beta

// 定点乘法运算
int8_t mulfix(int8_t x, int8_t y) {
	return (int16_t)x * (int16_t)y / FRAC_BASE;
}

// 计算平方根和倒数
int8_t sqrt_rsqrt(int8_t x, int recip) {
	if (recip) {
		int8_t int_halfx = mulfix((int8_t)(0.5 * FRAC_BASE), x);
		float fp_y = (float)x / FRAC_BASE;
		int i = *(int*)&fp_y;
		i = 0x5f3759df - (i >> 1); // Fast inverse square root approximation
		fp_y = *(float*)&i;
		int8_t int_y = (int8_t)(fp_y * FRAC_BASE);
		int_y = mulfix(int_y, (int8_t)((1.5f * FRAC_BASE) - mulfix(mulfix(int_halfx, int_y), int_y)));
		return int_y;
	} else {
		int8_t res = (int8_t)sqrt((double)x) << (FRAC_Q / 2);
		if (FRAC_Q % 2) {
			return mulfix(res, (int8_t)(1.414213562 * FRAC_BASE));
		} else {
			return res;
		}
	}
}

//====================================================================================================
// Madgwick算法更新函数

void MadgwickAHRSupdate(bmx055xAngularRate gx, bmx055yAngularRate gy, bmx055zAngularRate gz,
		   bmx055xAcceleration ax, bmx055yAcceleration ay, bmx055zAcceleration az,
		   bmx055xMagneto mx, bmx055yMagneto my, bmx055zMagneto mz,
		   int8_t* q0_ptr, int8_t* q1_ptr, int8_t* q2_ptr, int8_t* q3_ptr) {

	int8_t q0 = *q0_ptr;
	int8_t q1 = *q1_ptr;
	int8_t q2 = *q2_ptr;
	int8_t q3 = *q3_ptr;

	int8_t recipNorm;
	int8_t s0, s1, s2, s3;
	int8_t qDot1, qDot2, qDot3, qDot4;
	int8_t hx, hy;
	int8_t _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz;
	int8_t _2q0, _2q1, _2q2, _2q3, _2q0q2, _2q2q3, q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;

	// 当磁力计无效时，使用IMU算法
	if ((mx == 0x0) && (my == 0x0) && (mz == 0x0)) {
		MadgwickAHRSupdateIMU(gx, gy, gz, ax, ay, az, q0_ptr, q1_ptr, q2_ptr, q3_ptr);
		return;
	}

	// 根据陀螺仪的变化更新四元数
	qDot1 = (-mulfix(q1, gx) - mulfix(q2, gy) - mulfix(q3, gz)) / 2;
	qDot2 = ( mulfix(q0, gx) + mulfix(q2, gz) - mulfix(q3, gy)) / 2;
	qDot3 = ( mulfix(q0, gy) - mulfix(q1, gz) + mulfix(q3, gx)) / 2;
	qDot4 = ( mulfix(q0, gz) + mulfix(q1, gy) - mulfix(q2, gx)) / 2;

	// 如果加速度计有效，则进行归一化
	if (!((ax == 0x0) && (ay == 0x0) && (az == 0x0))) {
		recipNorm = sqrt_rsqrt(mulfix(ax, ax) + mulfix(ay, ay) + mulfix(az, az), true);
		ax = mulfix(ax, recipNorm);
		ay = mulfix(ay, recipNorm);
		az = mulfix(az, recipNorm);

		recipNorm = sqrt_rsqrt(mulfix(mx, mx) + mulfix(my, my) + mulfix(mz, mz), true);
		mx = mulfix(mx, recipNorm);
		my = mulfix(my, recipNorm);
		mz = mulfix(mz, recipNorm);

		// 算法修正
		_2q0mx = 2 * mulfix(q0, mx);
		_2q0my = 2 * mulfix(q0, my);
		_2q0mz = 2 * mulfix(q0, mz);
		_2q1mx = 2 * mulfix(q1, mx);
		_2q0 = 2 * q0;
		_2q1 = 2 * q1;
		_2q2 = 2 * q2;
		_2q3 = 2 * q3;
		_2q0q2 = 2 * mulfix(q0, q2);
		_2q2q3 = 2 * mulfix(q2, q3);
		q0q0 = mulfix(q0, q0);
		q0q1 = mulfix(q0, q1);
		q0q2 = mulfix(q0, q2);
		q0q3 = mulfix(q0, q3);
		q1q1 = mulfix(q1, q1);
		q1q2 = mulfix(q1, q2);
		q1q3 = mulfix(q1, q3);
		q2q2 = mulfix(q2, q2);
		q2q3 = mulfix(q2, q3);
		q3q3 = mulfix(q3, q3);

		// 计算地磁方向
		hx = mulfix(mx, q0q0) - mulfix(_2q0my, q3) + mulfix(_2q0mz, q2) + mulfix(mx, q1q1) +
		     mulfix(mulfix(_2q1, my), q2) + mulfix(mulfix(_2q1, mz), q3) - mulfix(mx, q2q2) - mulfix(mx, q3q3);
		hy = mulfix(_2q0mx, q3) + mulfix(my, q0q0) - mulfix(_2q0mz, q1) + mulfix(_2q1mx, q2) -
		     mulfix(my, q1q1) + mulfix(my, q2q2) + mulfix(mulfix(_2q2, mz), q3) - mulfix(my, q3q3);
		_2bx = sqrt_rsqrt(mulfix(hx, hx) + mulfix(hy, hy), false);
		_2bz = -mulfix(_2q0mx, q2) + mulfix(_2q0my, q1) + mulfix(mz, q0q0) + mulfix(_2q1mx, q3) -
		       mulfix(mz, q1q1) + mulfix(mulfix(_2q2, my), q3) - mulfix(mz, q2q2) + mulfix(mz, q3q3);
		_4bx = 2 * _2bx;
		_4bz = 2 * _2bz;

		// 梯度下降法修正
		s0 = -mulfix(_2q2, (2 * q1q3 - _2q0q2 - ax)) + mulfix(_2q1, (2 * q0q1 + _2q2q3 - ay)) -
		     mulfix(mulfix(_2bz, q2), (mulfix(_2bx, (FRAC_BASE / 2 - q2q2 - q3q3)) + mulfix(_2bz, (q1q3 - q0q2)) - mx)) +
		     mulfix((-mulfix(_2bx, q3) + mulfix(_2bz, q1)), (mulfix(_2bx, (q1q2 - q0q3)) + mulfix(_2bz, (q0q1 + q2q3)) - my)) +
		     mulfix(mulfix(_2bx, q2), (mulfix(_2bx, (q0q2 + q1q3)) + mulfix(_2bz, (FRAC_BASE / 2 - q1q1 - q2q2)) - mz));
		s1 = mulfix(_2q3, (2 * q1q3 - _2q0q2 - ax)) + mulfix(_2q0, (2 * q0q1 + _2q2q3 - ay)) -
		     4 * mulfix(q1, (FRAC_BASE - 2 * q1q1 - 2 * q2q2 - az)) + mulfix(mulfix(_2bz, q3),
										     (mulfix(_2bx, (FRAC_BASE / 2 - q2q2 - q3q3)) + mulfix(_2bz, (q1q3 - q0q2)) - mx)) +
		     mulfix(mulfix(_2bx, q2) + mulfix(_2bz, q0), (mulfix(_2bx, (q1q2 - q0q3)) + mulfix(_2bz, (q0q1 + q2q3)) - my)) +
		     mulfix(mulfix(_2bx, q3) - mulfix(_4bz, q1), (mulfix(_2bx, (q0q2 + q1q3)) + mulfix(_2bz, (FRAC_BASE / 2 - q1q1 - q2q2)) - mz));
		s2 = -mulfix(_2q0, (2 * q1q3 - _2q0q2 - ax)) + mulfix(_2q3, (2 * q0q1 + _2q2q3 - ay)) -
		     4 * mulfix(q2, (FRAC_BASE - 2 * q1q1 - 2 * q2q2 - az)) + mulfix((-mulfix(_4bx, q2) - mulfix(_2bz, q0)),
										     (mulfix(_2bx, (FRAC_BASE / 2 - q2q2 - q3q3)) + mulfix(_2bz, (q1q3 - q0q2)) - mx)) +
		     mulfix((mulfix(_2bx, q1) + mulfix(_2bz, q3)), (mulfix(_2bx, (q1q2 - q0q3)) + mulfix(_2bz, (q0q1 + q2q3)) - my)) +
		     mulfix((mulfix(_2bx, q0) - mulfix(_4bz, q2)), (mulfix(_2bx, (q0q2 + q1q3)) + mulfix(_2bz, (FRAC_BASE / 2 - q1q1 - q2q2)) - mz));
		s3 = mulfix(_2q1, (2 * q1q3 - _2q0q2 - ax)) + mulfix(_2q2, (2 * q0q1 + _2q2q3 - ay)) +
		     mulfix((-mulfix(_4bx, q3) + mulfix(_2bz, q1)), (mulfix(_2bx, (FRAC_BASE / 2 - q2q2 - q3q3)) +
								     mulfix(_2bz, (q1q3 - q0q2)) - mx)) + mulfix((-mulfix(_2bx, q0) + mulfix(_2bz, q2)), (mulfix(_2bx, (q1q2 - q0q3)) +
								     mulfix(_2bz, (q0q1 + q2q3)) - my)) + mulfix(mulfix(_2bx, q1), (mulfix(_2bx, (q0q2 + q1q3)) +
					       mulfix(_2bz, (FRAC_BASE / 2 - q1q1 - q2q2)) - mz));
		recipNorm = sqrt_rsqrt(mulfix(s0, s0) + mulfix(s1, s1) + mulfix(s2, s2) + mulfix(s3, s3), true);
		s0 = mulfix(s0, recipNorm);
		s1 = mulfix(s1, recipNorm);
		s2 = mulfix(s2, recipNorm);
		s3 = mulfix(s3, recipNorm);

		// 应用反馈修正
		qDot1 -= mulfix(beta, s0);
		qDot2 -= mulfix(beta, s1);
		qDot3 -= mulfix(beta, s2);
		qDot4 -= mulfix(beta, s3);
	}

	// 更新四元数
	q0 += qDot1 / sampleFreq;
	q1 += qDot2 / sampleFreq;
	q2 += qDot3 / sampleFreq;
	q3 += qDot4 / sampleFreq;

	// 归一化四元数
	recipNorm = sqrt_rsqrt(mulfix(q0, q0) + mulfix(q1, q1) + mulfix(q2, q2) + mulfix(q3, q3), true);
	q0 = mulfix(q0, recipNorm);
	q1 = mulfix(q1, recipNorm);
	q2 = mulfix(q2, recipNorm);
	q3 = mulfix(q3, recipNorm);

	*q0_ptr = q0;
	*q1_ptr = q1;
	*q2_ptr = q2;
	*q3_ptr = q3;
}

//====================================================================================================
// IMU-only update function

void MadgwickAHRSupdateIMU(bmx055xAngularRate gx, bmx055yAngularRate gy, bmx055zAngularRate gz,
		      bmx055xAcceleration ax, bmx055yAcceleration ay, bmx055zAcceleration az,
		      int8_t* q0_ptr, int8_t* q1_ptr, int8_t* q2_ptr, int8_t* q3_ptr) {
	// IMU-only update逻辑可以参照上面的MadgwickAHRSupdate编写。
	// 基本上可以移除磁力计相关的计算步骤。
}