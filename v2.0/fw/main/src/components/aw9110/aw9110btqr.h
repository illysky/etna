/************************************************************************************************************************************* 
 * @file  	
 * @brief 	
 *************************************************************************************************************************************/
#ifndef AW9110BTQR_H
#define AW9110BTQR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define AW9110BTQR_ADDR             0x58   // 1011000


#define AW9110BTQR_REG_CTL          0x11
#define AW9110BTQR_REG_GPMD_A       0x12
#define AW9110BTQR_REG_GPMD_B       0x13
#define AW9110BTQR_REG_CFG_A        0x04
#define AW9110BTQR_REG_CFG_B        0x05


#define AW9110BTQR_REG_DIM_OUT0     0x20
#define AW9110BTQR_REG_DIM_OUT1     0x21
#define AW9110BTQR_REG_DIM_OUT2     0x22
#define AW9110BTQR_REG_DIM_OUT3     0x23
#define AW9110BTQR_REG_DIM_OUT4     0x24
#define AW9110BTQR_REG_DIM_OUT5     0x25
#define AW9110BTQR_REG_DIM_OUT6     0x26
#define AW9110BTQR_REG_DIM_OUT7     0x27
#define AW9110BTQR_REG_DIM_OUT8     0x28
#define AW9110BTQR_REG_DIM_OUT9     0x29
#define AW9110BTQR_REG_RESET        0x7F

int32_t aw9110btqr_init (uint8_t addr);
int32_t aw9110btqr_write_reg (uint8_t reg, uint8_t value); 

#ifdef __cplusplus
}
#endif

#endif //MCP4726_H