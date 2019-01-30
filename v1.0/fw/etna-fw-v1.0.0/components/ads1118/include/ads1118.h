
// ******************************************************************************************************************************************************************
// @file:   
// @brief: 
//
// Created by Dominic Moffat on 11/12/2017.
// Copyright © 2017 Illysky. All rights reserved.
// ******************************************************************************************************************************************************************
#ifndef __ADS1118_H
#define __ADS1118_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define ADS1118_SINGLE_SHOT_INTERNAL_TEMPERATURE 		0x859B
#define ADS1118_SINGLE_SHOT_ADC	 										0x8BEB


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


int16_t 		ads1118_read_ss_adc (ads1118_config_mux_t mux, ads1118_config_pga_t pga, ads1118_config_dr_t dr);
uint8_t 		ads1118_conversion_complete (void); 
uint32_t 		ads1118_transfer (uint16_t* config, int16_t* conversion); 


void 				ads1118_hal_cs_assert (void); 
void 				ads1118_hal_cs_deassert (void); 
uint8_t			ads1118_hal_drdy (void); 
void 				ads1118_hal_delay (uint32_t ms); 
uint32_t 		ads1118_hal_transfer (uint8_t* tx, uint8_t* rx, uint8_t size);






#ifdef __cplusplus
}
#endif
#endif




