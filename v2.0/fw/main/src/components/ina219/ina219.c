
/************************************************************************************************************************************* 
 * @file  	
 * @brief 	
 *************************************************************************************************************************************/
#include "ina219.h" 
#include "hal.h"

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t ina219_init (uint8_t i2c, uint8_t device) 
{
    uint8_t buf=0;
    return hal_i2c_read (i2c, device, &buf, 1); 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t ina219_config (uint8_t i2c, uint8_t device, uint8_t fsr, uint8_t pga, uint8_t badc, uint8_t sadc, uint8_t mode ) 
{
    uint8_t buf[3]; 
    uint16_t config = (fsr<<13) | (pga<<11) | (badc<<7) | (sadc<<3) | (sadc<<4) | (mode<<0); 
    buf[0] = INA219_REG_CONFIG; 
    buf[1] = (config >> 8) & 0xFF; 
    buf[2] = (config >> 0) & 0xFF; 
    return hal_i2c_write (i2c, device, buf, 3, false);  
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t ina219_calibration (uint8_t i2c, uint8_t device, uint16_t cal)
{
    uint8_t buf[3]; 
    buf[0] = INA219_REG_CALIBRATION; 
    buf[1] = (cal >> 8) & 0xFF; 
    buf[2] = (cal >> 0) & 0xFF; 
    return hal_i2c_write (i2c, device, buf, 3, false);  
}


/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t ina219_read_bus (uint8_t i2c, uint8_t device, uint16_t* value, uint8_t* ovf)
{
    int32_t result = 0; 

    uint8_t addr = INA219_REG_BUSV; 
    result = hal_i2c_write (i2c, device, &addr, 1, true); 

    if (result == 0)
    {
        uint8_t buf[2]; 
        result = hal_i2c_read (i2c, device, buf, 2); 
        if (result == 0) 
        {
            uint16_t raw =  ((uint16_t) buf[1]) << 0; 
            raw |=          ((uint16_t) buf[0]) << 8; 

            if (ovf) 
            {
                *ovf =  (uint8_t) (raw & 0x0001); 
            }

            if (value) 
            {
                *value = (raw >> 3) * 4; 
            }
        }
    } 
    return result; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t ina219_read_shunt (uint8_t i2c, uint8_t device, double* mv)
{
    int32_t result = 0; 

    uint8_t addr = INA219_REG_SHUNTV; 
    result = hal_i2c_write (i2c, device, &addr, 1, true); 

    if (result == 0)
    {
        uint8_t buf[2]; 
        result = hal_i2c_read (i2c, device, buf, 1); 
        if (result == 0) 
        {
            int16_t raw =  ((uint16_t) buf[1]) << 0; 
            raw |=          ((uint16_t) buf[0]) << 8; 
            *mv = ((double) raw) * 0.01; 
        }
    } 
    return result; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t ina219_read_current (uint8_t i2c, uint8_t device, double* current, double divider)
{
    int32_t result = 0; 

    uint8_t addr = INA219_REG_CURRENT; 
    result = hal_i2c_write (i2c, device, &addr, 1, true); 

    if (result == 0)
    {
        uint8_t buf[2]; 
        result = hal_i2c_read (i2c, device, buf, 2); 
        if (result == 0) 
        {
            int16_t raw =  ((int16_t) buf[1]) << 0; 
            raw |=         ((int16_t) buf[0]) << 8; 
            *current = ((double) raw) / divider; 
        }
    } 
    return result; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t ina219_read_power (uint8_t i2c, uint8_t device, double* power, double multiplier)
{
    int32_t result = 0; 

    uint8_t addr = INA219_REG_POWER; 
    result = hal_i2c_write (i2c, device, &addr, 1, true); 

    if (result == 0)
    {
        uint8_t buf[2]; 
        result = hal_i2c_read (i2c, device, buf, 2); 
        if (result == 0) 
        {
            int16_t raw =  ((int16_t) buf[1]) << 0; 
            raw |=         ((int16_t) buf[0]) << 8; 

            *power = ((double) raw) * multiplier; 
        }
    } 
    return result; 
}






/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/


// Configures to INA219 to be able to measure up to 32V and 2A of current.
// Each unit of current corresponds to 100uA, and each unit of power
// corresponds to 2mW. Counter overflow occurs at 3.2A.
// NB: These calculations assume a 0.1 ohm resistor is present
void ina219_calculate_calibration(void) {
    // By default we use a pretty huge range for the input voltage,
    // which probably isn't the most appropriate choice for system
    // that don't use a lot of power.  But all of the calculations
    // are shown below if you want to change the settings.  You will
    // also need to change any relevant register settings, such as
    // setting the VBUS_MAX to 16V instead of 32V, etc.

    // 6. Calculate the power LSB
    // PowerLSB = 20 * CurrentLSB
    // PowerLSB = 0.002 (2mW per bit)

    // Max Current 


    // 7. Compute the maximum current and shunt voltage values before overflow
    //
    // Max_Current = Current_LSB * 32767
    // Max_Current = 3.2767A before overflow
    //
    // If Max_Current > Max_Possible_I then
    //    Max_Current_Before_Overflow = MaxPossible_I
    // Else
    //    Max_Current_Before_Overflow = Max_Current
    // End If
    //
    // Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
    // Max_ShuntVoltage = 0.32V
    //
    // If Max_ShuntVoltage >= VSHUNT_MAX
    //    Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
    // Else
    //    Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
    // End If

    // 8. Compute the Maximum Power
    // MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
    // MaximumPower = 3.2 * 32V
    // MaximumPower = 102.4W

    // Set multipliers to convert raw current/power values
    //ina219_currentDivider_mA = 10; // Current LSB = 100uA per bit (1000/100 = 10)
    //ina219_powerMultiplier_mW = 2; // Power LSB = 1mW per bit (2/1)

    // Set Calibration register to 'Cal' calculated above
    //ina219_write_calibration(INA219_MOTOR_ADDR);
    //ina219_write_calibration(INA219_SOL_ADDR);

    //uint16_t config = INA219_CONFIG_BVOLTAGERANGE_32V |
     //               INA219_CONFIG_GAIN_8_320MV | INA219_CONFIG_BADCRES_12BIT |
     //               INA219_CONFIG_SADCRES_12BIT_1S_532US |
     //               INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;

   //i2c_hal_write(INA219_MOTOR_ADDR, (uint8_t *)&config, sizeof(config));
    //i2c_hal_write(INA219_SOL_ADDR, (uint8_t *)&config, sizeof(config));
}
