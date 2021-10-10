/************************************************************************************************************************************* 
 * @file  	
 * @brief 	
 *************************************************************************************************************************************/


/************************************************************************************************************************************* 
 * @include  		
 *************************************************************************************************************************************/
/*** C Standard Specific ***/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/*** Nordic Specific ***/
#include "nrf_dfu_ble_svci_bond_sharing.h"
#include "nrf_svci_async_function.h"
#include "nrf_svci_async_handler.h"
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_power.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_delay.h"
#include "nrf_log_default_backends.h"
#include "nrf_bootloader_info.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_drv_saadc.h"
#include "app_error.h"
#include "nrfx_twim.h"
#include "nrfx_spim.h"
#include "nrfx_pwm.h"
#include "nrfx_uart.h"

/*** Bluetooth LE Specific ***/
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "app_timer.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_state.h"
#include "ble_dfu.h"
#include "ble_bas.h"
#include "ble_dis.h"
#include "nrf_ble_gatt.h"

/*** Library Specific ***/
#include "nrf_ble_qwr.h"
#include "fds.h"

/*** Components ***/
#include "ads1118.h"
#include "gfx.h"
#include "ssd1309.h"

/*** Application Specific ***/
#include "hal.h"
#include "ble_config.h"
#include "custom_board.h"
#include "etna.h"
#include "etna_bt.h"


/************************************************************************************************************************************* 
 * @prototypes  		
 *************************************************************************************************************************************/
static void app_adv_start(bool erase_bonds);                                       

/************************************************************************************************************************************* 
 * @variable  		
 *************************************************************************************************************************************/


NRF_BLE_GATT_DEF(m_gatt);
NRF_BLE_QWR_DEF(m_qwr);
BLE_ADVERTISING_DEF(m_advertising);
BLE_BAS_DEF(m_bas);


static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                           
// static ble_uuid_t m_adv_uuids[] = {{BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE}};


/************************************************************************************************************************************* 
 * @functions   		
 *************************************************************************************************************************************/
