/*
 * usb-cdc-print.h
 *
 *  Created on: Mar 10, 2021
 *      Author: Dishoungh
 */

#include "usbd_cdc_if.h"
#include "stdio.h"

#ifndef INC_USB_CDC_PRINT_H_
#define INC_USB_CDC_PRINT_H_

extern uint8_t CDC_Transmit_FS(uint8_t *Buf, uint16_t Len);
void CDC_Print(uint8_t* buf);
void test_print();

#endif /* INC_USB_CDC_PRINT_H_ */
