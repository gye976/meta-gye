#ifndef MPU6050_H
#define MPU6050_H

#include <endian.h>
#include <math.h>
#include <stdint.h>

#define INT_ENABLE	0x38
#define INT_STATUS	0x3A
#define FIFO_OVERFLOW_BIT	0x10

#define MPU6050_ADDR 0x68 // MPU6050 I2C address
#define PWR_MGMT_1 0x6B // Power Management 1 register
#define CONFIG 0x1A // DLPF Configuration register
#define SMPLRT_DIV 0x19

#define USER_CTRL      0x6A
#define FIFO_EN        0x23
#define FIFO_R_W       0x74

#define FIFO_COUNT_H 0x72
#define FIFO_COUNT_L 0x73

#define GYRO_CONFIG 0x1B // Gyroscope Configuration register
#define ACCEL_CONFIG 0x1C // Accelerometer Configuration register
#define ACCEL_XOUT_H 0x3B // Accelerometer X-axis high byte
#define ACCEL_XOUT_L 0x3C // Accelerometer X-axis low byte
#define GYRO_XOUT_H 0x43 // Gyroscope X-axis high byte
#define GYRO_XOUT_L 0x44 // Gyroscope X-axis low byte

#define RADIANS_TO_DEGREES 		((float)(180/3.14159))

#define ACC_FS_SENSITIVITY		4096.0f 
#define GYRO_FS_SENSITIVITY		65.536f

#define CF_ALPHA                   0.96f
#define A_ACC	0.3f
#define A_GYRO	0.5f

typedef struct mpu6050 {
	/* current value */
	float angle[3];
	float gyro[3];

	float gyro_bias[3];
	float alpha;

	float dt;
	int fd;

} mpu6050_t;

enum enum_axis {
	X, Y, Z
};

enum enum_angle {
	PITCH, ROLL, YAW
};

//'be16toh' only considers the bit, does not guarantee the sign.
static inline int16_t be16toh_s(int16_t be_val)
{
    return (int16_t)be16toh(be_val);
}

int mpu6050_init(mpu6050_t *mpu6050);
void mpu6050_calibrate(mpu6050_t *mpu6050);
void mpu6050_read_raw(mpu6050_t *mpu6050, float acc[], float gyro[]);

void mpu6050_gyro_bias(mpu6050_t *mpu6050, float *gyro);

void mpu6050_acc_to_angle(float acc[], float new_acc_angle[]);
void mpu6050_gyro_to_angle(mpu6050_t *mpu6050, float gyro[], float gyro_angle[]);

void do_EMA(float alpha, float *data, float new_data);
void do_complementary_filter(float acc_angle[], float gyro_angle[], float angle[]);
void mpu6050_calc_angle(mpu6050_t *mpu6050);


#endif 

