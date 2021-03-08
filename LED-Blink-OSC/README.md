# LED-Blink-OSC

This project is a simple test program that blinks the onboard LED located on pin PB15 while also verifying that the external crystal osciallator is functioning as intended. The .ioc configurations are as follows:

## Pinout & Configuration
**System Core > GPIO > "LED"**:
- Located on PB15
- GPIO output level: Low
- GPIO mode: Output Push Pull
- GPIO Pull-up/Pull-down: No pull-up and no pull-down
- Maximum output speed: Low
- User Label: LED


**System Core > RCC**:
- High Speed Clock (HSE): Crystal/Ceramic Resonator
- Low Speed Clock (LSE): Disable
- Master Clock Output 1: Unchecked
- Master Clock Output 2: Unchecked
- Audio Clock Input (I2S_CKIN): Unchecked

## Clock Configuration
- Changed the input frequency from default 25 MHz value to 16.
- Changed the PLL Source Mux to HSE.
- HCLK was changed to the max 168 MHz.

*From here, pressing enter prompted a clock solver wizard, which ran to calculate values for me.*

## Misc
`blink.h` and `blink.c` were created in `Src` to isolate the code needed for the blink functionality. The RCC/HSE code is auto-generated as a part of saving the .ioc file.