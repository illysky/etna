
#ifndef MANO_H__
#define MANO_H__



#ifdef __cplusplus
extern "C" {
#endif

/*** Include ***/
#include <stdint.h>
#include <stdbool.h>
#include "sdk_config.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"

#include "arm_math.h"





#define ETNA_PID_PARAM_KP        	300.0f        	/* Proportional */
#define ETNA_PID_PARAM_KI        	0.0f        	/* Integral */
#define ETNA_PID_PARAM_KD        	2.0f      	/* Derivative */

#define ETNA_CC_MAX_HEATER_TICKS 	19000
#define ETNA_CC_SAMPLE_TICKS 		22800
#define ETNA_CC_MAX_LED_MS 			200

#define ENTA_NTYPE_LUT_SIZE 51


typedef struct 
{
	uint32_t 	ticks; 
	double 		tip_temp; 
	double 		tip_temp_reported; 
	double 		setpoint; 
	bool 		tool_inserted; 
	uint32_t	heater_duty; 
	uint32_t	heater_duty_reported; 
	bool 		sleep; 
	bool 		overtemp; 
	arm_pid_instance_f32 pid;

} etna_heater_fsm_status_t; 


typedef enum 
{
    ETNA_CCP_STATE_WAIT,
    ETNA_CCP_STATE_PAUSE, 
    ETNA_CCP_STATE_MEASURE, 
} etna_ccp_state_t; 

void etna_init(void); 
void etna_start (void);
void etna_run (void); 

#ifdef __cplusplus
}
#endif

#endif 

/** @} */
