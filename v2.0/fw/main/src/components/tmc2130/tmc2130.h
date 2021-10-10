/************************************************************************************************************************************* 
 * @file  	
 * @brief 	
 *************************************************************************************************************************************/
#ifndef TMC2130_H
#define TMC2130_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define TMC2130_REG_GCONF           0x00
#define TMC2130_REG_GSTAT			0x01
#define TMC2130_REG_IOIN			0x04
#define TMC2130_REG_IHOLD_IRUN  	0x10
#define TMC2130_REG_TPOWERDOWN  	0x11
#define TMC2130_REG_TSTEP			0x12
#define TMC2130_REG_TPWMTHRS		0x13
#define TMC2130_REG_TCOOLTHRS		0x14
#define TMC2130_REG_THIGH			0x15
#define TMC2130_REG_XDIRECT	    	0x2D
#define TMC2130_REG_VDCMIN	    	0x33
#define TMC2130_REG_MSLUT0	    	0x60
#define TMC2130_REG_MSLUT1	    	0x61
#define TMC2130_REG_MSLUT2	    	0x62
#define TMC2130_REG_MSLUT3	    	0x63
#define TMC2130_REG_MSLUT4	    	0x64
#define TMC2130_REG_MSLUT5	    	0x65
#define TMC2130_REG_MSLUT6	    	0x66
#define TMC2130_REG_MSLUT7	    	0x67
#define TMC2130_REG_MSLUTSEL		0x68
#define TMC2130_REG_MSLUTSTART  	0x69
#define TMC2130_REG_MSCNT			0x6A
#define TMC2130_REG_MSCURACT		0x6B
#define TMC2130_REG_CHOPCONF		0x6C
#define TMC2130_REG_COOLCONF		0x6D
#define TMC2130_REG_DCCTRL		    0x6E
#define TMC2130_REG_DRV_STATUS  	0x6F
#define TMC2130_REG_PWMCONF		    0x70
#define TMC2130_REG_PWM_SCALE		0x71
#define TMC2130_REG_ENCM_CTRL		0x72
#define TMC2130_REG_LOST_STEPS  	0x73




void tmc2130_conf_run (uint8_t driver);
void tmc2130_conf_home (uint8_t driver); 
int32_t tmc2130_init (uint8_t driver, uint8_t spi, uint32_t cs); 


#ifdef __cplusplus
}
#endif

#endif //MCP4728_H