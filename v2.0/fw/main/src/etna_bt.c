/************************************************************************************************************************************* 
 * @file  	
 * @brief 	
 *************************************************************************************************************************************/


#include "string.h"
#include "stdlib.h"

#include "sdk_common.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_log.h"

#include "etna_bt.h"
#include "etna.h"
#include "custom_board.h"
#include "hal.h"

// ble_mama_t* p_mama;

uint32_t mama_ble_init(ble_mama_t * p_mama) {
    volatile ret_code_t         err_code;
    ble_uuid_t            		ble_uuid;
    ble_uuid128_t         		mama_base_uuid = MAMA_BASE_UUID;
    ble_add_char_params_t 		add_char_params;
	ble_add_char_user_desc_t 	user_description;  
	
    VERIFY_PARAM_NOT_NULL(p_mama);

	/*** Add base vendor UUID ***/
    err_code = sd_ble_uuid_vs_add(&mama_base_uuid, &p_mama->uuid_type);
    VERIFY_SUCCESS(err_code);
    ble_uuid.type = p_mama->uuid_type;
    ble_uuid.uuid = MAMA_SERVICE_UUID;

    /*** Add service ***/
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,&ble_uuid,&p_mama->service_handle);
    VERIFY_SUCCESS(err_code);

    /*** Add Characteristic: COMMAND ***/
    memset(&add_char_params, 0, sizeof(add_char_params));
    memset(&user_description, 0, sizeof(user_description));
    user_description.p_char_user_desc           = (uint8_t*)MAMA_CHAR_NAME_CMD; 
    user_description.size                       = sizeof(MAMA_CHAR_NAME_CMD); 
    user_description.max_size                   = sizeof(MAMA_CHAR_NAME_CMD); 
    user_description.read_access                = SEC_OPEN;
    add_char_params.uuid                    	= MAMA_CHAR_UUID_CMD;
    add_char_params.uuid_type                	= p_mama->uuid_type;
    add_char_params.max_len                  	= MAMA_CHAR_SIZE_CMD;
    add_char_params.init_len                 	= sizeof(uint8_t);
    add_char_params.is_var_len              	= true;
    add_char_params.char_props.write        	= 1;
    add_char_params.char_props.write_wo_resp 	= 1;
    add_char_params.read_access  				= SEC_OPEN;
    add_char_params.write_access 				= SEC_OPEN;
    add_char_params.p_user_descr				= &user_description; 		
    err_code = characteristic_add(p_mama->service_handle, &add_char_params, &p_mama->char_handle_cmd);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }

    /*** Add Characteristic: OUT ***/
    memset(&add_char_params, 0, sizeof(add_char_params));
    memset(&user_description, 0, sizeof(user_description));
    user_description.p_char_user_desc           = (uint8_t*)MAMA_CHAR_NAME_OUT; 
    user_description.size                       = sizeof(MAMA_CHAR_NAME_OUT); 
    user_description.max_size                   = sizeof(MAMA_CHAR_NAME_OUT); 
    user_description.read_access                = SEC_OPEN;
    add_char_params.uuid                    	= MAMA_CHAR_UUID_OUT;
    add_char_params.uuid_type                	= p_mama->uuid_type;
    add_char_params.max_len                  	= MAMA_CHAR_SIZE_OUT;
    add_char_params.init_len                 	= sizeof(uint8_t);
    add_char_params.is_var_len              	= true;
    add_char_params.char_props.notify           = 1;
    add_char_params.char_props.read             = 1;
    add_char_params.cccd_write_access           = SEC_OPEN;
    add_char_params.read_access  				= SEC_OPEN;
	add_char_params.p_user_descr			    = &user_description; 		
    err_code = characteristic_add(p_mama->service_handle, &add_char_params, &p_mama->char_handle_out);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }

    /*** Add Characteristic: STATUS ***/
    memset(&add_char_params, 0, sizeof(add_char_params));
    memset(&user_description, 0, sizeof(user_description));
    user_description.p_char_user_desc           = (uint8_t*)MAMA_CHAR_NAME_STATUS; 
    user_description.size                       = sizeof(MAMA_CHAR_NAME_STATUS); 
    user_description.max_size                   = sizeof(MAMA_CHAR_NAME_STATUS); 
    user_description.read_access                = SEC_OPEN;
    add_char_params.uuid                    	= MAMA_CHAR_UUID_STATUS;
    add_char_params.uuid_type                	= p_mama->uuid_type;
    add_char_params.max_len                  	= MAMA_CHAR_SIZE_STATUS;
    add_char_params.init_len                 	= sizeof(uint8_t);
    add_char_params.is_var_len              	= true;
    add_char_params.char_props.notify           = 1;
    add_char_params.char_props.read             = 1;
    add_char_params.cccd_write_access           = SEC_OPEN;
    add_char_params.read_access  				= SEC_OPEN;
	add_char_params.p_user_descr			    = &user_description; 		
    err_code = characteristic_add(p_mama->service_handle, &add_char_params, &p_mama->char_handle_status);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }

    /*** Add Characteristic: MODE ***/
    memset(&add_char_params, 0, sizeof(add_char_params));
    memset(&user_description, 0, sizeof(user_description));
    user_description.p_char_user_desc           = (uint8_t*)MAMA_CHAR_NAME_MODE; 
    user_description.size                       = sizeof(MAMA_CHAR_NAME_MODE); 
    user_description.max_size                   = sizeof(MAMA_CHAR_NAME_MODE); 
    user_description.read_access                = SEC_OPEN;
    add_char_params.uuid                    	= MAMA_CHAR_UUID_MODE;
    add_char_params.uuid_type                	= p_mama->uuid_type;
    add_char_params.max_len                  	= MAMA_CHAR_SIZE_MODE;
    add_char_params.init_len                 	= sizeof(uint8_t);
    add_char_params.is_var_len              	= true;
    add_char_params.char_props.notify           = 1;
    add_char_params.char_props.read             = 1;
    add_char_params.cccd_write_access           = SEC_OPEN;
    add_char_params.read_access  				= SEC_OPEN;
	add_char_params.p_user_descr			    = &user_description; 		
    err_code = characteristic_add(p_mama->service_handle, &add_char_params, &p_mama->char_handle_mode);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }

    /*** Add Characteristic: SESSION ***/
    memset(&add_char_params, 0, sizeof(add_char_params));
    memset(&user_description, 0, sizeof(user_description));
    user_description.p_char_user_desc           = (uint8_t*)MAMA_CHAR_NAME_SESSION; 
    user_description.size                       = sizeof(MAMA_CHAR_NAME_SESSION); 
    user_description.max_size                   = sizeof(MAMA_CHAR_NAME_SESSION); 
    user_description.read_access                = SEC_OPEN;
    add_char_params.uuid                    	= MAMA_CHAR_UUID_SESSION;
    add_char_params.uuid_type                	= p_mama->uuid_type;
    add_char_params.max_len                  	= MAMA_CHAR_SIZE_SESSION;
    add_char_params.init_len                 	= sizeof(uint8_t);
    add_char_params.is_var_len              	= true;
    add_char_params.char_props.write        	= 1;
    add_char_params.char_props.write_wo_resp 	= 1;
    add_char_params.char_props.notify           = 1;
    add_char_params.char_props.read             = 1;
    add_char_params.cccd_write_access           = SEC_OPEN;
    add_char_params.read_access  				= SEC_OPEN;
    add_char_params.write_access 				= SEC_OPEN;	
	add_char_params.p_user_descr			    = &user_description; 		
    err_code = characteristic_add(p_mama->service_handle, &add_char_params, &p_mama->char_handle_session);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }

    /*** Add Characteristic: INTERVAL ***/
    memset(&add_char_params, 0, sizeof(add_char_params));
    memset(&user_description, 0, sizeof(user_description));
    user_description.p_char_user_desc           = (uint8_t*)MAMA_CHAR_NAME_INTERVAL; 
    user_description.size                       = sizeof(MAMA_CHAR_NAME_INTERVAL); 
    user_description.max_size                   = sizeof(MAMA_CHAR_NAME_INTERVAL); 
    user_description.read_access                = SEC_OPEN;
    add_char_params.uuid                    	= MAMA_CHAR_UUID_INTERVAL;
    add_char_params.uuid_type                	= p_mama->uuid_type;
    add_char_params.max_len                  	= MAMA_CHAR_SIZE_INTERVAL;
    add_char_params.init_len                 	= sizeof(uint8_t);
    add_char_params.is_var_len              	= true;
    add_char_params.char_props.write        	= 1;
    add_char_params.char_props.write_wo_resp 	= 1;
    add_char_params.char_props.notify           = 1;
    add_char_params.char_props.read             = 1;
    add_char_params.cccd_write_access           = SEC_OPEN;
    add_char_params.read_access  				= SEC_OPEN;
    add_char_params.write_access 				= SEC_OPEN;	
	add_char_params.p_user_descr			    = &user_description; 		
    err_code = characteristic_add(p_mama->service_handle, &add_char_params, &p_mama->char_handle_interval);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }

    /*** Add Characteristic: CONF ***/
    memset(&add_char_params, 0, sizeof(add_char_params));
    memset(&user_description, 0, sizeof(user_description));
    user_description.p_char_user_desc           = (uint8_t*)MAMA_CHAR_NAME_CONF; 
    user_description.size                       = sizeof(MAMA_CHAR_NAME_CONF); 
    user_description.max_size                   = sizeof(MAMA_CHAR_NAME_CONF); 
    user_description.read_access                = SEC_OPEN;
    add_char_params.uuid                    	= MAMA_CHAR_UUID_CONF;
    add_char_params.uuid_type                	= p_mama->uuid_type;
    add_char_params.max_len                  	= MAMA_CHAR_SIZE_CONF;
    add_char_params.init_len                 	= sizeof(uint8_t);
    add_char_params.is_var_len              	= true;
    add_char_params.char_props.write        	= 1;
    add_char_params.char_props.write_wo_resp 	= 1;
    add_char_params.char_props.notify           = 1;
    add_char_params.char_props.read             = 1;
    add_char_params.cccd_write_access           = SEC_OPEN;
    add_char_params.read_access  				= SEC_OPEN;
    add_char_params.write_access 				= SEC_OPEN;	
	add_char_params.p_user_descr			    = &user_description; 		
    err_code = characteristic_add(p_mama->service_handle, &add_char_params, &p_mama->char_handle_conf);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }

    /*** Add Characteristic: TIME ***/
    memset(&add_char_params, 0, sizeof(add_char_params));
    memset(&user_description, 0, sizeof(user_description));
    user_description.p_char_user_desc           = (uint8_t*)MAMA_CHAR_NAME_TIME; 
    user_description.size                       = sizeof(MAMA_CHAR_NAME_TIME); 
    user_description.max_size                   = sizeof(MAMA_CHAR_NAME_TIME); 
    user_description.read_access                = SEC_OPEN;
    add_char_params.uuid                    	= MAMA_CHAR_UUID_TIME;
    add_char_params.uuid_type                	= p_mama->uuid_type;
    add_char_params.max_len                  	= MAMA_CHAR_SIZE_TIME;
    add_char_params.init_len                 	= sizeof(uint8_t);
    add_char_params.is_var_len              	= true;
    add_char_params.char_props.write        	= 1;
    add_char_params.char_props.write_wo_resp 	= 1;
    add_char_params.char_props.notify           = 1;
    add_char_params.char_props.read             = 1;
    add_char_params.cccd_write_access           = SEC_OPEN;
    add_char_params.read_access  				= SEC_OPEN;
    add_char_params.write_access 				= SEC_OPEN;	
	add_char_params.p_user_descr			    = &user_description; 		
    err_code = characteristic_add(p_mama->service_handle, &add_char_params, &p_mama->char_handle_time);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }

	return err_code;
}

