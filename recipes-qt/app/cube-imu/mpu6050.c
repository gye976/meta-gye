#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <endian.h>
#include <linux/i2c-dev.h>

#include "mpu6050.h"

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define OPEN_FD(fd, path, args...)                  \
do                                          \
{                                           \
	fd = open(path, args);         \
                                                \
	if (unlikely(fd == -1))                 \
	{                                       \
		perror("open");                     \
		fprintf(stderr, "path:%s\n", path); \
	}                                       \
} while (0)

int mpu6050_init(mpu6050_t *mpu6050)
{
	memset(mpu6050, 0, sizeof(mpu6050_t));

	int fd;
	OPEN_FD(fd, "/dev/i2c-1", O_RDWR);

	mpu6050->fd = fd;

    // Set the I2C address for the MPU6050
    if (ioctl(fd, I2C_SLAVE, MPU6050_ADDR) < 0) {
        perror("Failed to connect to MPU6050 sensor");
        exit(1);
    }

    // reset device
    char config[2] = {PWR_MGMT_1, 0x40};
    if (write(fd, config, 2) != 2) {
        perror("Failed to reset MPU6050");
        exit(1);
    }

    // Initialize MPU6050: Set Power Management 1 register to 0 to wake up the sensor
    config[1] = 0;
    if (write(fd, config, 2) != 2) {
        perror("Failed to initialize MPU6050");
        exit(1);
    }

    // Set DLPF: 21Hz bandwidth (Configuration 2)
    char dlpf_config[2] = {CONFIG, 0x04}; // 0x03 sets DLPF to Configuration 2
    if (write(fd, dlpf_config, 2) != 2) {
        perror("Failed to set DLPF");
        exit(1);
    }

    // Set Gyroscope sensitivity: ±500°/s (Configuration 0)
    char gyro_config[2] = {GYRO_CONFIG, 0x08}; 
    if (write(fd, gyro_config, 2) != 2) {
        perror("Failed to set gyroscope configuration");
        exit(1);
    }

    // Set Accelerometer sensitivity: ±8g (Configuration 0)
    char accel_config[2] = {ACCEL_CONFIG, 0x10}; 
    if (write(fd, accel_config, 2) != 2) {
        perror("Failed to set accelerometer configuration");
        exit(1);
    }

    // Set sample rate divider for 1 kHz sample rate
    char smplrt_div_config[2] = {SMPLRT_DIV, 0}; // 0 sets sample rate to 1 kHz
    if (write(fd, smplrt_div_config, 2) != 2) {
        perror("Failed to set sample rate divider");
        exit(1);
	}
	
	mpu6050_calibrate(mpu6050);

	return fd;
}

#define N 1000
#define A 0.8
void mpu6050_calibrate(mpu6050_t *mpu6050)
{
	float new_gyro[3] = { 0, };
	float gyro[3] = { 0, };

	float new_acc[3] = { 0, };
	float new_acc_angle[3] = { 0, };
	float acc_angle[3] = { 0, };

	for (int i = 0; i < N; i++) {
		mpu6050_read_raw(mpu6050, new_acc, new_gyro);
		mpu6050_acc_to_angle(new_acc, new_acc_angle);

		for (int j = 0; j < 3; j++) {
			gyro[j] = (1.0f - A) * gyro[j] + A * new_gyro[j];
		}
		acc_angle[0] = (1.0f - A) * acc_angle[0] + A * new_acc_angle[0];
		acc_angle[1] = (1.0f - A) * acc_angle[1] + A * new_acc_angle[1];
	}

	for (int i = 0; i < 3; i++) {
		mpu6050->gyro_bias[i] = gyro[i];
	}
	mpu6050->angle[0] = acc_angle[0];
	mpu6050->angle[1] = acc_angle[1];

	printf("gyro bias: %f, %f, %f\n", mpu6050->gyro_bias[0], mpu6050->gyro_bias[1], mpu6050->gyro_bias[2]);
	printf("initial angle by acc: %f, %f, %f\n", mpu6050->angle[0], mpu6050->angle[1], mpu6050->angle[2]);
	printf("\n");
}

