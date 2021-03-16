/*
 * mpu6050.h
 *
 *  Created on: Mar 10, 2021
 *      Author: Dishoungh
 */

#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_

#include "stm32f4xx_hal.h"
#include "usb-cdc-print.h"
#include "stdio.h"

#define MPU6050_ADDR 		0xD0
#define MPU6050_ID			0x68
#define MPU6050_I2C_TIMEOUT	0x03E8

#define SMPLRT_DIV_REG 		0x19
#define GYRO_CONFIG_REG		0x1B
#define ACCEL_CONFIG_REG	0x1C
#define ACCEL_XOUT_H_REG	0x3B
#define TEMP_OUT_H_REG		0x41
#define	GYRO_XOUT_H_REG		0x43
#define	PWR_MGMT_1_REG		0x6B
#define WHO_AM_I_REG		0x75

typedef struct
{
	int16_t accel_x_raw;
	float accel_x_adjusted;
	int16_t accel_y_raw;
	float accel_y_adjusted;
	int16_t accel_z_raw;
	float accel_z_adjusted;

	int16_t gyro_x_raw;
	float gyro_x_adjusted;
	int16_t gyro_y_raw;
	float gyro_y_adjusted;
	int16_t gyro_z_raw;
	float gyro_z_adjusted;

    uint16_t addr;

    I2C_HandleTypeDef* i2c;

    uint8_t  id;        /* Chip ID */

} MPU6050_HandleTypedef;

static inline uint8_t mpu6050_init(MPU6050_HandleTypedef* dev);
static inline uint8_t mpu6050_read_accel(MPU6050_HandleTypedef* dev);
static inline uint8_t mpu6050_read_gyro(MPU6050_HandleTypedef* dev);
void test_mpu6050(I2C_HandleTypeDef* i2c);

#endif /* INC_MPU6050_H_ */
