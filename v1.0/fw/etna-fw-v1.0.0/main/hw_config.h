
#ifndef __hw_cfg_h
#define __hw_cfg_h

#include "nrf_gpio.h" 

// UART
#define APP_UART_TXD_IO				27

// SPI0
#define APP_SPI0_MOSI					8
#define APP_SPI0_MISO					6
#define APP_SPI0_SCK					5

// SPI1
#define APP_SPI1_MOSI					26
#define APP_SPI1_SCK					10

// LED IOs
#define LED0_IO								19
#define LED0_TRIS()						nrf_gpio_cfg_output(LED0_IO)
#define LED0_ON()							nrf_gpio_pin_set(LED0_IO)
#define LED0_OFF()						nrf_gpio_pin_clear(LED0_IO)
#define LED0_TOG()						nrf_gpio_pin_toggle(LED0_IO)

// BUT
#define BUT0_IO								13
#define BUT0_TRIS()						nrf_gpio_cfg_input(BUT0_IO, NRF_GPIO_PIN_PULLUP)
#define BUT0_STATE()	 				nrf_gpio_pin_read(BUT0_IO)

// BUT
#define BUT1_IO								17
#define BUT1_TRIS()						nrf_gpio_cfg_input(BUT1_IO, NRF_GPIO_PIN_PULLUP)
#define BUT1_STATE() 					nrf_gpio_pin_read(BUT1_IO)

// OLED	
#define OLED_RST_IO						14
#define OLED_RST_TRIS()				nrf_gpio_cfg_output(OLED_RST_IO)	
#define OLED_RST_ASSERT()			nrf_gpio_pin_clear(OLED_RST_IO)
#define OLED_RST_DEASSERT()		nrf_gpio_pin_toggle(OLED_RST_IO)

#define OLED_CSN_IO						12
#define OLED_CSN_TRIS()				nrf_gpio_cfg_output(OLED_CSN_IO)	
#define OLED_CSN_ASSERT()			nrf_gpio_pin_clear(OLED_CSN_IO)
#define OLED_CSN_DEASSERT()		nrf_gpio_pin_toggle(OLED_CSN_IO)

#define OLED_DC_IO						9
#define OLED_DC_TRIS()				nrf_gpio_cfg_output(OLED_DC_IO)	
#define OLED_DC_DATA()				nrf_gpio_pin_clear(OLED_DC_IO)
#define OLED_DC_COMMAND()			nrf_gpio_pin_toggle(OLED_DC_IO)

// HEATER
#define HEAT_PWM_IO						31
#define HEAT_PWM_TRIS()				nrf_gpio_cfg_output(HEAT_PWM_IO)	
#define HEAT_PWM_ON()					nrf_gpio_pin_set(HEAT_PWM_IO)
#define HEAT_PWM_OFF()				nrf_gpio_pin_clear(HEAT_PWM_IO)

#define HEAT_EN_IO						29
#define HEAT_EN_TRIS()				nrf_gpio_cfg_output(HEAT_EN_IO)	
#define HEAT_EN_ON()					nrf_gpio_pin_set(HEAT_EN_IO)
#define HEAT_EN_OFF()					nrf_gpio_pin_clear(HEAT_EN_IO)

#define HEAT_VOLTAGE_ANA			NRF_SAADC_INPUT_AIN4 // ANA4
#define HEAT_CURRENT_ANA			NRF_SAADC_INPUT_AIN6 // ANA6

// ADS1118
#define ADS1118_CS_IO					7
#define ADS1118_CS_TRIS()			nrf_gpio_cfg_output(ADS1118_CS_IO)	
#define ADS1118_CS_ASSERT()		nrf_gpio_pin_clear(ADS1118_CS_IO)
#define ADS1118_CS_DEASSERT()	nrf_gpio_pin_toggle(ADS1118_CS_IO)

#define ADS1118_DRDY_IO				APP_SPI0_MISO
#define ADS1118_DRDY_STATE() 	nrf_gpio_pin_read(ADS1118_DRDY_IO)


// SOUNDER
#define SOUNDER_IO						25








void APP_ScheduleCccdCheck (void); 

#endif /* __stdint_h */

