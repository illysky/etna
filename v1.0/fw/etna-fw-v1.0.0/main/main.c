/*****************************************************************************************************************************************************************************************
 *
 * @file						
 * @brief 	
 * @version		
 * @author					
 * @date						
 *
*****************************************************************************************************************************************************************************************/

/****************************************************************************************************************************************************************************************
 * @include				
*****************************************************************************************************************************************************************************************/

// C
#include <stdint.h>
#include <string.h>

// Nordic
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_drv_twi.h"
#include "nrf_ble_gatt.h"
#include "nrf_drv_gpiote.h"
#include "nrf_drv_spi.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_saadc.h"
#include "app_timer.h"
#include "app_error.h"
#include "fds.h"
#include "fstorage.h"
#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "app_uart.h"
#include "app_fifo.h"

// Nordic BLE
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_bas.h"
#include "ble_hrs.h"
#include "ble_dis.h"
#include "ble_conn_params.h"
#include "ble_xpr.h"
#include "boards.h"
#include "softdevice_handler.h"
#include "bsp.h"
#include "bsp_btn_ble.h"
#include "ble_conn_state.h"
#include "peer_manager.h"

// Application
#include "app_config.h"
#include "hw_config.h"
#include "ads1118.h"

#include "arm_math.h"

/*****************************************************************************************************************************************************************************************
 * @globalvars				
*****************************************************************************************************************************************************************************************/
uint16_t								m_conn_handle = BLE_CONN_HANDLE_INVALID; 
ble_bas_t 							m_bas;                                   
nrf_ble_gatt_t 					m_gatt;    

const nrf_drv_spi_t APP_SPI0 = NRF_DRV_SPI_INSTANCE(0);  
const nrf_drv_spi_t APP_SPI1 = NRF_DRV_SPI_INSTANCE(1);  
const nrf_drv_timer_t APP_TIMER1 = NRF_DRV_TIMER_INSTANCE(1); 
const nrf_drv_timer_t APP_TIMER2 = NRF_DRV_TIMER_INSTANCE(2); 

/*****************************************************************************************************************************************************************************************
 * @localvars
*****************************************************************************************************************************************************************************************/
                   
//APP_TIMER_DEF(m_led_timer_id);   



#define PID_PARAM_KP        30.0f        /* Proporcional */
#define PID_PARAM_KI        1.0f        	/* Integral */
#define PID_PARAM_KD        0.36f      /* Derivative */

/*****************************************************************************************************************************************************************************************
 * @prototypes
*****************************************************************************************************************************************************************************************/



/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
static void app_pm_evt_handler(pm_evt_t const * p_evt)
{
    ret_code_t err_code;

    switch (p_evt->evt_id)
    {
        case PM_EVT_BONDED_PEER_CONNECTED:
        {
            NRF_LOG_INFO("Connected to a previously bonded device.\r\n");
        } break;

        case PM_EVT_CONN_SEC_SUCCEEDED:
        {
            NRF_LOG_INFO("Connection secured. Role: %d. conn_handle: %d, Procedure: %d\r\n",
                         ble_conn_state_role(p_evt->conn_handle),
                         p_evt->conn_handle,
                         p_evt->params.conn_sec_succeeded.procedure);
        } break;

        case PM_EVT_CONN_SEC_FAILED:
        {
            /* Often, when securing fails, it shouldn't be restarted, for security reasons.
             * Other times, it can be restarted directly.
             * Sometimes it can be restarted, but only after changing some Security Parameters.
             * Sometimes, it cannot be restarted until the link is disconnected and reconnected.
             * Sometimes it is impossible, to secure the link, or the peer device does not support it.
             * How to handle this error is highly application dependent. */
        } break;

        case PM_EVT_CONN_SEC_CONFIG_REQ:
        {
            // Reject pairing request from an already bonded peer.
            pm_conn_sec_config_t conn_sec_config = {.allow_repairing = false};
            pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
        } break;

        case PM_EVT_STORAGE_FULL:
        {
            // Run garbage collection on the flash.
            err_code = fds_gc();
            if (err_code == FDS_ERR_BUSY || err_code == FDS_ERR_NO_SPACE_IN_QUEUES)
            {
                // Retry.
            }
            else
            {
                APP_ERROR_CHECK(err_code);
            }
        } break;

        case PM_EVT_PEERS_DELETE_SUCCEEDED:
        {
           
        } break;

        case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
        {
            // The local database has likely changed, send service changed indications.
            pm_local_database_has_changed();
        } break;

        case PM_EVT_PEER_DATA_UPDATE_FAILED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peer_data_update_failed.error);
        } break;

        case PM_EVT_PEER_DELETE_FAILED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peer_delete_failed.error);
        } break;

        case PM_EVT_PEERS_DELETE_FAILED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peers_delete_failed_evt.error);
        } break;

        case PM_EVT_ERROR_UNEXPECTED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.error_unexpected.error);
        } break;

        case PM_EVT_CONN_SEC_START:
        case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
        case PM_EVT_PEER_DELETE_SUCCEEDED:
        case PM_EVT_LOCAL_DB_CACHE_APPLIED:
        case PM_EVT_SERVICE_CHANGED_IND_SENT:
        case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:
        default:
            break;
    }
}




