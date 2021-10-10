/************************************************************************************************************************************* 
 * @file  	
 * @brief 	
 *************************************************************************************************************************************/
#ifndef TMP117_H
#define TMP117_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <string.h>


#define TMP117_REG_TEMP_RESULT      0x0
#define TMP117_REG_CONF             0x0
#define TMP117_REG_TH_LIM           0x0
#define TMP117_REG_TL_LIM           0x0
#define TMP117_REG_EEPROM_UL        0x0
#define TMP117_REG_EEPROM_1         0x0
#define TMP117_REG_EEPROM_2         0x0
#define TMP117_REG_TEMP_OS          0x0
#define TMP117_REG_EEPRIM_3         0x0
#define TMP117_REG_DEVICE_ID        0x0
#define TMP117_RESOLUTION           0.0078125

int32_t tmp117_init (uint8_t addr); 
int32_t tmp117_set_cmode (uint8_t mode);
int32_t tmp117_set_ctime (uint8_t time); 
int32_t tmp117_set_averaging (uint8_t avg); 
int32_t tmp117_read_temp (double* temp); 





#ifdef __cplusplus
}
#endif

#endif 