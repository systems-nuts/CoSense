#ifndef MADGWICK_IMU_H
#define MADGWICK_IMU_H

// Type Definitions
typedef float bmx055xAcceleration;
typedef float bmx055yAcceleration;
typedef float bmx055zAcceleration;
typedef float bmx055xAngularRate;
typedef float bmx055yAngularRate;
typedef float bmx055zAngularRate;
typedef float bmx055xMagneto;
typedef float bmx055yMagneto;
typedef float bmx055zMagneto;

// Function Declarations
void MadgwickAHRSupdateIMU(bmx055xAngularRate gx, bmx055yAngularRate gy, bmx055zAngularRate gz,
                           bmx055xAcceleration ax, bmx055yAcceleration ay, bmx055zAcceleration az,
                           float* q0_ptr, float* q1_ptr, float* q2_ptr, float* q3_ptr);

#endif // MADGWICK_IMU_H
