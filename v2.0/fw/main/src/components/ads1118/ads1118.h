#ifndef __ADS1118_H
#define __ADS1118_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>



typedef enum {
	ADS1118_MUX_AIN0_P_AIN1_N = 0,
	ADS1118_MUX_AIN0_P_AIN3_N = 1,
	ADS1118_MUX_AIN1_P_AIN3_N = 2,
	ADS1118_MUX_AIN2_P_AIN3_N = 3,
	ADS1118_MUX_AIN0_P_GND_N = 4,
	ADS1118_MUX_AIN1_P_GND_N = 5,
	ADS1118_MUX_AIN2_P_GND_N = 6,
	ADS1118_MUX_AIN3_P_GND_N = 7,
} ads1118_config_mux_t;  

typedef enum {
	ADS1118_FSR_6144 = 0,
	ADS1118_FSR_4096 = 1,
	ADS1118_FSR_2048 = 2,
	ADS1118_FSR_1024 = 3,
	ADS1118_FSR_512 = 4,
	ADS1118_FSR_256 = 5,
} ads1118_config_pga_t;  

typedef enum {
	ADS1118_DR_8 = 0,
	ADS1118_DR_16 = 1,
	ADS1118_DR_32 = 2,
	ADS1118_DR_64 = 3,
	ADS1118_DR_128 = 4,
	ADS1118_DR_250 = 5,
	ADS1118_DR_275 = 6,
	ADS1118_DR_860 = 7,
} ads1118_config_dr_t;  


uint32_t ads1118_init (uint8_t instance, uint8_t spi, uint8_t cs, uint8_t drdy ); 
uint32_t ads1118_transfer (uint8_t instance, uint16_t* conf, int16_t* conv); 
uint32_t ads1118_conv_done (uint8_t instance); 



#ifdef __cplusplus
}
#endif
#endif




