/************************************************************************************************************************************* 
 * @file  	
 * @brief 	
 *************************************************************************************************************************************/
#ifndef PCF85063_H
#define PCF85063_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define PCF85063_REG_CTRL1		    0x00 
#define PCF85063_REG_CTRL2		    0x01
#define PCF85063_REG_OFFSET		    0x02
#define PCF85063_REG_RAM		    0x03
#define PCF85063_REG_TIME           0x04 
#define PCF85063_REG_ALRM           0x0B 

#define PCF85063_BCD2DEC(x)         ((x/16*10) + (x%16));
#define PCF85063_DEC2BCD(x)         ((x/10*16) + (x%10));

typedef struct
{
	uint8_t sec; 
	uint8_t min;
	uint8_t hour;
	uint8_t weekday;
	uint8_t day;
	uint8_t mo; 
	uint8_t year;
} pcf85063_time_t; 







int32_t pcf85063_init (uint8_t instance, uint8_t addr);
int32_t pcf85063_get_time (pcf85063_time_t* t);
int32_t pcf85063_set_time (pcf85063_time_t* t);


#ifdef __cplusplus
}
#endif

#endif //MCP4726_H