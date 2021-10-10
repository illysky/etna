
#ifndef MAMA_BLE_H__
#define MAMA_BLE_H__

#include <stdint.h>
#include <stdbool.h>
#include "sdk_config.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"


#ifdef __cplusplus
extern "C" {
#endif

#define BLE_MAMA_BLE_OBSERVER_PRIO 1

#define MAMA_BLE_DEF(_name)                          	\
    ble_mama_t _name;                        	\
    NRF_SDH_BLE_OBSERVER(_name ## _obs,            	 	\
                         BLE_BAS_BLE_OBSERVER_PRIO, 	\
                         mama_ble_on_ble_evt,        	\
                         &_name)
	
// ****************************************************************************************************************************************************************************************************
// Lima Base UUID 35b80000-3722-4b00-a2bb-34bf65be0ded
// #define MAMA_BASE_UUID                 		{{0xed,0x0d,0xbe,0x65,0xbf,0x34,0xbb,0xa2,0x00,0x4b,0x22,0x37,0x00,0x00,0xb8,0x35}}
#define MAMA_BASE_UUID						{{0x9A, 0x81, 0xAA, 0xE3, 0xD2, 0x3E, 0x10, 0x1B, 0x22, 0x10, 0x2F, 0x12, 0x00, 0x00, 0x12, 0xfA}}
#define MAMA_SERVICE_UUID					0x0000    	

#define MAMA_CHAR_UUID_OUT					0x0001
#define MAMA_CHAR_SIZE_OUT					20
#define MAMA_CHAR_NAME_OUT					"Out"

#define MAMA_CHAR_UUID_STATUS				0x0002
#define MAMA_CHAR_SIZE_STATUS				1
#define MAMA_CHAR_NAME_STATUS				"Status"

#define MAMA_CHAR_UUID_CMD					0x0003
#define MAMA_CHAR_SIZE_CMD					6
#define MAMA_CHAR_NAME_CMD					"Command"

#define MAMA_CHAR_UUID_MODE					0x0004
#define MAMA_CHAR_SIZE_MODE					1
#define MAMA_CHAR_NAME_MODE					"Mode"

#define MAMA_CHAR_UUID_SESSION				0x0005
#define MAMA_CHAR_SIZE_SESSION				4
#define MAMA_CHAR_NAME_SESSION				"Session"

#define MAMA_CHAR_UUID_INTERVAL				0x0006
#define MAMA_CHAR_SIZE_INTERVAL				2
#define MAMA_CHAR_NAME_INTERVAL				"Interval"

#define MAMA_CHAR_UUID_CONF					0x0007
#define MAMA_CHAR_SIZE_CONF					2
#define MAMA_CHAR_NAME_CONF					"Conf"

#define MAMA_CHAR_UUID_TIME					0x0008
#define MAMA_CHAR_SIZE_TIME					6
#define MAMA_CHAR_NAME_TIME					"Time"


#define MAMA_BLUETOOTH_COMMAND_STOP 			0x00
#define MAMA_BLUETOOTH_COMMAND_START 			0x01
#define MAMA_BLUETOOTH_COMMAND_PAUSE			0x02
#define MAMA_BLUETOOTH_COMMAND_RESUME			0x03
#define MAMA_BLUETOOTH_COMMAND_ZERO				0x04
#define MAMA_BLUETOOTH_COMMAND_OFF				0x05
#define MAMA_BLUETOOTH_COMMAND_CALIBRATION		0x06


#define MAMA_BLUETOOTH_STATUS_STOPPED			0
#define MAMA_BLUETOOTH_STATUS_RESUMED			1
#define MAMA_BLUETOOTH_STATUS_PAUSED			2
#define MAMA_BLUETOOTH_STATUS_CALIBRATE_OK		3
#define MAMA_BLUETOOTH_STATUS_CALIBRATE_ERROR	4

// ****************************************************************************************************************************************************************************************************

typedef struct {
    uint8_t                         	uuid_type;
    uint16_t                        	service_handle;
    uint16_t                            conn_handle;
	ble_gatts_char_handles_t        	char_handle_out;
	ble_gatts_char_handles_t        	char_handle_status;
	ble_gatts_char_handles_t        	char_handle_cmd;
	ble_gatts_char_handles_t        	char_handle_mode;
	ble_gatts_char_handles_t        	char_handle_session;
	ble_gatts_char_handles_t        	char_handle_interval;
	ble_gatts_char_handles_t        	char_handle_conf;
	ble_gatts_char_handles_t        	char_handle_time;
} ble_mama_t;

// extern ble_mama_t p_mama;

uint32_t 	mama_ble_init(ble_mama_t* p_mama);
void 		mama_ble_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);
uint32_t 	mama_ble_send_data (ble_mama_t * p_mama, uint8_t* p_data, uint16_t* p_length, uint16_t conn_handle);
uint32_t 	mama_ble_write_notify (ble_mama_t * p_mama, ble_gatts_char_handles_t characteristic, uint8_t *data, uint16_t length);


#ifdef __cplusplus
}
#endif

#endif 

/** @} */