/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
static void app_fds_evt_handler(fds_evt_t const * const p_evt)
{
    if (p_evt->id == FDS_EVT_GC)
    {
        NRF_LOG_DEBUG("GC completed\n");
    }
}

/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
static void app_gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t * p_evt)
{
   
}


/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
static void app_adv_evt_handler(ble_adv_evt_t ble_adv_evt)
{

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            break;
        case BLE_ADV_EVT_IDLE:
            break;
        default:
            break;
    }
}

/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
static void app_conn_params_evt_handler(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}

/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
static void app_conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
static void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
static void app_bluetooth_init (bool erase_bonds) 
{

		// Peer Manager Init **********************************************
		ble_gap_sec_params_t sec_param;
    ret_code_t           err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    if (erase_bonds)
    {
        err_code = pm_peers_delete();
        APP_ERROR_CHECK(err_code);
    }

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = SEC_PARAM_BOND;
    sec_param.mitm           = SEC_PARAM_MITM;
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob            = SEC_PARAM_OOB;
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc  = 1;
    sec_param.kdist_own.id   = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id  = 1;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(app_pm_evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = fds_register(app_fds_evt_handler);
    APP_ERROR_CHECK(err_code);
	
		// GAP CONFIG *************************************************************************************************
		// Init **********************************************
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;
		ble_gap_addr_t		deviceAddress;
		uint8_t buf[14];
		
		// Set Device Name ***********************************
		sd_ble_gap_addr_get(&deviceAddress);
		sprintf((char*) buf, DEVICE_NAME, deviceAddress.addr[1],deviceAddress.addr[0]);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)buf, strlen((const char*)buf));
    APP_ERROR_CHECK(err_code);

		// Set Appearance ************************************
    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_UNKNOWN);
    APP_ERROR_CHECK(err_code);
    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

		// Set Connection Intervals ***************************
    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

		// Set GAP *******************************************
    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
		sd_ble_gap_tx_power_set(4);

		// GAP CONFIG *************************************************************************************************
		// Init **********************************************
	  err_code = nrf_ble_gatt_init(&m_gatt, app_gatt_evt_handler);
		APP_ERROR_CHECK(err_code);

		// SERVICE CONFIG *********************************************************************************************
    ble_bas_init_t bas_init;
    ble_dis_init_t dis_init;
    memset(&bas_init, 0, sizeof(bas_init));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&bas_init.battery_level_char_attr_md.write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_report_read_perm);
		
		// Battery Service ************************************
    bas_init.evt_handler          = NULL;
    bas_init.support_notification = true;
    bas_init.p_report_ref         = NULL;
    bas_init.initial_batt_level   = 100;
    err_code = ble_bas_init(&m_bas, &bas_init);
    APP_ERROR_CHECK(err_code);

		// Device Info Service *********************************
    memset(&dis_init, 0, sizeof(dis_init));
		sd_ble_gap_addr_get(&deviceAddress);
		sprintf((char*) buf, "%02X%02X%02X%02X%02X%02X", deviceAddress.addr[5], deviceAddress.addr[4], deviceAddress.addr[3], deviceAddress.addr[2],deviceAddress.addr[1],deviceAddress.addr[0]); 
    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, (char *)MANUFACTURER_NAME);
		ble_srv_ascii_to_utf8(&dis_init.model_num_str, DEVICE_MODEL);
		ble_srv_ascii_to_utf8(&dis_init.serial_num_str, (char*) buf);
		ble_srv_ascii_to_utf8(&dis_init.fw_rev_str, DEVICE_FW);	
		ble_srv_ascii_to_utf8(&dis_init.hw_rev_str, DEVICE_HW);	
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dis_init.dis_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dis_init.dis_attr_md.write_perm);
    err_code = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(err_code);
		
		
		xpr_bluetooth_init (); 
		
		// ADV CONFIG ************************************************************************************************
    ble_advdata_t          	advdata;
    ble_adv_modes_config_t 	options;
		ble_uuid_t    					service_uuid;
		service_uuid.type=xpr_peripheral.uuid; 
		service_uuid.uuid=XPR_SERVICE_UUID; 

    // Build advertising data struct to pass into @ref ble_advertising_init.
    memset(&advdata, 0, sizeof(advdata));
    advdata.name_type               			= BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance     			 	= false;
    advdata.flags                   			= BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
		advdata.uuids_more_available.uuid_cnt = 1;
    advdata.uuids_more_available.p_uuids  = &service_uuid;
    memset(&options, 0, sizeof(options));
    options.ble_adv_fast_enabled  				= true;
    options.ble_adv_fast_interval 				= APP_ADV_INTERVAL;
		options.ble_adv_fast_timeout  				= 60;
    err_code = ble_advertising_init(&advdata, NULL, &options, app_adv_evt_handler, NULL);
    APP_ERROR_CHECK(err_code);
		
		// Connection Parameters *********************************************************************
    ble_conn_params_init_t cp_init;
    memset(&cp_init, 0, sizeof(cp_init));
    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = xpr_peripheral.data_characteristic.cccd_handle;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = app_conn_params_evt_handler;
    cp_init.error_handler                  = app_conn_params_error_handler;
    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
		

 }



