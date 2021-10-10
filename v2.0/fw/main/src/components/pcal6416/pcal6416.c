/************************************************************************************************************************************* 
 * @file  	
 * @brief 	
 *************************************************************************************************************************************/
#include "pcal6416.h" 
#include "hal.h"
 

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t pcal6416_init (uint8_t i2c, uint8_t device) 
{
    uint8_t buf=0;
    return hal_i2c_read (i2c, device, &buf, 1); 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t pcal6416_conf(uint8_t i2c, uint8_t device, uint16_t config) 
{
    uint8_t buf[3]={0};
    buf[0] = PCAL6416_REG_CONF_P0; 
    buf[1] = (uint8_t) ((config >> 0) & 0xFF); 
    buf[2] = (uint8_t) ((config >> 8) & 0xFF); 
    return hal_i2c_write (i2c, device, buf, 3, false); 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t pcal6416_set_out(uint8_t i2c, uint8_t device, uint16_t out) 
{
    uint8_t buf[3]={0};
    buf[0] = PCAL6416_REG_OUT_P0; 
    buf[1] = (uint8_t)  ((out >> 0) & 0xFF); 
    buf[2] = (uint8_t)  ((out >> 8) & 0xFF); 
    return hal_i2c_write (i2c, device, buf, 3,  false); 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t pcal6416_get_out(uint8_t i2c, uint8_t device, uint16_t* out) 
{
    uint8_t buf[8]={0};
    int32_t result = 0; 
    buf[0] = PCAL6416_REG_OUT_P0; 

    result = hal_i2c_write (i2c, device, buf, 1,  true); 
    if (result == 0)
    {
        result = hal_i2c_read (i2c, device, buf, 2);   
        if (result == 0) 
        {
            *out =  ((uint16_t) buf[0]) << 0;  
            *out |= ((uint16_t) buf[1]) << 8; 
        }
    
    }
    return result; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t pcal6416_write_gpio(uint8_t i2c, uint8_t device, uint8_t bit, uint8_t value) 
{
    uint16_t port = 0; 
    int32_t result = pcal6416_get_out(i2c, device, &port); 
    if (result == 0)
    {
        port &= ~(1UL << bit);  // Clears
        port |= value << bit;   // Write 
        result = pcal6416_set_out(i2c, device, port); 
    }
    return result; 
}













