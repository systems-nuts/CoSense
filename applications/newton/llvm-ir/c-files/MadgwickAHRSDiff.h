

// Define the precision of the gyroscope sensors (Gyroscope) data types
#define FRAC_Q_GYRO_X 16  // Precision bits for x-axis gyroscope data
#define FRAC_Q_GYRO_Y 16  // Precision bits for y-axis gyroscope data
#define FRAC_Q_GYRO_Z 16  // Precision bits for z-axis gyroscope data

#define FRAC_BASE_GYRO_X (1 << FRAC_Q_GYRO_X)  // 2^16, base value for x-axis gyroscope
#define FRAC_BASE_GYRO_Y (1 << FRAC_Q_GYRO_Y)  // 2^16, base value for y-axis gyroscope
#define FRAC_BASE_GYRO_Z (1 << FRAC_Q_GYRO_Z)  // 2^16, base value for z-axis gyroscope

// Define the precision of the accelerometer sensors (Acceleration) data types
#define FRAC_Q_ACCEL_X 12  // Precision bits for x-axis acceleration data
#define FRAC_Q_ACCEL_Y 12  // Precision bits for y-axis acceleration data
#define FRAC_Q_ACCEL_Z 12  // Precision bits for z-axis acceleration data

#define FRAC_BASE_ACCEL_X (1 << FRAC_Q_ACCEL_X)  // 2^12, base value for x-axis acceleration
#define FRAC_BASE_ACCEL_Y (1 << FRAC_Q_ACCEL_Y)  // 2^12, base value for y-axis acceleration
#define FRAC_BASE_ACCEL_Z (1 << FRAC_Q_ACCEL_Z)  // 2^12, base value for z-axis acceleration

// Define the precision of the magnetometer sensors (Magnetometer) data types
#define FRAC_Q_MAG_X 13  // Precision bits for x-axis magnetometer data
#define FRAC_Q_MAG_Y 13  // Precision bits for y-axis magnetometer data
#define FRAC_Q_MAG_Z 15  // Precision bits for z-axis magnetometer data

#define FRAC_BASE_MAG_X (1 << FRAC_Q_MAG_X)  // 2^13, base value for x-axis magnetometer
#define FRAC_BASE_MAG_Y (1 << FRAC_Q_MAG_Y)  // 2^13, base value for y-axis magnetometer
#define FRAC_BASE_MAG_Z (1 << FRAC_Q_MAG_Z)  // 2^15, base value for z-axis magnetometer


//void	MadgwickAHRSupdate(int32_t gx, int32_t gy, int32_t gz, int32_t ax, int32_t ay, int32_t az, int32_t mx, int32_t my, int32_t mz,
//			int32_t* q0_ptr, int32_t* q1_ptr, int32_t* q2_ptr, int32_t* q3_ptr);


void MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz,
			float* q0_ptr, float* q1_ptr, float* q2_ptr, float* q3_ptr);
void	MadgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az,
			float* q0_ptr, float* q1_ptr, float* q2_ptr, float* q3_ptr);