static void on_connect(ble_mama_t * p_mama, ble_evt_t const * p_ble_evt) {
	// NOP

}

static void on_write(ble_mama_t * p_mama, ble_evt_t const * p_ble_evt) {
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    uint8_t data[20];
    uint32_t length = p_evt_write->len;
    memcpy (data, &p_evt_write->data[0], length);

    // Command Characteristic 
    if ((p_evt_write->handle == p_mama->char_handle_cmd.value_handle) && (p_evt_write->len >= 1)) {
        uint8_t command = data[0]; 
		uint8_t response = 0; 
		switch (command) {
			case MAMA_BLUETOOTH_COMMAND_STOP: {
                if (AppData.status == APP_DATA_STATUS_REC) {
                    AppData.willStop = true;
                }
                break; 
            }
			case MAMA_BLUETOOTH_COMMAND_START: {
                if (AppData.status == APP_DATA_STATUS_IDLE) {
                    AppData.willStart = true;
                }
                break; 
            }
			case MAMA_BLUETOOTH_COMMAND_PAUSE: {
				AppData.willPause = true;	
				response = MAMA_BLUETOOTH_STATUS_PAUSED; 
				mama_ble_write_notify(p_mama, p_mama->char_handle_status, (uint8_t*) &response, 1);
				break; 
            }
			case MAMA_BLUETOOTH_COMMAND_RESUME: {
				AppData.willPause = false;	
				response = MAMA_BLUETOOTH_STATUS_RESUMED; 	
				mama_ble_write_notify(p_mama, p_mama->char_handle_status, (uint8_t*) &response, 1);	
				break;
            }
			case MAMA_BLUETOOTH_COMMAND_ZERO: {
				AppData.willZero = true; 
				break;
            }
			case MAMA_BLUETOOTH_COMMAND_OFF: {
				AppData.willPowerOff = true; 
				break; 
            }
			case MAMA_BLUETOOTH_COMMAND_CALIBRATION: {
                // Gather Variables
                uint8_t table_entry = data[1]; 	
                float calibration_data = 0; 
                memcpy (&calibration_data, &data[2], 4); 
                
                // Update only when we arent running, and sensible table entries and values - currently 6 entries with offsets -1000 to 1000 mmHg
                if (AppData.status == APP_DATA_STATUS_IDLE && table_entry < 6 && calibration_data > -400 && calibration_data < 400 ) {
                    AppConfig.calibrationP[table_entry] = calibration_data; 
                    AppData.updateConfig = true; 	
                    response = MAMA_BLUETOOTH_STATUS_CALIBRATE_OK; 	
                    mama_ble_write_notify(p_mama, p_mama->char_handle_status, (uint8_t*) &response, 1);	
                    
                    for (uint8_t a = 0; a<3; a++) {
                        LED_RED_ON; 
                        hal_delay_ms(200); 
                        LED_RED_OFF; 
                        hal_delay_ms(200); 
                    }
                } else {
                    response = MAMA_BLUETOOTH_STATUS_CALIBRATE_ERROR; 	
                    mama_ble_write_notify(p_mama, p_mama->char_handle_status, (uint8_t*) &response, 1);	
                }
                break;
            }				
		}
    } else if ((p_evt_write->handle == p_mama->char_handle_session.value_handle) && (length >= 4)) {
        memcpy(&AppConfig.session, data, length);

		mama_ble_write_notify (p_mama, p_mama->char_handle_session, (uint8_t*) &AppConfig.session, 4); 
		
        AppData.updateConfig = true; 
    } else if ((p_evt_write->handle == p_mama->char_handle_interval.value_handle) && (length >= 2)) {
        memcpy(&AppConfig.interval, data, length);

		mama_ble_write_notify (p_mama, p_mama->char_handle_interval, (uint8_t*) &AppConfig.interval, 2);

		AppData.updateConfig = true; 
    } else if ((p_evt_write->handle == p_mama->char_handle_conf.value_handle) && (length == APP_CONFIG_MAX_CONFIGS)) {
        // mama_convert_data_to_config (data);

		uint8_t configs[APP_CONFIG_MAX_CONFIGS]; 
		// mama_convert_config_to_data(configs); 
		
		mama_ble_write_notify (p_mama, p_mama->char_handle_interval, (uint8_t*) configs, APP_CONFIG_MAX_CONFIGS); 
		
        AppData.updateConfig = true; 
    } else if ((p_evt_write->handle == p_mama->char_handle_time.value_handle) && (length == 6)) {
        new_time.year_bcd   = data[0]; 
		new_time.mo_bcd     = data[1]; 
		new_time.day_bcd    = data[2]; 
		new_time.hour_bcd   = data[3]; 
		new_time.min_bcd    = data[4]; 
		new_time.sec_bcd    = data[5];
        
		AppData.willUpdateTime = true; 
    } else {
        // unknown
    }
    
}

