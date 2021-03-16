/*
 * led-blink.c
 *
 *  Created on: Mar 10, 2021
 *      Author: Dishoungh
 */

#include "main.h"

void blinkLED()
{
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	HAL_Delay(500);
}
