
#ifndef BOARD_CUSTOM_H
#define BOARD_CUSTOM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"

// LEDs definitions for PCA10040
// #define LEDS_NUMBER    4

// #define LED_START      17
// #define LED_1          17
// #define LED_2          18
// #define LED_3          19
// #define LED_4          20
// #define LED_STOP       20

// #define LEDS_ACTIVE_STATE 0

// #define LEDS_INV_MASK  LEDS_MASK

// #define LEDS_LIST { LED_1, LED_2, LED_3, LED_4 }

// #define BSP_LED_0      LED_1
// #define BSP_LED_1      LED_2
// #define BSP_LED_2      LED_3
// #define BSP_LED_3      LED_4

#define BUTTONS_NUMBER 1

//#define BUTTON_START   13
#define BUTTON_1       0xFF
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP
#define BUTTONS_ACTIVE_STATE 0
#define BUTTONS_LIST {BUTTON_1}

// DFU Pins
#define RX_PIN_NUMBER  12
#define TX_PIN_NUMBER  11
#define CTS_PIN_NUMBER 0xFF
#define RTS_PIN_NUMBER 0xFF
#define HWFC           false

#ifdef __cplusplus
}
#endif

#endif // PCA10040_H
