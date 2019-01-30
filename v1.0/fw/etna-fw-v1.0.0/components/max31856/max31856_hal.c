// ******************************************************************************************************************************************************************
// @file:   
// @brief:  Parse a url string in to posix components
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
#include "max31856.h"
#include "hw_config.h"

extern const nrf_drv_spi_t APP_SPI0; 

// **************************************************************************************************************************************************************************
//  @func
//  @brief
// **************************************************************************************************************************************************************************
void max31856_hal_cs_assert (void)
{
	MAX31856_CS_ASSERT(); 
}

// **************************************************************************************************************************************************************************
//  @func
//  @brief
// **************************************************************************************************************************************************************************
void max31856_hal_cs_deassert (void)
{
	MAX31856_CS_DEASSERT(); 
}


// **************************************************************************************************************************************************************************
//  @func
//  @brief
// **************************************************************************************************************************************************************************
uint8_t max31856_hal_drdy (void) 
{
	return MAX31856_DRDY_STATE();
}

// **************************************************************************************************************************************************************************
//  @func
//  @brief
// **************************************************************************************************************************************************************************
void max31856_hal_delay (uint32_t ms) 
{
	nrf_delay_ms(ms); 
}


// **************************************************************************************************************************************************************************
//  @func
//  @brief
// **************************************************************************************************************************************************************************
uint32_t max31856_hal_transfer (uint8_t* tx, uint8_t* rx, uint8_t size) 
{
	return nrf_drv_spi_transfer (&APP_SPI0, tx, size, rx, size); 
}




	







