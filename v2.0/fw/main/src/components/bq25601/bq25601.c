/************************************************************************************************************************************* 
 * @file  	
 * @brief 	
 *************************************************************************************************************************************/
#include "bq25601.h" 
#include "hal.h"


static uint8_t device = 0x00; 
static uint8_t instance = 0x00; 
/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t bq25601_init (uint32_t i, uint8_t addr)
{
    uint8_t buf=0;
    int32_t result=0;
    device = addr;                          // Set I2C address
    instance = i; 
    result = hal_i2c_read (instance, device, &buf, 1);  // Check it is present 
    return result; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t bq25601_write_reg (uint8_t reg, uint16_t value) 
{
	uint8_t buf[2]; 
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
int32_t bq25601_read_reg (uint8_t reg, uint8_t* value) 
{
    int32_t result = 0;
    result = hal_i2c_write (instance, device, &reg, 1, true);
    if (result == 0) 
    {
        uint8_t buf = 0;
        result = hal_i2c_read (instance,device, &buf, 1);  
        *value = buf; 
    }
	return result; 
}



/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t bq25601_set_iindpm (uint32_t current) 
{
    uint8_t buf = 0; 
    int32_t result = 0;
    result = bq25601_read_reg (BQ25601_REG00, &buf); 
    if (result == 0) 
    {
        buf &= ~(0x1F << 0);     
        buf |= ((uint8_t) (((float) current) / 100.0f)) << 0; 
        result = bq25601_write_reg (BQ25601_REG00, buf);
    }
    return result;  
} 


/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t bq25601_set_vindpm (uint8_t vindpm) 
{
    uint8_t buf = 0; 
    int32_t result = 0;
    result = bq25601_read_reg (BQ25601_REG06, &buf); 
    if (result == 0) 
    {
        buf &= ~(0x0F << 0);     
        buf |= (vindpm & 0x0F) << 0; 
        result = bq25601_write_reg (BQ25601_REG06, buf);
    }
    return result;  
} 
/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t bq25601_set_ichg (uint32_t current) 
{
    uint8_t ichg = 0; 
    uint8_t buf = 0; 
    int32_t result = 0;
    result = bq25601_read_reg (BQ25601_REG02, &buf);
    if (result == 0) 
    {
        // Calculate bit map
        if (current > 3000) current = 3000; 
        ichg = ((uint8_t)(current / 60)) & 0x3F; 
        buf &= ~(0x3F << 0);        
        buf |= (ichg << 0);       
        result = bq25601_write_reg (BQ25601_REG02, buf);
    }
    return result;  
} 


/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t bq25601_set_vreg (int32_t vreg) 
{ 
    uint8_t raw = 0; 
    uint8_t buf = 0; 
    int32_t result = 0;
    result = bq25601_read_reg (BQ25601_REG04, &buf);
    if (result == 0) 
    {
        if (vreg > 4624) vreg = 4624;   // Limit 
        vreg -= 3856;                   // Remove Offset
        if (vreg < 0) vreg = 0;         
        raw = (uint8_t) (vreg >> 5);    // Divide by 32
        buf &= ~(0x1F << 3);        
        buf |= (raw << 3);       
        result = bq25601_write_reg (BQ25601_REG04, buf);
    }
    return result;  
} 

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t bq25601_set_watchdog (uint8_t watchdog) 
{
    uint8_t buf = 0; 
    int32_t result = 0;
    result = bq25601_read_reg (BQ25601_REG05, &buf);
    if (result == 0) 
    {
        // Calculate bit map
        buf &= ~(0x03 << 4);        
        buf |= (watchdog << 4);
        result = bq25601_write_reg (BQ25601_REG05, buf);
    }
    return result;  
} 

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t bq25601_set_vmin (uint8_t vmin) 
{
    uint8_t buf = 0; 
    int32_t result = 0;
    result = bq25601_read_reg (BQ25601_REG01, &buf);
    if (result == 0) 
    {
        // Calculate bit map
        buf &= ~(0x03 << 1);     
        buf |= (vmin << 1);
        result = bq25601_write_reg (BQ25601_REG01, buf);
    }
    return result;  
} 


/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t bq25601_vbus_stat(uint8_t* status) 
{
    int32_t result = 0;
    result = bq25601_read_reg (BQ25601_REG08, status);
    *status = ((*status) >> 5) & 0x07; 
    return result; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t bq25601_chg_stat(uint8_t* status) 
{
    int32_t result = 0;
    uint8_t tmp; 
    result = bq25601_read_reg (BQ25601_REG08, &tmp);
    *status = ((tmp) >> 3) & 0x03; 
    return result; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t bq25601_get_status (uint8_t* status) 
{
    int32_t result = 0;
    uint8_t tmp; 
    result = bq25601_read_reg (BQ25601_REG08, &tmp);
    *status = tmp; 
    return result; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t bq25601_get_id(uint8_t* id) 
{
    int32_t result = 0;
    uint8_t tmp; 
    result = bq25601_read_reg (BQ25601_REG0B, &tmp);
    *id = ((tmp) >> 3) & 0x0F; 
    return result; 
}
/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t bq25601_get_fault (uint8_t* fault) 
{
    int32_t result = 0;
    uint8_t tmp; 
    result = bq25601_read_reg (BQ25601_REG09, &tmp);
    *fault = tmp; 
    return result; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t bq25601_pg_stat(uint8_t* status) 
{
    int32_t result = 0;
    result = bq25601_read_reg (BQ25601_REG08, status);
    *status = ((*status) >> 2) & 0x01; 
    return result; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t bq25601_reset (void) 
{
    uint8_t buf = 0; 
    int32_t result = 0;
    result = bq25601_read_reg (BQ25601_REG0B, &buf);
    if (result == 0) 
    {
        // Calculate bit map
        buf &= ~(0x01 << 7);        
        buf |= (0x01 << 7); 
        result = bq25601_write_reg (BQ25601_REG0B, buf);
    }
    return result;  
} 

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t bq25601_batfet_delay (bool enable) 
{
    uint8_t buf = 0; 
    int32_t result = 0;
    result = bq25601_read_reg (BQ25601_REG07, &buf);
    if (result == 0) 
    {
        // Reset Bit
        buf &= ~(0x01 << 3);        
        if (enable)
        {
            buf |= (0x01 << 3); 
        }

        result = bq25601_write_reg (BQ25601_REG07, buf);
    }
    return result;  
} 


/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t bq25601_qoff (void) 
{
    uint8_t buf = 0; 
    int32_t result = 0;
    result = bq25601_read_reg (BQ25601_REG07, &buf);
    if (result == 0) 
    {
        // Reset Bit
        buf &= ~(0x01 << 5);        
        buf |= (0x01 << 5); 
        result = bq25601_write_reg (BQ25601_REG07, buf);
    }
    return result;  
} 


/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t bq25601_force (void) 
{
    uint8_t buf = 0; 
    int32_t result = 0;
    result = bq25601_read_reg (BQ25601_REG07, &buf);
    if (result == 0) 
    {
        // Reset Bit
        buf &= ~(0x01 << 7);        
        buf |= (0x01 << 7); 
        result = bq25601_write_reg (BQ25601_REG07, buf);
    }
    return result;  
} 
/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t bq25601_clear_wd (void) 
{
    uint8_t buf = 0; 
    int32_t result = 0;
    result = bq25601_read_reg (BQ25601_REG01, &buf);
    if (result == 0) 
    {
        // Calculate bit map
        buf &= ~(0x01 << 6);        
        buf |= (0x01 << 6); 
        result = bq25601_write_reg (BQ25601_REG01, buf);
    }
    return result;  
} 