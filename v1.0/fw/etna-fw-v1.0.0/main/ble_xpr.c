#include "ble_xpr.h"


xpr_bluetooth_peripheral_t xpr_peripheral;
extern uint32_t 								take_reading; 

/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
uint32_t xpr_bluetooth_init (void)
{
	  uint32_t        			err_code;
    ble_uuid_t      			ble_uuid;
    ble_uuid128_t   			service_base_uuid = XPR_BASE_UUID; 
	
		// Variable for the initialisation
		ble_gatts_char_md_t 	char_md;									// Characteristic Metadata Structure
    ble_gatts_attr_md_t 	cccd_md;									// Characteristic Attribute Metadata Structure
    ble_gatts_attr_t    	attr_char_value;					// Attribute Structure
    ble_gatts_attr_md_t 	attr_md;		  						// Attribute Metadata Structure

		// Clear Connection Handle
		xpr_peripheral.conn_handle = BLE_CONN_HANDLE_INVALID;
    memset(&cccd_md, 0, sizeof(cccd_md));
		memset(&attr_md, 0, sizeof(attr_md));
		memset(&char_md, 0, sizeof(char_md));	
		memset(&attr_char_value, 0, sizeof(attr_char_value));		
    
		// Setup Service Base UUID
		err_code = sd_ble_uuid_vs_add(&service_base_uuid, &xpr_peripheral.uuid);		
    if (err_code != NRF_SUCCESS){
        return err_code;
    }

		// Add Sensor Service
    ble_uuid.type = xpr_peripheral.uuid; 		
    ble_uuid.uuid = XPR_SERVICE_UUID;				
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &xpr_peripheral.service_handle);
    if (err_code != NRF_SUCCESS){
        return err_code;
    }
		
		// Add Chars 
		float tmp = 0; 
		xpr_bluetooth_add_char (XPR_DATA_CHAR_UUID, XPR_DATA_CHAR_SIZE, &xpr_peripheral.data_characteristic, (uint8_t*) "Data", (uint8_t*) &tmp, true, true, false);		
		xpr_bluetooth_add_char (XPR_READ_CHAR_UUID, XPR_READ_CHAR_SIZE, &xpr_peripheral.read_characteristic, (uint8_t*) "Read", NULL, false, false, true);

		return NRF_SUCCESS;
}


/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
uint32_t xpr_bluetooth_add_char (uint16_t uuid, uint16_t char_len, ble_gatts_char_handles_t* p_handles, uint8_t * p_description, uint8_t * p_initialval, bool read, bool notify, bool write)
{		
		ble_gatts_attr_md_t 	cccd_md;
		ble_uuid_t          	ble_uuid;
    ble_gatts_char_md_t 	char_md;
    ble_gatts_attr_t    	attr_char_value;
    ble_gatts_attr_md_t 	attr_md;	
		uint32_t error_code;			 											 
	
		// Are we using notify with this Char? Yes: Then setup up a CCCD structure
    if (notify){
        memset(&cccd_md, 0, sizeof(cccd_md));  
			  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);								// Read OPEN
				BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);							// Write OPEN
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }
	
		// Characteristic Metadata
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read  						= read;												// Read Permitted
		char_md.char_props.notify 					= notify;											// Notify Permitted
		char_md.char_props.write 						= write;											// Write Permitted
		char_md.char_props.write_wo_resp 		= write;											// Write Permitted
    char_md.p_char_user_desc 						= NULL;												// Description Data
    char_md.p_char_pf       					 	= NULL;												
    char_md.p_user_desc_md   						= NULL;												// Description Data MD
    char_md.p_cccd_md        						= (notify) ? &cccd_md : NULL;
    char_md.p_sccd_md        						= NULL;
		char_md.p_char_user_desc						= (uint8_t *) p_description; 
		char_md.char_user_desc_max_size			= strlen ((const char*)p_description); 
		char_md.char_user_desc_size					= strlen ((const char*)p_description); 
			
    
		// Value Metadata
    memset(&attr_md, 0, sizeof(attr_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);								// Read OPEN
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);							// Write OPEN 
		attr_md.vloc       = BLE_GATTS_VLOC_STACK;												// Variable Location
    attr_md.rd_auth    = 0;																						// ReadAuth not required
    attr_md.wr_auth    = 0;																						// WriteAuth not required
    attr_md.vlen       = 1;																						// Length Variable
    
		// Set up UUID 
		ble_uuid.type=xpr_peripheral.uuid;																// Base UUID
		ble_uuid.uuid=uuid; 																							// Char UUID

    attr_char_value.p_uuid       = &ble_uuid;													// UUID Type
		attr_char_value.p_attr_md    = &attr_md;													// Metadata for Attribute
    attr_char_value.init_len     = char_len;													// Initial Length of Attribute
    attr_char_value.init_offs    = 0;																	// Offsets 
    attr_char_value.max_len      = char_len;													// Max Length 
    attr_char_value.p_value      = p_initialval;											// Pointer to the value	

		// Add it to our service
    error_code = sd_ble_gatts_characteristic_add(xpr_peripheral.service_handle, &char_md, &attr_char_value, p_handles);
		return error_code; 
}


