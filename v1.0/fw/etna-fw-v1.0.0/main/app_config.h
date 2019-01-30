/**
 * @file        		app_cfg.h 
 * @brief           Configures the application
 * @author         	Illysky Ltd
 * @project         Insight
 *
*/

#ifndef __app_cfg_h
#define __app_cfg_h


#define IS_SRVC_CHANGED_CHARACT_PRESENT  1                                           /**< Include or not the service_changed characteristic. if not enabled, the server's database cannot be changed for the lifetime of the device*/

#define MANUFACTURER_NAME                "Xelba"                       			 /**< Manufacturer. Will be passed to Device Information Service. */
#define DEVICE_NAME              				 "XP-%02X%02X"                             	 /**< Name of device. Will be included in the advertising data. */
#define DEVICE_MODEL										 "Probe"
#define DEVICE_FW												 "v1.0.0"
#define DEVICE_HW												 "v1.0.0"

#define APP_ADV_INTERVAL                 300                                         /**< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS       30                                         /**< The advertising timeout in units of seconds. */

#define APP_TIMER_PRESCALER              0                                           /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE          4                                           /**< Size of timer operation queues. */

#define BUTTON_DETECTION_DELAY     			 APP_TIMER_TICKS(50, APP_TIMER_PRESCALER)   /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */

#define BATTERY_LEVEL_MEAS_INTERVAL      APP_TIMER_TICKS(2000, APP_TIMER_PRESCALER)  /**< Battery level measurement interval (ticks). */
#define MIN_BATTERY_LEVEL                81                                          /**< Minimum simulated battery level. */
#define MAX_BATTERY_LEVEL                100                                         /**< Maximum simulated 7battery level. */
#define BATTERY_LEVEL_INCREMENT          1                                           /**< Increment between each simulated battery level measurement. */


#define MIN_CONN_INTERVAL                MSEC_TO_UNITS(25, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (0.4 seconds). */
#define MAX_CONN_INTERVAL                MSEC_TO_UNITS(70, UNIT_1_25_MS)            /**< Maximum acceptable connection interval (0.65 second). */
#define SLAVE_LATENCY                    0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                 MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY    APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT     3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_BOND                   1                                           /**< Perform bonding. */
#define SEC_PARAM_MITM                   0                                           /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                   0                                           /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS               0                                           /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES        BLE_GAP_IO_CAPS_NONE                        /**< No I/O capabilities. */
#define SEC_PARAM_OOB                    0                                           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE           7                                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE           16                                          /**< Maximum encryption key size. */

#define DEAD_BEEF                        0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define APP_FEATURE_NOT_SUPPORTED        BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2        /**< Reply when unsupported features are requested. */



#endif /* __stdint_h */