/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
static void app_start_bluetooth_adv (void)
{
    ret_code_t err_code;
    err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
}
















/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
static void app_timers_init(void)
{
			uint32_t err_code;

//    // Initialize timer module.
			APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);


//		err_code = app_timer_create(&m_battery_timer_id,APP_TIMER_MODE_REPEATED,app_battery_timer);
//		APP_ERROR_CHECK(err_code);
	
}


/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
static void app_start_timers (void)
{
	
//	uint32_t err_code;
//	err_code = app_timer_start (m_led_timer_id, APP_TIMER_TICKS(100, APP_TIMER_PRESCALER), NULL); 
//	APP_ERROR_CHECK(err_code);
//	
//	
//	err_code = app_timer_start (m_battery_timer_id, APP_TIMER_TICKS(3000, APP_TIMER_PRESCALER), NULL); 
//	APP_ERROR_CHECK(err_code);
//	
	
}









/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
static void app_sleep (void)
{
    uint32_t err_code = bsp_indication_set(BSP_INDICATE_IDLE);
    APP_ERROR_CHECK(err_code);
    err_code = bsp_btn_ble_sleep_mode_prepare();
    APP_ERROR_CHECK(err_code);
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}



/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
static void app_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t err_code;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected\r\n");
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break; // BLE_GAP_EVT_CONNECTED

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected, reason %d\r\n",
            p_ble_evt->evt.gap_evt.params.disconnected.reason);
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            break; // BLE_GAP_EVT_DISCONNECTED

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.\r\n");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTC_EVT_TIMEOUT

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.\r\n");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTS_EVT_TIMEOUT
        case BLE_EVT_USER_MEM_REQUEST:
            err_code = sd_ble_user_mem_reply(m_conn_handle, NULL);
            APP_ERROR_CHECK(err_code);
            break; // BLE_EVT_USER_MEM_REQUEST

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        {
            ble_gatts_evt_rw_authorize_request_t  req;
            ble_gatts_rw_authorize_reply_params_t auth_reply;

            req = p_ble_evt->evt.gatts_evt.params.authorize_request;

            if (req.type != BLE_GATTS_AUTHORIZE_TYPE_INVALID)
            {
                if ((req.request.write.op == BLE_GATTS_OP_PREP_WRITE_REQ)     ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL))
                {
                    if (req.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
                    }
                    else
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
                    }
                    auth_reply.params.write.gatt_status = APP_FEATURE_NOT_SUPPORTED;
                    err_code = sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle,&auth_reply);
                    APP_ERROR_CHECK(err_code);
                }
            }
        } break; // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST


        default:
            // No implementation needed.
            break;
    }
}

