/*
Authored 2021, Orestis Kaparounakis.

All rights reserved.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
						     BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
						     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
    */

#ifndef MADGWICK_AHRS_FIX_H
#define MADGWICK_AHRS_FIX_H

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

// 定义定点数精度为2位
#define FRAC_Q				2
#define K   				(1 << (FRAC_Q - 1))
#define FRAC_BASE			(1 << FRAC_Q)     // FRAC_BASE = 4
#define DEC2FRAC(_d)		((int8_t)(_d * FRAC_BASE))  // 将小数转换为定点数格式
#define DISPLAY_INT(_x)		((_x >> FRAC_Q))   // 显示定点数的整数部分
#define DISPLAY_FRAC(_y)	(((FRAC_BASE - 1) & _y) * (10000 / FRAC_BASE))  // 显示定点数的小数部分
#define FORMAT_FIXP			"%s%d.%04d"  // 格式化输出定点数

// 四元数更新函数
void	MadgwickAHRSupdate(int8_t gx, int8_t gy, int8_t gz, int8_t ax, int8_t ay, int8_t az, int8_t mx, int8_t my, int8_t mz,
			int8_t* q0_ptr, int8_t* q1_ptr, int8_t* q2_ptr, int8_t* q3_ptr);

// IMU-only更新函数
void	MadgwickAHRSupdateIMU(int8_t gx, int8_t gy, int8_t gz, int8_t ax, int8_t ay, int8_t az,
		   int8_t* q0_ptr, int8_t* q1_ptr, int8_t* q2_ptr, int8_t* q3_ptr);

// 计算平方根或倒数平方根
int8_t	sqrt_rsqrt(int8_t x, int recip);

// 定点乘法函数 (内联优化)
inline int8_t fixmul(int8_t x, int8_t y) {
// 定点乘法后将结果右移以保持精度
return ((int16_t)x * y) >> FRAC_Q;
}

#endif /* MADGWICK_AHRS_FIX_H */