/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
void xpr_bluetooth_on_dispatch (ble_evt_t * p_ble_evt) 
{
	volatile uint32_t test; 
	
    switch (p_ble_evt->header.evt_id){
				case BLE_GAP_EVT_CONNECTED:
            xpr_bluetooth_on_connect  (p_ble_evt);
            break;
        case BLE_GAP_EVT_DISCONNECTED:
            xpr_bluetooth_on_disconnect (p_ble_evt);
            break;
				case BLE_GAP_EVT_RSSI_CHANGED:
						break; 
        case BLE_GATTS_EVT_WRITE:
            xpr_bluetooth_on_write (p_ble_evt); 
            break;
        default:
            break;
    }
}


/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
void xpr_bluetooth_on_connect (ble_evt_t * p_ble_evt)
{  
	xpr_peripheral.conn_handle = p_ble_evt->evt.gap_evt.conn_handle;	
}

/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
void xpr_bluetooth_on_disconnect (ble_evt_t * p_ble_evt)
{
	xpr_peripheral.conn_handle = BLE_CONN_HANDLE_INVALID;
}


/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
void xpr_bluetooth_on_write (ble_evt_t * p_ble_evt)
{
	ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	uint16_t handle = p_evt_write->handle; 
	uint32_t length = p_evt_write->len;  
	uint8_t data[32]; 

	// Copy Data
	memcpy (data, &p_evt_write->data[0], length); 
	if (handle==xpr_peripheral.read_characteristic.value_handle &&  length == 1){		
	
	}
}


/**********************************************************************************************************************
 * @function     
 * @brief       
**********************************************************************************************************************/
uint32_t xpr_bluetooth_write_gatt (ble_gatts_char_handles_t characteristic, uint8_t *data, uint16_t length) 
{
	ble_gatts_value_t char_data;
	memset(&char_data, 0, sizeof(ble_gatts_value_t));
	char_data.len = length; 
	char_data.offset = 0; 
	char_data.p_value = data; 
	return sd_ble_gatts_value_set	(xpr_peripheral.conn_handle, characteristic.value_handle, &char_data); 
} 

/**********************************************************************************************************************
 * @function     
 * @brief       
**********************************************************************************************************************/
uint32_t xpr_bluetooth_notify (ble_gatts_char_handles_t characteristic, uint8_t *data, uint16_t length) 
{
		ble_gatts_hvx_params_t hvx_params; 

		// Return if we are not connected. 
    if (xpr_peripheral.conn_handle == BLE_CONN_HANDLE_INVALID){
			return BLE_CONN_HANDLE_INVALID; 
    }

		// Notify Data
		memset(&hvx_params, 0, sizeof(hvx_params));
		hvx_params.handle = characteristic.value_handle;   
		hvx_params.p_data = data; 
		hvx_params.p_len  = &length; 
		hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
		
		volatile uint32_t err_code = sd_ble_gatts_hvx(xpr_peripheral.conn_handle, &hvx_params);	
		
		return err_code; 
}


/**********************************************************************************************************************
 * @eof       			End of File 
**********************************************************************************************************************/

