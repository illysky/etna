/************************************************************************************************************************************* 
 * @file  	
 * @brief 	
 *************************************************************************************************************************************/
#include "tmp117.h" 
#include "hal.h"

static uint8_t device = 0x00; 

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t tmp117_init (uint8_t addr) 
{
    uint8_t buf=0;
    device = addr;                          // Set I2C address
    return hal_i2c_read (device, &buf, 1);  // Check it is present 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t tmp117_read_reg (uint8_t reg, uint16_t* value) 
{
    int32_t result = 0;
    result = hal_i2c_write (device, &reg, 1, true);
    if (result == 0) 
    {
        uint8_t buf[3];
        result = hal_i2c_read (device, buf, 2);  
        *value = ((uint16_t) buf[0]) << 8; 
        *value |= ((uint16_t) buf[1]) & 0xFF; 
    }
	return result; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t tmp117_write_reg (uint8_t reg, uint16_t value) 
{
	uint8_t buf[3]; 
    int32_t result = 0;
	buf[0] = reg; 
	buf[1] = (uint8_t) (value >> 8) & 0xFF; 
	buf[2] = (uint8_t) (value & 0xFF); 
	result = hal_i2c_write (device, buf, 3, false); 
    return result; 
}




/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t tmp117_set_cmode (uint8_t mode)
{
	uint16_t tmp = 0;
    int32_t result = tmp117_read_reg(TMP117_REG_CONF, &tmp); 
    if (result == 0)
    {
        tmp &= ~((1UL << 11) | (1UL << 10));             // Clear bits
        tmp = tmp | ( mode  & 0x03 ) << 10;                 // Set bits  
        result = tmp117_write_reg (TMP117_REG_CONF, tmp);   // Write 
    }
	return result;
}
/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t tmp117_set_ctime (uint8_t time)
{
	uint16_t tmp = 0;
    int32_t result = tmp117_read_reg(TMP117_REG_CONF, &tmp); 
    if (result == 0)
    {
        tmp &= ~((1UL << 9) | (1UL << 8) | (1UL << 7));     // Clear bits
        tmp = tmp | ( time  & 0x07 ) << 7;                  // Set bits  
        result = tmp117_write_reg (TMP117_REG_CONF, tmp);   // Write 
    }
	return result;
}
/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t tmp117_set_averaging (uint8_t avg)
{
	uint16_t tmp = 0;
    int32_t result = tmp117_read_reg(TMP117_REG_CONF, &tmp); 
    if (result == 0)
    {
        tmp &= ~((1UL << 6) | (1UL << 5) );                 // Clear bits
        tmp = tmp | ( avg & 0x03 ) << 5;                    // Set bits  
        result = tmp117_write_reg (TMP117_REG_CONF, tmp);   // Write 
    }
	return result;
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t tmp117_read_temp (double* temp)
{
	int16_t tmp = 0;
    *temp = 0; 
    int32_t result = tmp117_read_reg(TMP117_REG_TEMP_RESULT, (uint16_t*) &tmp); 
    if (result == 0)
    {
        *temp = ((double) tmp)* TMP117_RESOLUTION; 

    }
	return result;
}













