/************************************************************************************************************************************* 
 * @file  	
 * @brief 	
 *************************************************************************************************************************************/
#include "ads1015.h" 
#include "hal.h"

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t ads1015_init (uint8_t i2c, uint8_t device) 
{
    uint8_t buf = 0;
    return hal_i2c_read (i2c, device, &buf, 1);  // Check it is present 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t ads1015_read_single (uint8_t i2c, uint8_t device, uint16_t* value, uint8_t channel) 
{
  uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                    ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                    ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                    ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                    ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
                    ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)
	
	
	config |= ADS1015_REG_CONFIG_PGA_4_096V;
	
	  // Set single-ended input channel
    switch (channel)
    {
        case (0):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
            break;
        case (1):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
            break;
        case (2):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
            break;
        case (3):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
            break;
    }
	
	  // Set 'start single-conversion' bit
    config |= ADS1015_REG_CONFIG_OS_SINGLE;
    ads1015_write_reg (i2c, device, ADS1015_REG_POINTER_CONFIG, config); 
    hal_delay_ms (ADS1015_CONVERSIONDELAY); 
	ads1015_read_reg (i2c,device,ADS1015_REG_POINTER_CONVERT,value); 
	*value = *value >> 4;     // Shift
	*value = *value * 2;      // Gain
	return 0; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t ads1015_write_reg (uint8_t i2c, uint8_t device, uint8_t reg, uint16_t value) 
{
	uint8_t buf[3]; 
    int32_t result = 0;
	buf[0] = reg; 
	buf[1] = (value >> 8); 
	buf[2] = (uint8_t) (value & 0xFF); 
	result = hal_i2c_write (i2c, device, buf, 3, false); 
    return result; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t ads1015_read_reg (uint8_t i2c, uint8_t device, uint8_t reg, uint16_t* value) 
{
    int32_t result = 0;
    result = hal_i2c_write (i2c, device, &reg, 1, true);
    if (result == 0) 
    {
        uint8_t buf[3];
        result = hal_i2c_read (i2c, device, buf, 2);  
        *value = ((uint16_t) buf[0]) << 8; 
        *value |= ((uint16_t) buf[1]) & 0xFF; 
    }
	return result; 
}