/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static bool app_shutdown_handler(nrf_pwr_mgmt_evt_t event)
{
    switch (event)
    {
        case NRF_PWR_MGMT_EVT_PREPARE_DFU:
            NRF_LOG_INFO("Power management wants to reset to DFU mode.");

            uint32_t err_code;
            err_code = nrf_sdh_disable_request();
            APP_ERROR_CHECK(err_code);
            err_code = app_timer_stop_all();
            APP_ERROR_CHECK(err_code);

            break;

        default:
            return true;
    }

    NRF_LOG_INFO("Power management allowed to reset to DFU mode.");
    return true;
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
NRF_PWR_MGMT_HANDLER_REGISTER(app_shutdown_handler, 0);
static void buttonless_dfu_sdh_state_observer(nrf_sdh_state_evt_t state, void * p_context)
{
    if (state == NRF_SDH_EVT_STATE_DISABLED)
    {
        nrf_power_gpregret2_set(BOOTLOADER_DFU_SKIP_CRC);
        nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
    }
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
NRF_SDH_STATE_OBSERVER(m_buttonless_dfu_state_obs, 0) =
{
    .handler = buttonless_dfu_sdh_state_observer,
};

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void app_adv_conf_get(ble_adv_modes_config_t * p_config)
{
    memset(p_config, 0, sizeof(ble_adv_modes_config_t));

    p_config->ble_adv_fast_enabled  = true;
    p_config->ble_adv_fast_interval = APP_ADV_INTERVAL;
    p_config->ble_adv_fast_timeout  = APP_ADV_DURATION;
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void app_disconnect(uint16_t conn_handle, void * p_context)
{
    UNUSED_PARAMETER(p_context);

    ret_code_t err_code = sd_ble_gap_disconnect(conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_WARNING("Failed to disconnect connection. Connection handle: %d Error: %d", conn_handle, err_code);
    }
    else
    {
        NRF_LOG_DEBUG("Disconnected connection handle %d", conn_handle);
    }
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void app_ble_dfu_evt_handler(ble_dfu_buttonless_evt_type_t event)
{
    switch (event)
    {
        case BLE_DFU_EVT_BOOTLOADER_ENTER_PREPARE:
        {
            NRF_LOG_INFO("Device is preparing to enter bootloader mode.");

            // Prevent device from advertising on disconnect.
            ble_adv_modes_config_t config;
            app_adv_conf_get(&config);
            config.ble_adv_on_disconnect_disabled = true;
            ble_advertising_modes_config_set(&m_advertising, &config);

            // Disconnect all other bonded devices that currently are connected.
            // This is required to receive a service changed indication
            // on bootup after a successful (or aborted) Device Firmware Update.
            uint32_t conn_count = ble_conn_state_for_each_connected(app_disconnect, NULL);
            NRF_LOG_INFO("Disconnected %d links.", conn_count);
            break;
        }

        case BLE_DFU_EVT_BOOTLOADER_ENTER:
            // YOUR_JOB: Write app-specific unwritten data to FLASH, control finalization of this
            //           by delaying reset by reporting false in app_shutdown_handler
            NRF_LOG_INFO("Device will enter bootloader mode.");
            break;

        case BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED:
            NRF_LOG_ERROR("Request to enter bootloader mode failed asynchroneously.");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
            break;

        case BLE_DFU_EVT_RESPONSE_SEND_ERROR:
            NRF_LOG_ERROR("Request to send a response to client failed.");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
            APP_ERROR_CHECK(false);
            break;

        default:
            NRF_LOG_ERROR("Unknown event from ble_dfu_buttonless.");
            break;
    }
}
/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{

    __disable_irq();
    NRF_LOG_FINAL_FLUSH();

    #ifndef DEBUG
    NRF_LOG_ERROR("MAMA Fatal error");
    #else
    switch (id)
    {
    #if defined(SOFTDEVICE_PRESENT) && SOFTDEVICE_PRESENT
        case NRF_FAULT_ID_SD_ASSERT:
            NRF_LOG_ERROR("SOFTDEVICE: ASSERTION FAILED");
            break;
        case NRF_FAULT_ID_APP_MEMACC:
            NRF_LOG_ERROR("SOFTDEVICE: INVALID MEMORY ACCESS");
            break;
    #endif
        case NRF_FAULT_ID_SDK_ASSERT:
        {
            assert_info_t * p_info = (assert_info_t *)info;
            NRF_LOG_ERROR("ASSERTION FAILED at %s:%u",
                          p_info->p_file_name,
                          p_info->line_num);
            break;
        }
        case NRF_FAULT_ID_SDK_ERROR:
        {
            error_info_t * p_info = (error_info_t *)info;
            NRF_LOG_ERROR("ERROR %u [%s] at %s:%u\r\nPC at: 0x%08x",
                          p_info->err_code,
                          nrf_strerror_get(p_info->err_code),
                          p_info->p_file_name,
                          p_info->line_num,
                          pc);
             NRF_LOG_ERROR("End of error report");
            break;
        }
        default:
            NRF_LOG_ERROR("UNKNOWN FAULT at 0x%08X", pc);
            break;
    }
    #endif

    NRF_BREAKPOINT_COND;
    // On assert, the system can only recover with a reset.

    #ifndef DEBUG
    NRF_LOG_WARNING("System reset");

    // mama_assert_end();  // End MAMA! 
    nrf_delay_ms(100); 
    NVIC_SystemReset();
    #else
    app_error_save_and_stop(id, pc, info);
    #endif // DEBUG
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void app_pm_evt_handler(pm_evt_t const * p_evt)
{
    pm_handler_on_pm_evt(p_evt);
    pm_handler_flash_clean(p_evt);

}
/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void app_timers_init(void)
{

    // Initialize timer module.
    uint32_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    // Create timers.

    /* YOUR_JOB: Create any timers to be used by the application.
                 Below is an example of how to create a timer.
                 For every new timer needed, increase the value of the macro APP_TIMER_MAX_TIMERS by
                 one.
       uint32_t err_code;
       err_code = app_timer_create(&m_app_timer_id, APP_TIMER_MODE_REPEATED, timer_timeout_handler);
       APP_ERROR_CHECK(err_code); */
}
/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void app_gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

	uint8_t name [16] = {0}; 
	ble_gap_addr_t ble_addr;
	sd_ble_gap_addr_get(&ble_addr);
	sprintf((char*) name, LOCAL_NAME,ble_addr.addr[1],ble_addr.addr[0]);
	
    err_code = sd_ble_gap_device_name_set(&sec_mode,(const uint8_t *)name,strlen( (char*) name));
    APP_ERROR_CHECK(err_code);

    /* YOUR_JOB: Use an appearance value matching the application's use case.
       err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_);
       APP_ERROR_CHECK(err_code); */

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}
/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void app_nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void app_service_init(void)
{
    
    uint32_t                  	err_code;
    nrf_ble_qwr_init_t        	qwr_init  = {0};
    ble_dfu_buttonless_init_t 	dfus_init = {0};
	ble_bas_init_t    			bas_init;
    ble_dis_init_t    			dis_init;

    // Initialize Queued Write Module.
    qwr_init.error_handler = app_nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    dfus_init.evt_handler = app_ble_dfu_evt_handler;

    err_code = ble_dfu_buttonless_init(&dfus_init);
    APP_ERROR_CHECK(err_code);
		
	/*** Battery Service ***/
    memset(&bas_init, 0, sizeof(bas_init));
    bas_init.evt_handler          = NULL;
    bas_init.support_notification = true;
    bas_init.p_report_ref         = NULL;
    bas_init.initial_batt_level   = 100;
    bas_init.bl_rd_sec        = SEC_OPEN;
    bas_init.bl_cccd_wr_sec   = SEC_OPEN;
    bas_init.bl_report_rd_sec = SEC_OPEN;
    err_code = ble_bas_init(&m_bas, &bas_init);
    APP_ERROR_CHECK(err_code);
		
	/*** Device Information ***/
	uint8_t serial [16] = {0};  
	ble_gap_addr_t ble_addr;
	sd_ble_gap_addr_get(&ble_addr);
	sprintf((char*) serial, "%02X%02X%02X%02X%02X%02X", ble_addr.addr[5], ble_addr.addr[4],ble_addr.addr[3],ble_addr.addr[2],ble_addr.addr[1],ble_addr.addr[0]);

    memset(&dis_init, 0, sizeof(dis_init));
    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, (char *)MANUFACTURER_NAME);
	ble_srv_ascii_to_utf8(&dis_init.model_num_str, (char *) MODEL);
	ble_srv_ascii_to_utf8(&dis_init.hw_rev_str, (char *)HARDWARE_VERSION);
	ble_srv_ascii_to_utf8(&dis_init.fw_rev_str, (char *)FIRMWARE_VERSION);
	ble_srv_ascii_to_utf8(&dis_init.serial_num_str, (char *)serial);
		
    dis_init.dis_char_rd_sec = SEC_OPEN;
    err_code = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(err_code);
		

}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void app_on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void app_conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void app_conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = app_on_conn_params_evt;
    cp_init.error_handler                  = app_conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void app_on_adv_evt(ble_adv_evt_t ble_adv_evt)
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

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void app_ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    uint32_t err_code = NRF_SUCCESS;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_DISCONNECTED:
            break;

        case BLE_GAP_EVT_CONNECTED:
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
            break;
        }

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        default:
            // No implementation needed.
            break;
    }
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void app_ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, app_ble_evt_handler, NULL);
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void app_peer_manager_init()
{
    pm_privacy_params_t  pri_param; 
	ble_gap_sec_params_t sec_param;
    ret_code_t           err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = SEC_PARAM_BOND;
    sec_param.mitm           = SEC_PARAM_MITM;
    sec_param.lesc           = SEC_PARAM_LESC;
    sec_param.keypress       = SEC_PARAM_KEYPRESS;
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
	
	pri_param.privacy_mode = BLE_GAP_PRIVACY_MODE_DEVICE_PRIVACY; 
	pri_param.private_addr_type = BLE_GAP_ADDR_TYPE_RANDOM_PRIVATE_RESOLVABLE; 
	pri_param.private_addr_cycle_s = 0; 
	pri_param.p_device_irk = NULL; 
	pm_privacy_set(&pri_param);
}