static void on_hvx_tx_complete(ble_mama_t * p_mama, ble_evt_t const * p_ble_evt) {
    // NOP
}

void mama_ble_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context) {
    if ((p_context == NULL) || (p_ble_evt == NULL)) {
        return;
    }

    ble_mama_t * p_nus = (ble_mama_t  *) p_context;

    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_nus, p_ble_evt);
            break;
        case BLE_GATTS_EVT_WRITE:
            on_write(p_nus, p_ble_evt);
            break;
        case BLE_GATTS_EVT_HVN_TX_COMPLETE:
            on_hvx_tx_complete(p_nus, p_ble_evt);
            break;
        default:
            // No implementation needed.
            break;
    }
}

uint32_t mama_ble_write_notify (ble_mama_t * p_mama, ble_gatts_char_handles_t characteristic, 
                                uint8_t *data, uint16_t length) {
    ble_gatts_hvx_params_t hvx_params; 

    // VERIFY_PARAM_NOT_NULL(p_mama);

    // Return if we are not connected. 
    if (p_mama->conn_handle == BLE_CONN_HANDLE_INVALID){
        return BLE_CONN_HANDLE_INVALID; 
    }

    // Notify Data
    memset(&hvx_params, 0, sizeof(hvx_params));
    hvx_params.handle = characteristic.value_handle;   
	hvx_params.p_data = data; 
    hvx_params.p_len  = &length; 
    hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
    
    uint32_t err_code = sd_ble_gatts_hvx(p_mama->conn_handle, &hvx_params);	
    
    return err_code; 
}

// void mama_convert_data_to_config (uint8_t* data) {
//     // Dont need, used for IR stuff
// } 

// void mama_convert_config_to_data (uint8_t* data) {
//     // Dont need, used for IR stuff
// } 
