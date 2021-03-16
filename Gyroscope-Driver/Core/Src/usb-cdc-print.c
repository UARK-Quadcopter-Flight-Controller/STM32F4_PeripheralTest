/*
 * usb-cdc-print.c
 *
 *  Created on: Mar 10, 2021
 *      Author: Dishoungh
 */

#include "usb-cdc-print.h"
#include "led-blink.h"

void CDC_Print(uint8_t* buf)
{
	CDC_Transmit_FS(buf, strlen(buf));
	memset(buf, 0, sizeof(buf));
}

void test_print()
{
	// Create a buffer
	char txBuf[8];
	uint8_t count = 1;

	while (1)
	{
		sprintf(txBuf, "%u\r\n", count);
		count++;

		if(count > 100)
		{
		  count = 1;
		}

		CDC_Transmit_FS((uint8_t *) txBuf, strlen(txBuf));
		blinkLED();
	}
}