/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
static void app_ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    ble_conn_state_on_ble_evt(p_ble_evt);
    pm_on_ble_evt(p_ble_evt);
    ble_bas_on_ble_evt(&m_bas, p_ble_evt);
		xpr_bluetooth_on_dispatch (p_ble_evt);
    ble_conn_params_on_ble_evt(p_ble_evt);
    bsp_btn_ble_on_ble_evt(p_ble_evt);
    app_ble_evt(p_ble_evt);
    ble_advertising_on_ble_evt(p_ble_evt);
    nrf_ble_gatt_on_ble_evt(&m_gatt, p_ble_evt);
}


/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
static void app_sys_evt_dispatch(uint32_t sys_evt)
{
    // Dispatch the system event to the fstorage module, where it will be
    // dispatched to the Flash Data Storage (FDS) module.
    fs_sys_event_handler(sys_evt);

    // Dispatch to the Advertising module last, since it will check if there are any
    // pending flash operations in fstorage. Let fstorage process system events first,
    // so that it can report correctly to the Advertising module.
    ble_advertising_on_sys_evt(sys_evt);
}

/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
static void app_ble_stack_init(void)
{
    uint32_t err_code;

    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

    ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(NRF_BLE_CENTRAL_LINK_COUNT,
                                                    NRF_BLE_PERIPHERAL_LINK_COUNT,
                                                    &ble_enable_params);
    APP_ERROR_CHECK(err_code);

    // Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(NRF_BLE_CENTRAL_LINK_COUNT, NRF_BLE_PERIPHERAL_LINK_COUNT);

    // Enable BLE stack.
#if (NRF_SD_BLE_API_VERSION == 3)
    ble_enable_params.gatt_enable_params.att_mtu = NRF_BLE_GATT_MAX_MTU_SIZE;
#endif
		ble_enable_params.common_enable_params.vs_uuid_count = 2; 
    err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_ble_evt_handler_set(app_ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_sys_evt_handler_set(app_sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);

}


/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
void app_power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}




/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
void app_saadc_handler (nrf_drv_saadc_evt_t const * p_event) 
{
	
}


#define APP_CC_MAX_HEATER_TICKS 18500
#define APP_CC_SAMPLE_TICKS 		18800
#define APP_CC_MAX_LED_MS 200



