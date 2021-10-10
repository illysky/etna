/************************************************************************************************************************************* 
 * @file  	
 * @brief 	
 *************************************************************************************************************************************/
#include "pcf85063.h" 
#include "hal.h"

static uint8_t device = 0x00; 
static uint8_t instance = 0x00; 

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t pcf85063_init (uint8_t i, uint8_t a) 
{
    uint8_t buf = 0; 
    device = a;                          // Set I2C address
    instance = i; 
    int32_t result = hal_i2c_read (instance, device, &buf, 1);  
    return result;  
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t pcf85063_write_reg (uint8_t reg, uint8_t value) 
{
	uint8_t buf[3]; 
    int32_t result = 0;
	buf[0] = reg; 
	buf[1] = value; 
	result = hal_i2c_write (instance, device, buf, 2, false); 
    return result; 
}


/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t pcf85063_read_reg (uint8_t reg, uint8_t* value) 
{
    int32_t result = 0;
    result = hal_i2c_write (instance, device, &reg, 1, true);
    if (result == 0) 
    {
        uint8_t buf = 0;
        result = hal_i2c_read (instance, device, &buf, 1);  
        *value = buf; 
    }
	return result; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t pcf85063_get_time (pcf85063_time_t* t) 
{
    int32_t result = 0;
    uint8_t buf[7] = {0};
    uint8_t reg = PCF85063_REG_TIME; 
    memset(t, 0, sizeof(pcf85063_time_t)); 
    result = hal_i2c_write (instance,  device, &reg, 1, true);
    if (result == 0) 
    {
        result = hal_i2c_read (instance, device, buf, 7);   
        buf[0] &= 0x7F; 
        t->sec = PCF85063_BCD2DEC(buf[0]) ; 
        t->min = PCF85063_BCD2DEC( buf[1]); 
        t->hour = PCF85063_BCD2DEC( buf[2]); 
        t->day = PCF85063_BCD2DEC( buf[3]); 
        t->weekday = PCF85063_BCD2DEC( buf[4]); 
        t->mo = PCF85063_BCD2DEC( buf[5]); 
        t->year = PCF85063_BCD2DEC( buf[6]); 
    }
	return result; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t pcf85063_set_time (pcf85063_time_t* t) 
{

    uint8_t buf[8] = {0};
    buf[0] = PCF85063_REG_TIME; 
    buf[1] = PCF85063_DEC2BCD(t->sec);
    buf[2] = PCF85063_DEC2BCD(t->min);
    buf[3] = PCF85063_DEC2BCD(t->hour);
    buf[4] = PCF85063_DEC2BCD(t->day);
    buf[5] = PCF85063_DEC2BCD(t->weekday);
    buf[6] = PCF85063_DEC2BCD(t->mo);
    buf[7] = PCF85063_DEC2BCD(t->year);
	return hal_i2c_write (instance, device, buf, 8, true);
}