//static ble_uuid_t m_adv_uuids[] ={{BLE_UUID_IPSP_SERVICE, BLE_UUID_TYPE_BLE}};     
/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void app_adv_init(void)
{
    uint32_t               err_code;
    ble_advertising_init_t init;

    ble_uuid_t ble_uuid; 
    memset(&ble_uuid,0,sizeof(ble_uuid_t)); 
    //ble_uuid.type = m_mama.uuid_type; 
    //ble_uuid.uuid = MAMA_SERVICE_UUID; 

    memset(&init, 0, sizeof(init));

    init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance      = true;
    init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    //init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    //init.advdata.uuids_complete.p_uuids  = m_adv_uuids;
    //init.srdata.uuids_complete.uuid_cnt = 1; 
    //init.srdata.uuids_complete.p_uuids = &ble_uuid;

    app_adv_conf_get(&init.config);

    init.evt_handler = app_on_adv_evt;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void app_log_init(void)
{
    uint32_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void app_gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void app_adv_start(bool erase_bonds)
{

        uint32_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
        APP_ERROR_CHECK(err_code);

        NRF_LOG_DEBUG("advertising is started");

}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void app_power_management_init(void)
{
    uint32_t err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void app_idle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}


/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int main(void)
{ 
    app_log_init();
    NRF_LOG_INFO("");
    NRF_LOG_INFO("******************************************************");
    NRF_LOG_INFO("* MAMA Mano");
    NRF_LOG_INFO("* v1.0.0");
    NRF_LOG_INFO("******************************************************");
    
    uint32_t err_code = ble_dfu_buttonless_async_svci_init();
    APP_ERROR_CHECK(err_code);

    app_power_management_init();
    app_ble_stack_init();
    app_timers_init();

    // Config Application
    etna_init();  

	// Start Bluetooth 
    if (0)
    {
        app_peer_manager_init();
        app_gap_params_init();
        app_gatt_init();
        app_service_init();
        app_adv_init();
        app_conn_params_init();

        err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, m_advertising.adv_handle, 4);
        APP_ERROR_CHECK(err_code);
        
       app_adv_start(true);
    }


    // Enter main loop -> Application
    for (;;)
    {
        etna_run(); // all our sm 
    }
}

