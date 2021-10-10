/************************************************************************************************************************************* 
 * @file  	
 * @brief 	
 *************************************************************************************************************************************/
#ifndef IS31FL_H
#define IS31FL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define IS31FL_REG_SHDN       0x00
#define IS31FL_REG_OUT1       0x01
#define IS31FL_REG_OUT2       0x02
#define IS31FL_REG_OUT3       0x03
#define IS31FL_REG_OUT4       0x04
#define IS31FL_REG_OUT5       0x05
#define IS31FL_REG_OUT6       0x06
#define IS31FL_REG_OUT7       0x07
#define IS31FL_REG_OUT8       0x08
#define IS31FL_REG_OUT9       0x09
#define IS31FL_REG_OUT10      0x0A
#define IS31FL_REG_OUT11      0x0B
#define IS31FL_REG_OUT12      0x0C
#define IS31FL_REG_OUT13      0x0D
#define IS31FL_REG_OUT14      0x0E
#define IS31FL_REG_OUT15      0x0F
#define IS31FL_REG_OUT16      0x10
#define IS31FL_REG_OUT17      0x11
#define IS31FL_REG_OUT18      0x12
#define IS31FL_REG_LEDCTRL1   0x13
#define IS31FL_REG_LEDCTRL2   0x14
#define IS31FL_REG_LEDCTRL3   0x15
#define IS31FL_REG_UPDATE     0x16
#define IS31FL_REG_RESET      0x17

int32_t is31fl_init (uint8_t addr);
int32_t is31fl_write_reg (uint8_t reg, uint8_t value); 

#ifdef __cplusplus
}
#endif

#endif //MCP4726_H