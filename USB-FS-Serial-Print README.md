# USB-FS-Serial-Print
This project is a simple test program that enables the USB FS interface so that print messages can be sent over the USB interface and appear as a serial device to a computer. The external oscillator is required to use the USB interface (else the math doesn't work out in the clock configurator). The .ioc configurations are as follows:

## Pinout & Configuration
**System Core > Connectivity > USB_OTG_FS**:
- Mode: Device Only

**System Core > RCC**:
- High Speed Clock (HSE): Crystal/Ceramic Resonator
- Low Speed Clock (LSE): Disable
- Master Clock Output 1: Unchecked
- Master Clock Output 2: Unchecked
- Audio Clock Input (I2S_CKIN): Unchecked

**Middleware > USB_Device**:
- Class for FS IP: Communication Device Class (Virtual Port Com)

## Clock Configuration
- Changed the input frequency from default 25 MHz value to 16.
- Changed the PLL Source Mux to HSE.
- HCLK was changed to the max 168 MHz.

*From here, pressing enter prompted a clock solver wizard, which ran to calculate values for me.*


## Misc
`blink.h` and `blink.c` were created in `Src` to isolate the code needed for the blink functionality. The RCC/HSE code is auto-generated as a part of saving the .ioc file.