uint32_t test_ticks  = 0; 
uint32_t test_sample  = 0; 
uint32_t app_get_tip_temp (double* temperature);
#define LOOKUP_SIZE 51
double n_type_lookup_mv [LOOKUP_SIZE] = {0.000,0.261,0.525,0.793,1.065,1.340,1.619,1.902,2.189,2.480,2.774,3.072,3.374,3.680,3.989,4.302,4.618,4.937,5.259,5.585,5.913,6.245,6.579,6.916,7.255,7.597,7.941,8.288,8.637,8.988,9.341,9.696,10.054,10.413,10.774,11.136,11.501,11.867,12.234,12.603,12.974,13.346,13.719,14.094,14.469,14.848,15.225,15.604,15.984,16.366,16.748,}; 
double n_type_lookup_temp [LOOKUP_SIZE] = {0,10,20,30,40,50,60,70,80,90,100,110,120,130,140,150,160,170,180,190,200,210,220,230,240,250,260,270,280,290,300,310,320,330,340,350,360,370,380,390,400,410,420,430,440,450,460,470,480,490,500}; 
double thc_temperature = 0; 

double setpoint = 300.0f;
double setpoint_prev = 0.0f;
float duty = 0.0f;
bool tip_open_circuit = false; 
float raw_mV = 0;
float cjc_mV = 0;
	
arm_pid_instance_f32 pid;
	
	/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
void app_set_heater_duty (float duty) 
{
	// Set heater ticks
	float heater_ticks = (duty /  100.0) * APP_CC_MAX_HEATER_TICKS; 
	if (heater_ticks == 0)  heater_ticks = 1; 			// Keep at least one tick going
	nrf_drv_timer_extended_compare(&APP_TIMER1, NRF_TIMER_CC_CHANNEL0, heater_ticks, NULL, true);
	
	// Set led ticks

	 

}



	/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/

void app_led_timer_handler (nrf_timer_event_t event_type, void * p_context) 
{
	uint32_t result = 0; 
	
	switch (event_type)
	{
		
		// LED OFF
		case NRF_TIMER_EVENT_COMPARE0:
			LED0_OFF(); 
			break;
		
		// LED ON
		case NRF_TIMER_EVENT_COMPARE1:
			LED0_ON(); 
			break;
		default:
			break; 
	}
	
}


/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
void app_timer_handler (nrf_timer_event_t event_type, void * p_context) 
{
		uint32_t result = 0; 
	
    switch (event_type)
    {
        case NRF_TIMER_EVENT_COMPARE0:
						HEAT_PWM_OFF();
            break;
        case NRF_TIMER_EVENT_COMPARE1:
						
						test_ticks++;
				
						
						// 1. Turn Heater Off 
						HEAT_PWM_OFF();
						nrf_delay_us(1);
					
						// 2. Measure Temp
						result = app_get_tip_temp(&thc_temperature); 
				
						// 3. Check for open circuit on the thc
						if (result) {
							tip_open_circuit = true; 
							HEAT_PWM_OFF();							// Shutdown Heater
							arm_pid_reset_f32	(&pid); 	// Reset PID
							duty = 0; 									// Set Minimum Duty
							app_set_heater_duty (duty);	// Update PWM
							break; 								
						}
						else {
							tip_open_circuit = false;
						}

						// 4. Check for overtemp
						if (thc_temperature > 500) {
							HEAT_PWM_OFF();							// Shutdown Heater
							arm_pid_reset_f32	(&pid); 	// Reset PID
							duty = 0; 									// Set Minimum Duty
							app_set_heater_duty (duty);	// Update PWM
							break; 
						}
						
						// 5. Calculate error and run PID
						float error = ((float) setpoint) - ((float) thc_temperature); 
						if (error < -5)	error = 0; // Prevent excessive negative errors
						duty = arm_pid_f32(&pid, error);
						if (duty > 100.0)	duty = 100; 											// Saturate max ticks
						if (duty < 0.0)		duty = 0.0; 											// Prevent minus duty
						
						// 6. Test for heater timeout, if we haven't made substantial temperature increases in a period of time then shut it down
							
						// 7. All good, begin heating 
						if (duty > 0.0)		HEAT_PWM_ON();							// Start heater only if duty is not zero
						app_set_heater_duty(duty);										// Update PWM duty 
						
            break;
        default:
            break;
    }
}




