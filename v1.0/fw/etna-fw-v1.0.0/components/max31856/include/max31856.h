
// ******************************************************************************************************************************************************************
// @file:   
// @brief:  Parse a url string in to posix components
//
// Created by Dominic Moffat on 11/12/2017.
// Copyright © 2017 Illysky. All rights reserved.
// ******************************************************************************************************************************************************************
#ifndef __MAX31856_H
#define __MAX31856_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX31856_CR0_REG           0x00
#define MAX31856_CR0_AUTOCONVERT   0x80
#define MAX31856_CR0_1SHOT         0x40
#define MAX31856_CR0_OCFAULT1      0x20
#define MAX31856_CR0_OCFAULT0      0x10
#define MAX31856_CR0_CJ            0x08
#define MAX31856_CR0_FAULT         0x04
#define MAX31856_CR0_FAULTCLR      0x02

#define MAX31856_CR1_REG           0x01
#define MAX31856_MASK_REG          0x02
#define MAX31856_CJHF_REG          0x03
#define MAX31856_CJLF_REG          0x04
#define MAX31856_LTHFTH_REG        0x05
#define MAX31856_LTHFTL_REG        0x06
#define MAX31856_LTLFTH_REG        0x07
#define MAX31856_LTLFTL_REG        0x08
#define MAX31856_CJTO_REG          0x09
#define MAX31856_CJTH_REG          0x0A
#define MAX31856_CJTL_REG          0x0B
#define MAX31856_LTCBH_REG         0x0C
#define MAX31856_LTCBM_REG         0x0D
#define MAX31856_LTCBL_REG         0x0E
#define MAX31856_SR_REG            0x0F

#define MAX31856_FAULT_CJRANGE     0x80
#define MAX31856_FAULT_TCRANGE     0x40
#define MAX31856_FAULT_CJHIGH      0x20
#define MAX31856_FAULT_CJLOW       0x10
#define MAX31856_FAULT_TCHIGH      0x08
#define MAX31856_FAULT_TCLOW       0x04
#define MAX31856_FAULT_OVUV        0x02
#define MAX31856_FAULT_OPEN        0x01

typedef enum
{
  MAX31856_TCTYPE_B  = 0,
  MAX31856_TCTYPE_E  = 1,
  MAX31856_TCTYPE_J  = 2,
  MAX31856_TCTYPE_K  = 3,
  MAX31856_TCTYPE_N  = 4,
  MAX31856_TCTYPE_R  = 5,
  MAX31856_TCTYPE_S  = 6,
  MAX31856_TCTYPE_T  = 7,
  MAX31856_VMODE_G8  = 8,
  MAX31856_VMODE_G32 = 9,
} max31856_thermocoupletype_t;

void max31856_init(void); 
void max31856_set_thermocouple (max31856_thermocoupletype_t type); 
void max31856_temperature_oneshot (void) ; 
float max31856_read_thc_temperature (void); 
float max31856_read_cj_temperature (void); 


uint32_t max31856_read8(uint8_t addr);
uint32_t max31856_read16(uint8_t addr); 
uint32_t max31856_read24(uint8_t addr);
uint32_t max31856_write8(uint8_t addr, uint8_t data);

void 			max31856_hal_cs_assert (void); 
void 			max31856_hal_cs_deassert (void); 
void 			max31856_hal_enable (bool enable); 
void 			max31856_hal_delay (uint32_t ms); 
uint32_t 	max31856_hal_transfer (uint8_t* tx, uint8_t* rx, uint8_t size);






#ifdef __cplusplus
}
#endif
#endif




