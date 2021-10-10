/************************************************************************************************************************************* 
 * @file  	
 * @brief 	
 *************************************************************************************************************************************/
#ifndef BQ25601_H
#define BQ25601_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define BQ25601_REG00       0x00 
#define BQ25601_REG01       0x01 
#define BQ25601_REG02       0x02 
#define BQ25601_REG03       0x03 
#define BQ25601_REG04       0x04 
#define BQ25601_REG05       0x05  
#define BQ25601_REG06       0x06 
#define BQ25601_REG07       0x07 
#define BQ25601_REG08       0x08 
#define BQ25601_REG09       0x09 
#define BQ25601_REG0A       0x0A
#define BQ25601_REG0B       0x0B

int32_t bq25601_init (uint32_t i, uint8_t addr); 
int32_t bq25601_set_ichg (uint32_t current); 
int32_t bq25601_set_vindpm (uint8_t vindpm); 
int32_t bq25601_set_iindpm (uint32_t current); 
int32_t bq25601_vbus_stat(uint8_t* status);
int32_t bq25601_chg_stat(uint8_t* status); 
int32_t bq25601_set_watchdog (uint8_t watchdog);
int32_t bq25601_pg_stat(uint8_t* status); 
int32_t bq25601_reset (void);
int32_t bq25601_clear_wd (void); 
int32_t bq25601_get_fault (uint8_t* fault); 
int32_t bq25601_get_id(uint8_t* id);
int32_t bq25601_get_status (uint8_t* status); 
int32_t bq25601_qoff (void); 
int32_t bq25601_batfet_delay (bool enable); 
int32_t bq25601_set_vreg (int32_t vreg); 
int32_t bq25601_force (void); 

#ifdef __cplusplus
}
#endif

#endif 