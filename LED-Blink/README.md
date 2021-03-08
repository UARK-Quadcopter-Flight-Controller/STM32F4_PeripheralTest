# LED-Blink

This project is a simple test program that blinks the onboard LED located on pin PB15. The .ioc configurations are as follows:

## Pinout & Configuration
**System Core > GPIO > "LED"**:
- Located on PB15
- GPIO output level: Low
- GPIO mode: Output Push Pull
- GPIO Pull-up/Pull-down: No pull-up and no pull-down
- Maximum output speed: Low
- User Label: LED

## Misc
`blink.h` and `blink.c` were created in `Src` to isolate the code needed for the blink functionality.