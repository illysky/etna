/************************************************************************************************************************************* 
 * @file  	
 * @brief 	
 *************************************************************************************************************************************/
#include "tmc2130.h" 
#include "hal.h"
#include "nrf_log.h"
#include "custom_board.h"
 
static uint8_t tmc2130_cs_pins[8] = {0xFF}; 
static uint8_t tmc2130_spi[8] = {0xFF}; 

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static uint32_t tmc2130_write_mask (uint32_t var, uint32_t val, uint32_t p, uint32_t w )
{
    uint32_t mask = (2^w)-1; 
    var &= ~(mask << p);              // Clear 
    var |= (val & mask)  << p;        // Write 
    return var; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t tmc2130_read (uint32_t driver, uint8_t addr, uint32_t *data) 
{
    int32_t result = 0; 
    uint8_t tx[5] = {0};
    uint8_t rx[5] = {0}; 
    tx[0]  = addr; 
    hal_gpio_write (tmc2130_cs_pins[driver], 0); 
    result = hal_spi_transfer (tmc2130_spi[driver], tx, 5, NULL, 0); 
    hal_gpio_write (tmc2130_cs_pins[driver], 1); 
    hal_gpio_write (tmc2130_cs_pins[driver], 0); 
    result = hal_spi_transfer (tmc2130_spi[driver], tx, 5, rx, 5);
    hal_gpio_write (tmc2130_cs_pins[driver], 1); 
    if (result == 0)
    {
        *data = ((uint32_t) rx[1]) << 24 | ((uint32_t) rx[2]) << 16 | ((uint32_t) rx[3]) << 8 | ((uint32_t) rx[4]) << 0;  
    } 
    return result; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t tmc2130_write (uint32_t driver, uint8_t addr, uint32_t data) 
{
    uint8_t tx[5] = {0};
    tx[0] = addr | 0x80; 
    tx[1] = (uint8_t) ((data >> 24) & 0xFF); 
    tx[2] = (uint8_t) ((data >> 16) & 0xFF); 
    tx[3] = (uint8_t) ((data >> 8) & 0xFF); 
    tx[4] = (uint8_t) ((data >> 0) & 0xFF); 
    hal_gpio_write (tmc2130_cs_pins[driver], 0); 
    hal_spi_transfer (tmc2130_spi[driver], tx, 5, NULL, 0); 
    hal_gpio_write (tmc2130_cs_pins[driver], 1); 
    return 0; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t tmc2130_toff(uint8_t driver, uint32_t value)
{
    uint32_t tmp = 0; 
    tmc2130_read (driver, TMC2130_REG_CHOPCONF, &tmp); 
    tmc2130_write (driver, TMC2130_REG_CHOPCONF, tmc2130_write_mask (tmp, value, 0, 4)) ;
    return 0; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t tmc2130_tbl (uint8_t driver, uint32_t value)
{
    uint32_t tmp = 0; 
    tmc2130_read (driver, TMC2130_REG_CHOPCONF, &tmp); 
    tmc2130_write (driver, TMC2130_REG_CHOPCONF, tmc2130_write_mask (tmp, value, 15, 2)) ;
    return 0; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void tmc2130_conf_home (uint8_t driver) 
{
    tmc2130_write (driver, TMC2130_REG_CHOPCONF,   0x000100C3); 
    tmc2130_write (driver, TMC2130_REG_IHOLD_IRUN, 0x00061F0A); 
    tmc2130_write (driver, TMC2130_REG_TPOWERDOWN, 0x0000000A); 
    tmc2130_write (driver, TMC2130_REG_TPWMTHRS,   0x000001F4); 
    tmc2130_write (driver, TMC2130_REG_PWMCONF,    0x00040FC8); 
    tmc2130_write (driver, TMC2130_REG_TCOOLTHRS,   0); 
    tmc2130_write (driver, TMC2130_REG_TCOOLTHRS,   0xFFFFF); 
    tmc2130_write (driver, TMC2130_REG_THIGH,   0); 
    tmc2130_write (driver, TMC2130_REG_COOLCONF,   0x000000); 
    tmc2130_write (driver, TMC2130_REG_GCONF,      0x00000087);

}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void tmc2130_conf_run (uint8_t driver) 
{
    tmc2130_write (driver, TMC2130_REG_CHOPCONF,   0x000100C3); 
    tmc2130_write (driver, TMC2130_REG_IHOLD_IRUN, 0x00061F0A); 
    tmc2130_write (driver, TMC2130_REG_TPOWERDOWN, 0x0000000A); 
    tmc2130_write (driver, TMC2130_REG_TPWMTHRS,   0x000001F4); 
    tmc2130_write (driver, TMC2130_REG_PWMCONF,    0x00040FC8); 
    tmc2130_write (driver, TMC2130_REG_GCONF,      0x00000007); 
    tmc2130_write (driver, TMC2130_REG_TCOOLTHRS,   0); 
}



/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t tmc2130_init (uint8_t driver, uint8_t spi, uint32_t cs) 
{
    tmc2130_cs_pins[driver] = cs;   // Grab CS pin
    tmc2130_spi[driver] = spi;      // Grab instance to use for this driver
    return 0; 
}










