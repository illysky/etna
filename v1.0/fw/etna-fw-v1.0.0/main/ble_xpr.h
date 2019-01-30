
#ifndef BLE_SRG_H__
#define BLE_SRG_H__

#include <stdint.h>
#include <string.h>

#include "sdk_common.h"
#include <string.h>
#include "ble_l2cap.h"
#include "ble_srv_common.h"


// UUIDs for TRK Setup


#define XPR_BASE_UUID											{0x1F, 0x88, 0xAA, 0x2c, 0xda, 0x2e, 0x12, 0x97, 0x08, 0x49, 0x2e, 0x00, 0x82, 0x01, 0x53, 0xfc}
#define XPR_SERVICE_UUID									0x0000

#define XPR_DATA_CHAR_UUID								0x0001
#define XPR_DATA_CHAR_SIZE								4


#define XPR_READ_CHAR_UUID								0x0002
#define XPR_READ_CHAR_SIZE								1


typedef struct {
	
	uint16_t 										conn_handle;
	uint16_t 										service_handle;
	ble_gatts_char_handles_t 		data_characteristic;
	ble_gatts_char_handles_t 		read_characteristic;

	// Base UUID
	uint8_t  										uuid;
	
} xpr_bluetooth_peripheral_t;
extern xpr_bluetooth_peripheral_t xpr_peripheral; 



uint32_t 	xpr_bluetooth_init (void); 
uint32_t 	xpr_bluetooth_add_char (uint16_t uuid, uint16_t char_len, ble_gatts_char_handles_t* p_handles, uint8_t * p_description, uint8_t * p_initialval, bool read, bool notify, bool write);
void 			xpr_bluetooth_on_dispatch (ble_evt_t * p_ble_evt); 
void			xpr_bluetooth_on_connect (ble_evt_t * p_ble_evt); 
void 			xpr_bluetooth_on_disconnect (ble_evt_t * p_ble_evt);
void 			xpr_bluetooth_on_write (ble_evt_t * p_ble_evt); 
uint32_t 	xpr_bluetooth_write_gatt (ble_gatts_char_handles_t characteristic, uint8_t *data, uint16_t length); 
uint32_t 	xpr_bluetooth_notify (ble_gatts_char_handles_t characteristic, uint8_t *data, uint16_t length) ; 

 
					 
														
#endif // 

/** @} */