/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
void app_hardware_init (void)
{
	
	uint32_t error = 0; 
	
	// GPIO Config ********************************************************
	LED0_TRIS(); 
	BUT0_TRIS(); 
	BUT1_TRIS(); 
	
	OLED_RST_TRIS(); 
	OLED_CSN_TRIS(); 
	OLED_DC_TRIS(); 

	HEAT_PWM_TRIS(); 
	HEAT_EN_TRIS(); 
	HEAT_PWM_OFF(); 
	
	LED0_ON();
	
	ADS1118_CS_TRIS(); 
	ADS1118_CS_DEASSERT(); 

	// Config SPI ********************************************************
  nrf_drv_spi_config_t spi0_config = NRF_DRV_SPI_DEFAULT_CONFIG;
  spi0_config.miso_pin = APP_SPI0_MISO;
  spi0_config.mosi_pin = APP_SPI0_MOSI;
  spi0_config.sck_pin  = APP_SPI0_SCK;
	spi0_config.frequency = NRF_DRV_SPI_FREQ_1M; 
	spi0_config.mode =  NRF_DRV_SPI_MODE_1; 
	APP_ERROR_CHECK(nrf_drv_spi_init(&APP_SPI0, &spi0_config, NULL));
	
	// Config SPI ********************************************************
  nrf_drv_spi_config_t spi1_config = NRF_DRV_SPI_DEFAULT_CONFIG;
  spi1_config.miso_pin = NRF_DRV_SPI_PIN_NOT_USED;
  spi1_config.mosi_pin = APP_SPI1_MOSI;
  spi1_config.sck_pin  = APP_SPI1_SCK;
	spi1_config.frequency = NRF_DRV_SPI_FREQ_1M; 
	spi1_config.mode =  NRF_DRV_SPI_MODE_0; 
	APP_ERROR_CHECK(nrf_drv_spi_init(&APP_SPI1, &spi1_config, NULL));
	
	// Config Heater Timer
	nrf_drv_timer_config_t timer1_config = NRF_DRV_TIMER_DEFAULT_CONFIG; 
	timer1_config.frequency = 4; 
	nrf_drv_timer_init	(&APP_TIMER1, &timer1_config, app_timer_handler); 
	nrf_drv_timer_extended_compare(&APP_TIMER1, NRF_TIMER_CC_CHANNEL0, 1, NULL, true);																												// When to shut off the heater
	nrf_drv_timer_extended_compare(&APP_TIMER1, NRF_TIMER_CC_CHANNEL1, APP_CC_SAMPLE_TICKS, NRF_TIMER_SHORT_COMPARE1_CLEAR_MASK, true);				// When to sample the thermocouple
	
	
	// Config LED Timer
	nrf_drv_timer_config_t timer2_config = NRF_DRV_TIMER_DEFAULT_CONFIG; 
	timer2_config.frequency = 6; 
	nrf_drv_timer_init	(&APP_TIMER2, &timer2_config, app_led_timer_handler); 
	uint32_t led_ticks_duty = nrf_drv_timer_ms_to_ticks(&APP_TIMER2, 20);
	uint32_t led_ticks_period = nrf_drv_timer_ms_to_ticks(&APP_TIMER2, APP_CC_MAX_LED_MS);
	nrf_drv_timer_extended_compare(&APP_TIMER2, NRF_TIMER_CC_CHANNEL0, led_ticks_duty, NULL, true);																												// When to shut off the heater
	nrf_drv_timer_extended_compare(&APP_TIMER2, NRF_TIMER_CC_CHANNEL1, led_ticks_period, NRF_TIMER_SHORT_COMPARE1_CLEAR_MASK, true);				// When to sample the thermocouple

}


/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
void app_button_event_handler (uint8_t pin_no, uint8_t button_action)
{  
	switch (pin_no){
		case BUT0_IO:
			break; 
		case BUT1_IO:
			break; 
		default:
			break; 
	}
}

