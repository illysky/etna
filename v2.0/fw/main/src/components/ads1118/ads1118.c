// ******************************************************************************************************************************************************************
// @file:   
// @brief:  
//
// Created by Dominic Moffat on 11/12/2017.
// Copyright © 2017 Illysky. All rights reserved.
// ******************************************************************************************************************************************************************

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "ads1118.h"
#include "hal.h"

static uint8_t ads1118_spi[8]; 
static uint8_t ads1118_cs_pin[8]; 
static uint8_t ads1118_drdy_pin[8]; 

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
uint32_t ads1118_init (uint8_t instance, uint8_t spi, uint8_t cs, uint8_t drdy ) 
{
	ads1118_spi[instance] = spi; 
	ads1118_cs_pin[instance] = cs; 
	ads1118_drdy_pin[instance] = drdy; 
	return 0; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
uint32_t ads1118_conv_done (uint8_t instance) 
{
	uint32_t state = 0; 
	hal_gpio_write (ads1118_cs_pin[instance], 0);  
	state = hal_gpio_read(ads1118_drdy_pin[instance]); 
	hal_gpio_write (ads1118_cs_pin[instance], 1);  
	if (state) 	return 0; 
	else 		return 1; 
}

// **************************************************************************************************************************************************************************
//  @func
//  @brief
// **************************************************************************************************************************************************************************
uint32_t ads1118_transfer (uint8_t instance, uint16_t* conf, int16_t* conv)
{
	uint32_t result = 0;
	uint8_t tx[4] = {0}; 
	uint8_t rx[4] = {0}; 
	
	tx[0] = (*conf >> 8) & 0xFF; 	
	tx[1] = (*conf >> 0) & 0xFF; 
	tx[3] = 0x00; 			
	tx[4] = 0x00; 		
	
	// Transfer SPI
	hal_gpio_write (ads1118_cs_pin[instance], 0);  
	result = hal_spi_transfer (ads1118_spi[instance], tx, 4,rx, 4, 1); 
	hal_gpio_write (ads1118_cs_pin[instance], 1); 
	
	// Parse data
	*conv = ((int16_t) rx[0]) << 8; 
	*conv |= ((int16_t) rx[1]); 
	*conf = ((int16_t) rx[2]) << 8; 
	*conf |= ((int16_t) rx[3]); 
	
	// Clear Junk
	if (result != 0) 
	{
		*conv = 0; 
		*conf = 0; 
	}
	
	return result; 
}





