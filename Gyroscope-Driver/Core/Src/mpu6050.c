/*
 * mpu6050.c
 *
 *  Created on: Mar 10, 2021
 *      Author: Dishoungh
 */

#ifndef SRC_MPU6050_C_
#define SRC_MPU6050_C_

#include "mpu6050.h"

static inline uint8_t mpu6050_init(MPU6050_HandleTypedef* dev)
{
	uint8_t data;

	// Check ID "WHO_AM_I" (Should be 0x68)
	if (HAL_I2C_Mem_Read(dev->i2c, dev->addr, WHO_AM_I_REG, 1, &dev->id, 1, MPU6050_I2C_TIMEOUT) != HAL_OK)
		return 0;

	if (dev->id != MPU6050_ID)
		return 0;

	// Write all 0s to Power Management Register to wake sensor up
	data = 0x00;
	if (HAL_I2C_Mem_Write(dev->i2c, dev->addr, PWR_MGMT_1_REG, 1, &data, 1, MPU6050_I2C_TIMEOUT) != HAL_OK)
		return 0;

	// Set Data Rate of 1KHz
	data = 0x07;
	if (HAL_I2C_Mem_Write(dev->i2c, dev->addr, SMPLRT_DIV_REG, 1, &data, 1, MPU6050_I2C_TIMEOUT) != HAL_OK)
		return 0;

	// Set Accelerometer configuration in ACCEL_CONFIG Register
	// XA_ST=0, YA_ST=0, ZA_ST=0, FS_SEL=0 -> +/- 2g
	data = 0x00;
	if (HAL_I2C_Mem_Write(dev->i2c, dev->addr, ACCEL_CONFIG_REG, 1, &data, 1, MPU6050_I2C_TIMEOUT) != HAL_OK)
		return 0;

	// Set Gyroscopic Configuration in GYRO_CONFIG Register
	// XG_ST=0, YG_ST=0, ZG_ST=0, FS_SEL=0 -> +/- 250 degrees/sec
	data = 0x00;
	if (HAL_I2C_Mem_Write(dev->i2c, dev->addr, GYRO_CONFIG_REG, 1, &data, 1, MPU6050_I2C_TIMEOUT) != HAL_OK)
		return 0;

	return 1;
}

static inline uint8_t mpu6050_read_accel(MPU6050_HandleTypedef* dev)
{
	uint8_t accel_data[6];

	// Read 6 Bytes of Data starting from ACCEL_XOUT_H register

	if (HAL_I2C_Mem_Read(dev->i2c, dev->addr, ACCEL_XOUT_H_REG, 1, accel_data, 6, MPU6050_I2C_TIMEOUT) != HAL_OK)
		return 0;

	// Calculate Raw Accelerometer Values
	dev->accel_x_raw = (int16_t)(accel_data[0] << 8 | accel_data[1]);
	dev->accel_y_raw = (int16_t)(accel_data[2] << 8 | accel_data[3]);
	dev->accel_z_raw = (int16_t)(accel_data[4] << 8 | accel_data[5]);

	// Convert Raw Values
	dev->accel_x_adjusted = (dev->accel_x_raw) / 16384.0;
	dev->accel_y_adjusted = (dev->accel_y_raw) / 16384.0;
	dev->accel_z_adjusted = (dev->accel_z_raw) / 16384.0;

	return 1;
}

static inline uint8_t mpu6050_read_gyro(MPU6050_HandleTypedef* dev)
{
	uint8_t gyro_data[6];

	// Read 6 Bytes of Data starting from Gyro_XOUT_H register

	if (HAL_I2C_Mem_Read(dev->i2c, dev->addr, GYRO_XOUT_H_REG, 1, gyro_data, 6, MPU6050_I2C_TIMEOUT) != HAL_OK)
		return 0;

	// Calculate Raw Gyro Values
	dev->gyro_x_raw = (int16_t)(gyro_data[0] << 8 | gyro_data[1]);
	dev->gyro_y_raw = (int16_t)(gyro_data[2] << 8 | gyro_data[3]);
	dev->gyro_z_raw = (int16_t)(gyro_data[4] << 8 | gyro_data[5]);

	// Convert Raw Gyro Values
	dev->gyro_x_adjusted = (dev->gyro_x_raw) / 131.0;
	dev->gyro_y_adjusted = (dev->gyro_y_raw) / 131.0;
	dev->gyro_z_adjusted = (dev->gyro_z_raw) / 131.0;

	return 1;
}

void test_mpu6050(I2C_HandleTypeDef* i2c)
{
	MPU6050_HandleTypedef mpu6050;

	mpu6050.addr = MPU6050_ADDR;
	mpu6050.i2c = i2c;

	uint8_t pBuf[256];

	//Initialize MPU6050
	while (!(mpu6050_init(&mpu6050)))
	{
		sprintf((char *)pBuf, "MPU6050 Initialization Failed\r\n\n");
		CDC_Print(pBuf);
		HAL_Delay(1000);
	}

	sprintf((char *)pBuf, "MPU6050 Found\r\n\n");
	CDC_Print(pBuf);

	while(1)
	{
		if (mpu6050_read_accel(&mpu6050) && mpu6050_read_gyro(&mpu6050))
		{
			sprintf((char *)pBuf, "Ax: %.2f\r\n"
								  "Ay: %.2f\r\n"
								  "Az: %.2f\r\n"
								  "Gx: %.2f\r\n"
								  "Gy: %.2f\r\n"
								  "Gz: %.2f\r\n\n",
								  mpu6050.accel_x_adjusted,
								  mpu6050.accel_y_adjusted,
								  mpu6050.accel_z_adjusted,
								  mpu6050.gyro_x_adjusted,
								  mpu6050.gyro_y_adjusted,
								  mpu6050.gyro_z_adjusted);

			CDC_Print(pBuf);

		}
		else
		{
			sprintf((char *)pBuf, "Failed to Get MPU6050 Values\r\n\n");
			CDC_Print(pBuf);
		}

		HAL_Delay(1000);

	}
}

#endif /* SRC_MPU6050_C_ */
