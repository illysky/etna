/************************************************************************************************************************************* 
 * @file  	
 * @brief 	
 *************************************************************************************************************************************/
#include "mcp4726.h" 
#include "hal.h"

static uint8_t device = 0x00; 

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t mcp4726_init (uint8_t addr) 
{
    uint8_t buf=0;
    device = addr;                          // Set I2C address
    return hal_i2c_read (device, &buf, 1);  // Check it is present 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t mcp4726_write_dac(uint16_t value) 
{
    // Automatically enables VOUT
    uint8_t buf[2]; 
    value = value & 0x0FFF; 
    buf[0] = (MCP4726_CMD_VOLDAC | (((uint8_t) ((value >> 8) & 0xFF)))); 
    buf[1] = ((uint8_t) (value & 0xFF)); 
    return hal_i2c_write (device, buf, 2, false); 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t mcp4726_write_conf (uint8_t gain, uint8_t vref) 
{
    uint8_t buf[6] = {0}; 
    int32_t result = hal_i2c_read (device, buf, 6); 
    if (result == 0)
    {
        gain = gain & 0x01; 
        vref = vref & 0x03; 
        uint8_t pd = (buf[0] >> 1) & 0x01;
        uint8_t config = (MCP4726_CMD_VOLCONFIG | (gain << 0) | (vref << 3) | (pd << 1)); 
        result = hal_i2c_write (device, &config, 1, false); 
    } 
    return result;
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t mcp4726_write_pd (uint8_t pd) 
{
    uint8_t buf[6] = {0}; 
    int32_t result = hal_i2c_read (device, buf, 6); 
    if (result == 0)
    {
        pd = pd & 0x01;
        uint8_t gain = (buf[0] >> 0) & 0x01;            // Get existing gain
        uint8_t vref = (buf[0] >> 3) & 0x03;            // Get existing vref
        uint8_t config = (MCP4726_CMD_VOLCONFIG | (gain << 0) | (vref << 3) | (pd << 1)); 
        result = hal_i2c_write (device, &config, 1, false); 
    }
    return result; 
}