void mpu6050_read_raw(mpu6050_t *mpu6050, float acc[], float gyro[])
{
	char data[14];
	
	char reg[1] = {ACCEL_XOUT_H};

	int ret;		
	ret = write(mpu6050->fd, reg, 1);
	if (unlikely(ret != 1)) {
		perror("read_raw, write err");
		exit(1);
	}
		
	ret = read(mpu6050->fd, data, 14);
	if (unlikely(ret != 14)) {
		perror("read_raw, read err");
		exit(1);
	}

	int16_t accel_x = (data[0] << 8) | data[1];
	int16_t accel_y = (data[2] << 8) | data[3];
	int16_t accel_z = (data[4] << 8) | data[5];

	int16_t gyro_x = (data[8] << 8) | data[9];
	int16_t gyro_y = (data[10] << 8) | data[11];
	int16_t gyro_z = (data[12] << 8) | data[13];

	acc[X] = (float)accel_x / ACC_FS_SENSITIVITY;
	acc[Y] = (float)accel_y / ACC_FS_SENSITIVITY;
	acc[Z] = (float)accel_z / ACC_FS_SENSITIVITY;

	gyro[X] = (float)gyro_x / GYRO_FS_SENSITIVITY;
	gyro[Y] = (float)gyro_y / GYRO_FS_SENSITIVITY;
	gyro[Z] = (float)gyro_z / GYRO_FS_SENSITIVITY;

	// printf("%d, %d, %d\n", (int)accel_x, (int)accel_y, (int)accel_z);
	// printf("%d, %d, %d\n", (int)gyro_x, (int)gyro_y, (int)gyro_z);
}

void mpu6050_gyro_bias(mpu6050_t *mpu6050, float *gyro)
{
	float *bias = mpu6050->gyro_bias;

	for (int i = 0; i < 3; i++) {
		gyro[i] -= bias[i];
	}
}		

void mpu6050_gyro_to_angle(mpu6050_t *mpu6050, float gyro[], float gyro_angle[])
{
	for (int i = 0; i < 3; i++) {
		//
		gyro_angle[i] += gyro[i] * mpu6050->dt;
	}
}

void mpu6050_acc_to_angle(float acc[], float new_acc_angle[])
{
	if (unlikely(sqrt(pow(acc[X],2) + pow(acc[Z],2))) == 0) {
		printf("nan\n");
		exit(1);
	}
	if (unlikely(sqrt(pow(acc[Y],2) + pow(acc[Z],2))) == 0) {
		printf("nan\n");
		exit(1);
	}

	new_acc_angle[PITCH] = atan(acc[Y] / sqrt(pow(acc[X],2) + pow(acc[Z],2))) \
		* RADIANS_TO_DEGREES;

	new_acc_angle[ROLL] = atan(-1 * acc[X] / sqrt(pow(acc[Y], 2) + pow(acc[Z],2))) \
		* RADIANS_TO_DEGREES;
}

void do_EMA(float alpha, float *data, float new_data)
{
	*data = alpha * new_data + (1.0f - alpha) * (*data);
}

void do_complementary_filter(float acc_angle[], float gyro_angle[], float angle[])
{
	for (int i = 0; i < 2; i++) {
		angle[i] = (1.0 - CF_ALPHA) * acc_angle[i] + CF_ALPHA * gyro_angle[i];
	}
}

void mpu6050_calc_angle(mpu6050_t *mpu6050)
{
	float *angle = mpu6050->angle;
	float *gyro = mpu6050->gyro;

	float acc_angle[3] = {
		angle[0], angle[1], angle[2],
	};
	float gyro_angle[3] = {
		angle[0], angle[1], angle[2],
	};

	float new_acc[3] = { 0, };
	float new_acc_angle[3] = { 0, };

	float new_gyro[3] = { 0, };

	mpu6050_read_raw(mpu6050, new_acc, new_gyro);

	/* gyro */
	mpu6050_gyro_bias(mpu6050, new_gyro);

	for (int i = 0; i < 3; i++) {
		do_EMA(A_GYRO, &gyro[i], new_gyro[i]);
	}
    // for (int i = 0; i < 3; i++) {
    //     printf("gyro: %f, ", gyro[i]);
    // }
    // printf("\n");
	
	mpu6050_gyro_to_angle(mpu6050, gyro, gyro_angle);

	/* acc */
	mpu6050_acc_to_angle(new_acc, new_acc_angle);

	for (int i = 0; i < 2; i++) {
		do_EMA(A_ACC, &acc_angle[i], new_acc_angle[i]);
	}

	/**/
	do_complementary_filter(acc_angle, gyro_angle, angle);

	mpu6050->angle[YAW] = gyro_angle[YAW];

    // for (int i = 0; i < 3; i++) {
    //     printf("gyro_angle: %f, ", gyro_angle[i]);
    // }
    // printf("\n");
    // for (int i = 0; i < 3; i++) {
    //     printf("acc_angle: %f, ", acc_angle[i]);
    // }
    // printf("\n");
    // for (int i = 0; i < 3; i++) {
    //     printf("angle: %f, ", angle[i]);
    // }
    // printf("\n\n");
}
