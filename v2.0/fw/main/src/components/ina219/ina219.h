/************************************************************************************************************************************* 
 * @file  	
 * @brief 	
 *************************************************************************************************************************************/
#ifndef __INA219_H
#define __INA219_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <stdbool.h>



#define INA219_REG_CONFIG           0x00
#define INA219_REG_SHUNTV           0x01
#define INA219_REG_BUSV             0x02
#define INA219_REG_POWER            0x03
#define INA219_REG_CURRENT          0x04
#define INA219_REG_CALIBRATION      0x05

#define INA219_CONFIG_FSR_16V           0x00
#define INA219_CONFIG_FSR_32V           0x01

#define INA219_CONFIG_GAIN_40MV         0x00
#define INA219_CONFIG_GAIN_80MV         0x01
#define INA219_CONFIG_GAIN_160MV        0x02
#define INA219_CONFIG_GAIN_320MV        0x03

#define INA219_CONFIG_ADCRES_1_9BIT     0x00
#define INA219_CONFIG_ADCRES_1_10BIT    0x01
#define INA219_CONFIG_ADCRES_1_11BIT    0x02
#define INA219_CONFIG_ADCRES_1_12BIT    0x03
#define INA219_CONFIG_ADCRES_2_12BIT    0x08
#define INA219_CONFIG_ADCRES_4_12BIT    0x09
#define INA219_CONFIG_ADCRES_8_12BIT    0x0A
#define INA219_CONFIG_ADCRES_16_12BIT   0x0B
#define INA219_CONFIG_ADCRES_32_12BIT   0x0C
#define INA219_CONFIG_ADCRES_64_12BIT   0x0E
#define INA219_CONFIG_ADCRES_128_12BIT   0x0F

#define INA219_CONFIG_MODE_POWERDOWN              0x00
#define INA219_CONFIG_MODE_SVOLT_TRIGGERED        0x01
#define INA219_CONFIG_MODE_BVOLT_TRIGGERED        0x02
#define INA219_CONFIG_MODE_SANDBVOLT_TRIGGERED    0x03
#define INA219_CONFIG_MODE_ADCOFF                 0x04
#define INA219_CONFIG_MODE_SVOLT_CONTINUOUS       0x05
#define INA219_CONFIG_MODE_BVOLT_CONTINUOUS       0x06
#define INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS   0x07



int32_t ina219_init (uint8_t i2c, uint8_t device); 
int32_t ina219_config (uint8_t i2c, uint8_t device, uint8_t fsr, uint8_t pga, uint8_t badc, uint8_t sadc, uint8_t mode ); 
int32_t ina219_calibration (uint8_t i2c, uint8_t device, uint16_t cal); 
int32_t ina219_read_bus (uint8_t i2c, uint8_t device, uint16_t* value, uint8_t* ovf); 
int32_t ina219_read_shunt (uint8_t i2c, uint8_t device, double* mv); 
int32_t ina219_read_current (uint8_t i2c, uint8_t device, double* current, double divider); 
int32_t ina219_read_power (uint8_t i2c, uint8_t device, double* power, double multiplier); 


#ifdef __cplusplus
}
#endif

#endif //MCP4728_H