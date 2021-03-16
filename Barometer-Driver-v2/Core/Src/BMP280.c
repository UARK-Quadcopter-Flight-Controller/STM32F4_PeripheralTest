/**
 * Ciastkolog.pl (https://github.com/ciastkolog)
 *
*/
/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 sheinz (https://github.com/sheinz)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "bmp280.h"
#include "math.h"
#include "stdio.h"
#include "main.h"

void bmp280_init_default_params(bmp280_params_t *params)
{
	params->mode = BMP280_MODE_NORMAL;
	params->filter = BMP280_FILTER_OFF;
	params->oversampling_pressure = BMP280_STANDARD;
	params->oversampling_temperature = BMP280_STANDARD;
	params->oversampling_humidity = BMP280_STANDARD;
	params->standby = BMP280_STANDBY_250;
}

static inline int read_data(BMP280_HandleTypedef *dev, uint8_t addr, uint8_t* value, uint16_t len)
{
	addr |= 0x80;	//Bit 7 must be asserted to read
	HAL_GPIO_WritePin(BARO_NCS_GPIO_Port, BARO_NCS_Pin, GPIO_PIN_RESET); //Deassert Slave Select
	HAL_StatusTypeDef tx = HAL_SPI_Transmit(dev->spi, &addr, 1, BMP280_SPI_TIMEOUT); //Transmit address to receive from
	HAL_StatusTypeDef rx = HAL_SPI_Receive(dev->spi, value, len, BMP280_SPI_TIMEOUT);  //Receive data byte(s)
	HAL_GPIO_WritePin(BARO_NCS_GPIO_Port, BARO_NCS_Pin, GPIO_PIN_SET); //Assert Slave Select

	if ((tx == HAL_OK) && (rx == HAL_OK))
		return 0;
	else
		return 1;
}

static bool read_register16(BMP280_HandleTypedef *dev, uint8_t addr, uint16_t *value)
{
	uint8_t* rxBuf[2];

	addr |= 0x80;	//Bit 7 must be asserted to read
	HAL_GPIO_WritePin(BARO_NCS_GPIO_Port, BARO_NCS_Pin, GPIO_PIN_RESET); 	//Deassert Slave Select
	HAL_StatusTypeDef tx = HAL_SPI_Transmit(dev->spi, &addr, 1, BMP280_SPI_TIMEOUT); //Transmit address to receive from
	HAL_StatusTypeDef rx = HAL_SPI_Receive(dev->spi, rxBuf, 2, BMP280_SPI_TIMEOUT);  //Receive data bytes
	HAL_GPIO_WritePin(BARO_NCS_GPIO_Port, BARO_NCS_Pin, GPIO_PIN_SET); 	    //Assert Slave Select

	if ((tx == HAL_OK) && (rx == HAL_OK))
	{
		*value = (uint16_t) ((*rxBuf[1] << 8) | *rxBuf[0]);
		return true;
	}
	else
		return false;
}

static int write_register8(BMP280_HandleTypedef *dev, uint8_t addr, uint8_t value)
{
	addr &= 0x7F;  //Bit 7 must be deasserted to write
	uint8_t txBuf[2];
	txBuf[0] = addr;
	txBuf[1] = value;

	HAL_GPIO_WritePin(BARO_NCS_GPIO_Port, BARO_NCS_Pin, GPIO_PIN_RESET);
	HAL_StatusTypeDef tx = HAL_SPI_Transmit(dev->spi, txBuf, 2, BMP280_SPI_TIMEOUT);
	HAL_GPIO_WritePin(BARO_NCS_GPIO_Port, BARO_NCS_Pin, GPIO_PIN_SET);

	if (tx == HAL_OK)
		return false;
	else
		return true;
}

static bool read_calibration_data(BMP280_HandleTypedef *dev)
{

	if (read_register16(dev, 0x89, &(dev->dig_T1)) &&
		read_register16(dev, 0x8b, (uint16_t *) &dev->dig_T2) &&
		read_register16(dev, 0x8d, (uint16_t *) &dev->dig_T3) &&
		read_register16(dev, 0x8f, &dev->dig_P1) &&
		read_register16(dev, 0x91, (uint16_t *) &dev->dig_P2) &&
		read_register16(dev, 0x93, (uint16_t *) &dev->dig_P3) &&
		read_register16(dev, 0x95, (uint16_t *) &dev->dig_P4) &&
		read_register16(dev, 0x97, (uint16_t *) &dev->dig_P5) &&
		read_register16(dev, 0x99, (uint16_t *) &dev->dig_P6) &&
		read_register16(dev, 0x9b, (uint16_t *) &dev->dig_P7) &&
		read_register16(dev, 0x9d, (uint16_t *) &dev->dig_P8) &&
		read_register16(dev, 0x9f, (uint16_t *) &dev->dig_P9))
	{
		return true;
	}

	return false;
}

bool bmp280_init(BMP280_HandleTypedef *dev, bmp280_params_t *params)
{
	if (read_data(dev, BMP280_REG_ID, &(dev->id), 1))
		return false;

	if (dev->id != BMP280_CHIP_ID)
		return false;

	// Soft reset.
	if (write_register8(dev, BMP280_REG_RESET, BMP280_RESET_VALUE))
		return false;

	// Wait until finished copying over the NVP data.
	while (1)
	{
		uint8_t status;
		if (!read_data(dev, BMP280_REG_STATUS, &status, 1) && (status & 1) == 0)
			break;
	}

	if (!read_calibration_data(dev))
		return false;

	uint8_t config = (params->standby << 5) | (params->filter << 2);

	if (write_register8(dev, BMP280_REG_CONFIG, config))
		return false;

	if (params->mode == BMP280_MODE_FORCED)
		params->mode = BMP280_MODE_SLEEP;  // initial mode for forced is sleep

	uint8_t ctrl = (params->oversampling_temperature << 5)
			| (params->oversampling_pressure << 2) | (params->mode);

	if (write_register8(dev, BMP280_REG_CTRL, ctrl))
		return false;

	return true;
}

/**
 * Compensation algorithm is taken from BMP280 datasheet.
 *
 * Return value is in degrees Celsius.
 */
static inline int32_t compensate_temperature(BMP280_HandleTypedef *dev, int32_t adc_temp, int32_t *fine_temp)
{
	int32_t var1, var2;

	var1 = ((((adc_temp >> 3) - ((int32_t) dev->dig_T1 << 1)))
			* (int32_t) dev->dig_T2) >> 11;
	var2 = (((((adc_temp >> 4) - (int32_t) dev->dig_T1)
			* ((adc_temp >> 4) - (int32_t) dev->dig_T1)) >> 12)
			* (int32_t) dev->dig_T3) >> 14;

	*fine_temp = var1 + var2;
	return (*fine_temp * 5 + 128) >> 8;
}

/**
 * Compensation algorithm is taken from BMP280 datasheet.
 *
 * Return value is in Pa, 24 integer bits and 8 fractional bits.
 */
static inline uint32_t compensate_pressure(BMP280_HandleTypedef *dev, int32_t adc_press, int32_t fine_temp)
{
	int64_t var1, var2, p;

	var1 = (int64_t) fine_temp - 128000;
	var2 = var1 * var1 * (int64_t) dev->dig_P6;
	var2 = var2 + ((var1 * (int64_t) dev->dig_P5) << 17);
	var2 = var2 + (((int64_t) dev->dig_P4) << 35);
	var1 = ((var1 * var1 * (int64_t) dev->dig_P3) >> 8)
			+ ((var1 * (int64_t) dev->dig_P2) << 12);
	var1 = (((int64_t) 1 << 47) + var1) * ((int64_t) dev->dig_P1) >> 33;

	if (var1 == 0)
	{
		return 0;  // avoid exception caused by division by zero
	}

	p = 1048576 - adc_press;
	p = (((p << 31) - var2) * 3125) / var1;
	var1 = ((int64_t) dev->dig_P9 * (p >> 13) * (p >> 13)) >> 25;
	var2 = ((int64_t) dev->dig_P8 * p) >> 19;

	p = ((p + var1 + var2) >> 8) + ((int64_t) dev->dig_P7 << 4);
	return p;
}

bool bmp280_read_fixed(BMP280_HandleTypedef *dev, int32_t *temperature, uint32_t *pressure)
{
	int32_t adc_pressure;
	int32_t adc_temp;
	uint8_t data[8];

	// Need to read in one sequence to ensure they match.
	if (read_data(dev, 0xf7, data, 6))
		return false;

	adc_pressure = data[0] << 12 | data[1] << 4 | data[2] >> 4;
	adc_temp = data[3] << 12 | data[4] << 4 | data[5] >> 4;

	int32_t fine_temp;
	*temperature = compensate_temperature(dev, adc_temp, &fine_temp);
	*pressure = compensate_pressure(dev, adc_pressure, fine_temp);

	return true;
}

bool bmp280_read_float(BMP280_HandleTypedef *dev, float *temperature, float *pressure, float *altitude)
{
	int32_t fixed_temperature;
	uint32_t fixed_pressure;
	if (bmp280_read_fixed(dev, &fixed_temperature, &fixed_pressure))
	{
		*temperature = (float) fixed_temperature / 140;
		*pressure = (float) fixed_pressure / 360;

		/*Calculating Altitude from Pressure and Temperature*/
		if (*pressure)
			*altitude = (float) (((powf((101325 / *pressure), (1 / 5.257)) - 1) * (*temperature + 273.15)) / 0.0065);
		else
			*altitude = (float) 0;

		return true;
	}

	return false;
}

bool bmp280_is_measuring(BMP280_HandleTypedef *dev)
{
	uint8_t status;
	if (read_data(dev, BMP280_REG_STATUS, &status, 1))
		return false;
	if (status & (1 << 3))
		return true;
	return false;
}