/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
static const app_button_cfg_t app_buttons[2] =
{
    {BUT0_IO, APP_BUTTON_ACTIVE_LOW,  NRF_GPIO_PIN_PULLUP,  app_button_event_handler},
		{BUT1_IO, APP_BUTTON_ACTIVE_LOW,  NRF_GPIO_PIN_PULLUP,  app_button_event_handler}
};

/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
void app_button_register (void) 
{
		uint32_t test = app_button_init((app_button_cfg_t *)app_buttons, 2, BUTTON_DETECTION_DELAY);
		app_button_enable(); 
}




double app_lookup_temp	(double mV)
{
	// Locate n 
	uint32_t index = 0; 
	
	for (index = 0; index<LOOKUP_SIZE; index++) {
		if (n_type_lookup_mv[index] > mV) {
			break; 
		}
	}
	
	return n_type_lookup_temp[index-1] + (n_type_lookup_temp[index] - n_type_lookup_temp[index-1]) * ((mV -  n_type_lookup_mv[index-1])/(n_type_lookup_mv[index] - n_type_lookup_mv[index-1])); 

}

/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
double app_lookup_mV	(double temp)
{
	// Locate n 
	uint32_t index = 0; 
	
	for (index = 0; index<LOOKUP_SIZE; index++) {
		if (n_type_lookup_temp[index] > temp) {
			break; 
		}
	}
	
	return n_type_lookup_mv[index-1] + (n_type_lookup_mv[index] - n_type_lookup_mv[index-1]) * ((temp -  n_type_lookup_temp[index-1])/(n_type_lookup_temp[index] - n_type_lookup_temp[index-1])); 
}

/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
uint32_t app_get_tip_temp (double* temperature) 
{
	
	int16_t tmp = 0; 
	uint16_t config = 0; 
	double cj_voltage = 0; 
	double thc_voltage = 0; 
	
	// Get Readings 
	config = ADS1118_SINGLE_SHOT_ADC; 
	ads1118_transfer (&config, &tmp); 
	while (ads1118_conversion_complete() == 0);
	config = ADS1118_SINGLE_SHOT_INTERNAL_TEMPERATURE; 
	ads1118_transfer (&config, &tmp); 
	thc_voltage = ((double) tmp) * (7.8125 / 1000.0); 
	
	
	
	
	// Check for open circuit thc 
	if (thc_voltage > 200) {
		return 1; 
	}  
	
	// TODO: Check for grounded thc
	
	
	if (thc_voltage < 0.0f) thc_voltage = 0.0f; 
	while (ads1118_conversion_complete() == 0);
	config = ADS1118_SINGLE_SHOT_ADC;
	ads1118_transfer (&config, &tmp);
	cj_voltage = app_lookup_mV (((double) (tmp >> 2)) * 0.03125);  
	*temperature = app_lookup_temp ((thc_voltage + cj_voltage)); 
	
	raw_mV = thc_voltage; 
	cjc_mV = thc_voltage + cj_voltage; 
	
 	return 0;  
}



/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
void app_run_pid (void) 
{

	
	


}

/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
int main(void)
 {
  	app_timers_init();
	app_ble_stack_init();
	app_hardware_init();
	
	 
	pid.Kp = PID_PARAM_KP;        /* Proporcional */
  pid.Ki = PID_PARAM_KI;        /* Integral */
  pid.Kd = PID_PARAM_KD;        /* Derivative */
	 
	arm_pid_init_f32(&pid, 1);
	 
	nrf_drv_timer_enable(&APP_TIMER1);
	nrf_drv_timer_enable(&APP_TIMER2);
	while (1); 

	
	
	
	
	
	app_bluetooth_init(true);
  app_start_bluetooth_adv();

	// Enter main loop.
  for (;;)
	{
		if (NRF_LOG_PROCESS() == false)
		{
				app_power_manage();
		}
  }
}

