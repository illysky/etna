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
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_drv_spi.h"
#include "ads1118.h"
#include "hw_config.h"

extern const nrf_drv_spi_t APP_SPI0; 

// **************************************************************************************************************************************************************************
//  @func
//  @brief
// **************************************************************************************************************************************************************************
void ads1118_hal_cs_assert (void)
{
	ADS1118_CS_ASSERT(); 
}

// **************************************************************************************************************************************************************************
//  @func
//  @brief
// **************************************************************************************************************************************************************************
void ads1118_hal_cs_deassert (void)
{
	ADS1118_CS_DEASSERT(); 
}


// **************************************************************************************************************************************************************************
//  @func
//  @brief
// **************************************************************************************************************************************************************************
uint8_t ads1118_hal_drdy (void) 
{
	return ADS1118_DRDY_STATE();
}

// **************************************************************************************************************************************************************************
//  @func
//  @brief
// **************************************************************************************************************************************************************************
void ads1118_hal_delay (uint32_t ms) 
{
	nrf_delay_ms(ms); 
}


// **************************************************************************************************************************************************************************
//  @func
//  @brief
// **************************************************************************************************************************************************************************
uint32_t ads1118_hal_transfer (uint8_t* tx, uint8_t* rx, uint8_t size) 
{
	return nrf_drv_spi_transfer (&APP_SPI0, tx, size, rx, size); 
}




	







