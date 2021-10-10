/************************************************************************************************************************************* 
 * @file  	
 * @brief 	
 *************************************************************************************************************************************/
#include "aw9110btqr.h" 
#include "hal.h"

static uint8_t device = 0x00; 

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t aw9110btqr_init (uint8_t addr) 
{
    device = addr;                          // Set I2C address
    return 0; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t aw9110btqr_write_reg (uint8_t reg, uint8_t value) 
{
	uint8_t buf[3]; 
    int32_t result = 0;
	buf[0] = reg; 
	buf[1] = value; 
	result = hal_i2c_write (device, buf, 2, false); 
    return result; 
}



