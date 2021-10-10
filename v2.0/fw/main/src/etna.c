

/*** C Standard ***/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "arm_math.h"

// *** Nordic App *** 
#include "app_timer.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_drv_saadc.h"
#include "nrf_fstorage.h"
#include "nrf_fstorage_sd.h"
#include "nrfx_twim.h"
#include "nrfx_spim.h"
#include "nrfx_timer.h"
#include "nrfx_uart.h"
#include "nrfx_nvmc.h"
#include "app_button.h"


/*** Modules  ***/
#include "hal.h"
#include "custom_board.h"
#include "etna.h"
#include "etna_bt.h"

#include "ads1118.h"
#include "gfx.h"
#include "ssd1309.h"

#include "core.h"
#include "Port.h"

static void _heater_enable (bool); 

#define ADS1118_SINGLE_SHOT_INTERNAL_TEMPERATURE 		0x859B
#define ADS1118_SINGLE_SHOT_ADC_THERMISTOR_DIFF			0x8BEB
#define ADS1118_SINGLE_SHOT_ADC_VUSB			        0xE5EB  // Single Ended Chan 2, 2.048 VREF, 860SPS 
#define ADS1118_SINGLE_SHOT_ADC_CURRENT			        0xF5EB  // Single Ended Chan 2, 2.048 VREF, 860SPS 

//#define ETNA_TEMP_ALPHA 	0.005f
#define ETNA_TEMP_ALPHA 	0.05f

static DevicePolicyPtr_t dpm;           /* Device policy manager pointer */
static Port_t ports[NUM_PORTS];         /* Initalize port objects for each port */

/*** Private Functions  ***/
static void _button_handler (uint8_t pin_no, uint8_t active); 

/*** Module Variables  ***/
static const app_button_cfg_t buttons [] = 
{
    {BUT_0_IO, APP_BUTTON_ACTIVE_LOW,  NRF_GPIO_PIN_PULLUP,  _button_handler},
	{BUT_1_IO, APP_BUTTON_ACTIVE_LOW,  NRF_GPIO_PIN_PULLUP,  _button_handler},	
};

APP_TIMER_DEF(m_sample_timer_id);

const nrfx_timer_t APP_TIMER1 = NRFX_TIMER_INSTANCE(1); 

// *** N-Type LUT *** //
double etna_ntype_lut_mv [ENTA_NTYPE_LUT_SIZE] = {0.000,0.261,0.525,0.793,1.065,1.340,1.619,1.902,2.189,2.480,2.774,3.072,3.374,3.680,3.989,4.302,4.618,4.937,5.259,5.585,5.913,6.245,6.579,6.916,7.255,7.597,7.941,8.288,8.637,8.988,9.341,9.696,10.054,10.413,10.774,11.136,11.501,11.867,12.234,12.603,12.974,13.346,13.719,14.094,14.469,14.848,15.225,15.604,15.984,16.366,16.748,}; 
double etna_ntype_lut_temp [ENTA_NTYPE_LUT_SIZE] = {0,10,20,30,40,50,60,70,80,90,100,110,120,130,140,150,160,170,180,190,200,210,220,230,240,250,260,270,280,290,300,310,320,330,340,350,360,370,380,390,400,410,420,430,440,450,460,470,480,490,500}; 
etna_heater_fsm_status_t m_etna_heater_fsm_state = {0}; 
etna_ccp_state_t m_etna_ccp_state = ETNA_CCP_STATE_WAIT; 

bool        m_contract_ready = false; 
bool        m_pdo_negotiated = false; 
uint32_t    pdo_ticks = 0;  

void handle_core_event(uint32_t event, uint8_t portid, void *usr_ctx, void *app_ctx)
{
    
    if (event & PD_NEW_CONTRACT)
    {
        NRF_LOG_INFO("PD_NEW_CONTRACT"); 
        /* handle event */
    }

    if (event & PD_CONTRACT_ALL)
    {
        NRF_LOG_INFO("PD_CONTRACT_ALL"); 
        m_contract_ready = true; 
    }

    if (event & PD_STATE_CHANGED)
    {
        NRF_LOG_INFO("PD_STATE_CHANGED"); 
        /* handle event */
    }
}

uint8_t button = 0; 

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void _button_handler (uint8_t pin_no, uint8_t active)
{  
    NRF_LOG_INFO("Button");
    if (pin_no == BUT_0_IO)
    {
        NRF_LOG_INFO("Hello");
    }

    switch (pin_no) 
    {
        case BUT_0_IO: 
        {
		    
            if (active) 
            {
               NRF_LOG_INFO("Press");
               button = 1; 
            }
            else 
            {
                NRF_LOG_INFO("Release");
                button = 0; 
            }
        }
    }
}



/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void _ccp_irq_handler (nrf_timer_event_t event_type, void * p_context) 
{
    switch (event_type)
    {
		case NRF_TIMER_EVENT_COMPARE0:
		{
            _heater_enable (false); 
            m_etna_ccp_state = ETNA_CCP_STATE_PAUSE;        // Shut off the heater ready for measurement
		}
        break;
        case NRF_TIMER_EVENT_COMPARE1:
		{			
            m_etna_ccp_state = ETNA_CCP_STATE_MEASURE;      // Signal that we are ready for measurement
		}
        break;
        default:
            break;
    }
}   

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void _timer_handler(void * p_context) 
{
    // Don't need to do anything here, hwe are just glad weve been woken up. 

}


/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
uint32_t app_2timer_ms (uint32_t ticks)
{
	// eg. (7 + 1) * 1000 / 32768
	//   = 8000 / 32768
	//   = 0.24414062
	float numerator = ((float)APP_TIMER_CONFIG_RTC_FREQUENCY  + 1.0f) * 1000.0f;
	float denominator = (float)APP_TIMER_CLOCK_FREQ;
	float ms_per_tick = numerator / denominator;
	uint32_t ms = ms_per_tick * ticks;
	return ms;
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static double _lut_mv (double t)
{
	// Locate n 
	uint32_t index = 0; 
	for (index = 0; index<ENTA_NTYPE_LUT_SIZE; index++) 
	{
		if (etna_ntype_lut_temp[index] > t) 
		{
			break; 
		}
	}
	return etna_ntype_lut_mv[index-1] + (etna_ntype_lut_mv[index] - etna_ntype_lut_mv[index-1]) * ((t -  etna_ntype_lut_temp[index-1])/(etna_ntype_lut_temp[index] - etna_ntype_lut_temp[index-1])); 
}


/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static double _lut_temp (double mv)
{
	// Locate n 
	uint32_t index = 0; 
	for (index = 0; index<ENTA_NTYPE_LUT_SIZE; index++) 
	{
		if (etna_ntype_lut_mv[index] > mv) 
		{
			break; 
		}
	}
	return etna_ntype_lut_temp[index-1] + (etna_ntype_lut_temp[index] - etna_ntype_lut_temp[index-1]) * ((mv -  etna_ntype_lut_mv[index-1])/(etna_ntype_lut_mv[index] - etna_ntype_lut_mv[index-1])); 
}


/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static uint32_t _get_tip (double* t) 
{
	
	int16_t raw = 0; 
	uint16_t conf = 0; 
	double v_cj = 0; 		// Cold Junc Voltage
	double v_thc = 0; 		// Thermocouple Voltage
	
	// 1. Start THC read on ADS1118
	conf = ADS1118_SINGLE_SHOT_ADC_THERMISTOR_DIFF; 
	ads1118_transfer (0, &conf, &raw); 
	while (ads1118_conv_done(0) == 0);

	// 2. Read the THC result and start internal temp read on ADS1118
	conf = ADS1118_SINGLE_SHOT_INTERNAL_TEMPERATURE; 
	ads1118_transfer (0, &conf, &raw); 
	v_thc = ((double) raw) * (7.8125 / 1000.0); 			// Get the voltage of THC
	if (v_thc < 0.0f) v_thc = 0.0f; 						// Remove negative voltages

	// 3. Check for open circuit THC
 	if (v_thc > 250) 
	{
		return 1; 					
	}
	// 4. TODO: Check for grounded thc

	// 5. Calculate compensated THC temperature
	while (ads1118_conv_done(0) == 0);						// Wait for reading to complete
	conf = ADS1118_SINGLE_SHOT_ADC_THERMISTOR_DIFF;			// Readback data for internal temperature sensor
	ads1118_transfer (0, &conf, &raw);				
	v_cj = _lut_mv (((double) (raw >> 2)) * 0.03125);  	// get the voltage of the internal temp
	*t = _lut_temp ((v_thc + v_cj)); 	 	            // compensate our thc reading
	
	//raw_mV = thc_voltage; 
	//cjc_mV = thc_voltage + cj_voltage; 
	
 	return 0;  
}




/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void _heater_enable (bool enable) 
{
	hal_gpio_write(HEATER_PWM_IO, enable); 
    //hal_gpio_write(HEATER_PWM_IO, 0); 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void _set_heater_duty (float duty) 
{
	uint32_t ticks = (uint32_t)(duty /  100.0) * ETNA_CC_MAX_HEATER_TICKS; 
	if (ticks == 0)  ticks = 1; 			
	nrfx_timer_compare (&APP_TIMER1, NRF_TIMER_CC_CHANNEL0, ticks, true);
	//nrf_drv_timer_extended_compare(&APP_TIMER1, NRF_TIMER_CC_CHANNEL0, ticks, NRF_TIMER_SHORT_COMPARE3_CLEAR_MASK, true);	
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/



/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void _hw_init (void) 
{

    hal_gpio_config_output(OLED_CS_IO, 1); 
	hal_gpio_config_output(OLED_RST_IO, 1); 
	hal_gpio_config_output(OLED_DC_IO, 1); 
	hal_gpio_config_output(ADS1118_CS_IO, 1); 
    hal_gpio_config_output(HEATER_PWM_IO, 0); 



	/*** I2C ***/
	hal_i2c_init (0, true);

	//***********************************************************************************************
	// SPI1  																				 
	//***********************************************************************************************
	hal_spi_init (1, SPI1_MISO_IO, SPI1_MOSI_IO, SPI1_SCK_IO, 0); 


	ssd1309_init (1, OLED_DC_IO, OLED_CS_IO, OLED_RST_IO); 
	gfx_init (128, 64, ssd1309_update_frame);	


    ads1118_init (0, 1, ADS1118_CS_IO, SPI1_MISO_IO); 

	// 1. Start THC read on ADS1118
    uint16_t conf = 0; 
    double vusb = 0; 		// Thermocouple Voltage
    int16_t raw = 0; 
	conf = ADS1118_SINGLE_SHOT_ADC_THERMISTOR_DIFF; 
	ads1118_transfer (0, &conf, &raw); 
	while (ads1118_conv_done(0) == 0);

	conf = ADS1118_SINGLE_SHOT_ADC_VUSB; 
	ads1118_transfer (0, &conf, &raw); 
    NRF_LOG_INFO("THC: %i", raw); 
	while (ads1118_conv_done(0) == 0);

    conf = ADS1118_SINGLE_SHOT_ADC_CURRENT; 
	ads1118_transfer (0, &conf, &raw); 
    vusb = ((double) raw) * (62.5 / 1000.0) * 10.95; 			// Get the voltage of THC
    char buf[32]; 
    sprintf(buf, "USB: %f", vusb); 
    NRF_LOG_INFO("%s", buf); 
	while (ads1118_conv_done(0) == 0);
	ads1118_transfer (0, &conf, &raw); 
    NRF_LOG_INFO("Current: %i", raw); 





} 



/************************************************************************************** 
 * @brief	
 * 	
 *************************************************************************************/
static void _run_pid (void)
{
    switch (m_etna_ccp_state)
    {
        // Heater is off
        case ETNA_CCP_STATE_WAIT: 
        {
            
        }
        break; 
        // Heater is paused
        case ETNA_CCP_STATE_PAUSE: 
        {
            m_etna_ccp_state = ETNA_CCP_STATE_WAIT; 
        }
        break; 
        // Heater is driving
        case ETNA_CCP_STATE_MEASURE: 
        {
            double t_raw = 0; 
            // 1. Turn heater off and settle (we did it before but well, who cares!)
            _heater_enable (false); 

            // 2. Check tip temperature
            if (_get_tip(&t_raw)) 
            {
                // Tip not present
                m_etna_heater_fsm_state.tool_inserted = false; 				// Tool removed
                arm_pid_reset_f32	(&m_etna_heater_fsm_state.pid);			// Reset PID algorithm
                m_etna_heater_fsm_state.tip_temp = 0; 						// Set tip temp tp zero
                m_etna_heater_fsm_state.heater_duty = 0;					// Set minium duty					
            }
            else 
            {
                // Tip Present
                m_etna_heater_fsm_state.tool_inserted = true; 		        // Tool inserted
                m_etna_heater_fsm_state.tip_temp = (ETNA_TEMP_ALPHA * t_raw) + (1.0f-ETNA_TEMP_ALPHA) * m_etna_heater_fsm_state.tip_temp; 
            }

            // 3. Check for overtemp
            if (m_etna_heater_fsm_state.tip_temp > 500 && m_etna_heater_fsm_state.tool_inserted) 
            {
                
                NRF_LOG_INFO("Overtemp %lu ", (uint32_t) m_etna_heater_fsm_state.tip_temp); 
                //m_etna_heater_fsm_state.overtemp = true; 
                //_heater_enable (false); 						            // Heater off
                //arm_pid_reset_f32	(&m_etna_heater_fsm_state.pid);	        // Reset PID algorithm
                //m_etna_heater_fsm_state.heater_duty = 0;				    // Set minium duty
            }

            // 4. Calculate error and run PID if we have tool inserted and we are not in overtemp fault
            if (m_etna_heater_fsm_state.tool_inserted && m_etna_heater_fsm_state.overtemp == false)
            {
                float error = ((float) m_etna_heater_fsm_state.setpoint) - ((float) m_etna_heater_fsm_state.tip_temp); 
                uint32_t duty = 0; 
                //if (error < -5)	error = 0; 								    // Prevent excessive negative errors
                duty = arm_pid_f32(&m_etna_heater_fsm_state.pid, error);	    // Get the duty required
                if (duty > 100.0)	duty = 100; 						        // Limit duty upper
                if (duty < 0.0)		duty = 0.0; 						        // Limit duty lower
                m_etna_heater_fsm_state.heater_duty = duty; 				    // Update our global duty
            }

            // 5. Todo: Test for heater timeout, if we haven't made substantial temperature increases in a period of time then shut it down
            
            // 8. All good, begin heating             
            if (m_etna_heater_fsm_state.heater_duty > 0.0)	
            {
                _heater_enable (true); 	// Heater on
            }	
            _set_heater_duty(m_etna_heater_fsm_state.heater_duty);				        // Update PWM duty 
            nrfx_timer_clear(&APP_TIMER1);
            m_etna_ccp_state = ETNA_CCP_STATE_WAIT; 
        }
        break; 

    }
}


/************************************************************************************** 
 * @brief	
 * 	
 *************************************************************************************/
void etna_init (void) 
{
    int32_t err_code; 

	_hw_init (); 

    // *** Timer ***
	err_code = app_timer_create(&m_sample_timer_id, APP_TIMER_MODE_REPEATED, _timer_handler);
    APP_ERROR_CHECK(err_code);
    app_timer_start(m_sample_timer_id, APP_TIMER_TICKS(1), NULL); 

	// *** UI ***
    uint32_t result = app_button_init((app_button_cfg_t *) buttons, sizeof(buttons)/sizeof(app_button_cfg_t), APP_TIMER_TICKS(50));
	if (result == 0) app_button_enable();
	nrf_delay_ms(50);

    // *** USB PD Negotiate 12V ***
    ports[0].PortID = 0;
    core_initialize(&ports[0], 0x24);
    core_enable_typec(&ports[0], TRUE);
    DPM_Init(&dpm);
    DPM_AddPort(dpm, &ports[0]);
    ports[0].dpm = dpm;
    register_observer(EVENT_ALL, &handle_core_event, 0);
    // Wait for contracts to arrive
    //gfx_clear(); 
    //gfx_write_string ("Wait PDO", &segoeUI_10ptFontInfo, 0, 64, 32, FONT_H_ALIGNMENT_CENTER, FONT_V_ALIGNMENT_MIDDLE); 
    //gfx_update();
    hal_timeout (&pdo_ticks, true, 250); 



	// Config Heater Timer
	nrfx_timer_config_t tmr1_conf; 
	tmr1_conf.frequency = NRF_TIMER_FREQ_1MHz; 
	tmr1_conf.mode =  NRF_TIMER_MODE_TIMER; 
    tmr1_conf.bit_width = NRF_TIMER_BIT_WIDTH_16; 
	tmr1_conf.interrupt_priority = 3; 
	
    nrfx_err_t error = 0; 

	error = nrfx_timer_init(&APP_TIMER1, &tmr1_conf, _ccp_irq_handler); 
    APP_ERROR_CHECK(error);
	nrfx_timer_compare(&APP_TIMER1, NRF_TIMER_CC_CHANNEL0, 1 , true);								// When to shut off the heater
	nrfx_timer_compare(&APP_TIMER1, NRF_TIMER_CC_CHANNEL1, ETNA_CC_SAMPLE_TICKS, true);				// When to sample the thermocouple
	//nrf_drv_timer_extended_compare(&APP_TIMER1, NRF_TIMER_CC_CHANNEL0, 1, NRF_TIMER_SHORT_COMPARE3_CLEAR_MASK, true);	
	//nrf_drv_timer_extended_compare(&APP_TIMER1, NRF_TIMER_CC_CHANNEL1, ETNA_CC_SAMPLE_TICKS, NRF_TIMER_SHORT_COMPARE1_CLEAR_MASK, true);		
	m_etna_heater_fsm_state.pid.Kp = ETNA_PID_PARAM_KP;        /* Proportional */
	m_etna_heater_fsm_state.pid.Ki = ETNA_PID_PARAM_KI;        /* Integral */
	m_etna_heater_fsm_state.pid.Kd = ETNA_PID_PARAM_KD;        /* Derivative */
	arm_pid_init_f32(&m_etna_heater_fsm_state.pid, 1);
	m_etna_heater_fsm_state.setpoint = 350.0;

}







/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void etna_run (void) 
{
    static uint32_t ticker = 0; 
    static double set_point = 350; 
    core_state_machine(&ports[0]);

    //nrfx_timer_enable(&APP_TIMER1);

    // Neogiate our PDO after a few seconds 
    if (hal_timeout (&pdo_ticks, false, 1500) == true && m_pdo_negotiated == false)
    {    
        for (uint8_t a=0; a<6; a++)
        {
            if (ports[0].SrcCapsReceived[a].PDO.SupplyType == 0)
            {
                uint32_t v = ports[0].SrcCapsReceived[a].FPDOSupply.Voltage * 50; 
                //NRF_LOG_INFO("Found PDO %u: Voltage %lumV", a, v); 
                if (v == 12000)
                {
                    v = v / 1000; 
                    NRF_LOG_INFO("Found Matching PDO %u: Voltage %lumV", a, v); 
                    ports[0].SinkRequest.FVRDO.MinMaxCurrent = 300; 
                    ports[0].SinkRequest.FVRDO.OpCurrent = 300; 
                    ports[0].SinkRequest.FVRDO.UnChnkExtMsgSupport = 0;
                    ports[0].SinkRequest.FVRDO.NoUSBSuspend = 1;
                    ports[0].SinkRequest.FVRDO.USBCommCapable = 0;
                    ports[0].SinkRequest.FVRDO.CapabilityMismatch = 0;
                    ports[0].SinkRequest.FVRDO.GiveBack = 0;
                    ports[0].SinkRequest.FVRDO.ObjectPosition = a+1; // Set the PDO we are using
                    SetPEState(&ports[0],peSinkSelectCapability);
                    m_pdo_negotiated = true; 
                    nrfx_timer_enable(&APP_TIMER1);
                } 
            }
        }
    }

    // Start Loop
    if (m_pdo_negotiated)
    {
        char buf [32]; 
        _run_pid ();        // Run the pid

        m_etna_heater_fsm_state.setpoint = set_point; 

        if (hal_timeout (&ticker, false, 500))
        {
            //etna_serial_send ((float) etna_heater_fsm_state.setpoint, (float) etna_heater_fsm_state.tip_temp); 
            
            float reading = m_etna_heater_fsm_state.tip_temp;
            if ( fabs(m_etna_heater_fsm_state.tip_temp - m_etna_heater_fsm_state.setpoint) < 3) reading = m_etna_heater_fsm_state.setpoint; 
                
            gfx_clear();
            sprintf(buf, "Set: %.0f", set_point); 		
            gfx_write_string (buf, &segoeUI_10ptFontInfo, 0, 64, 16, FONT_H_ALIGNMENT_CENTER, FONT_V_ALIGNMENT_MIDDLE); 
            sprintf(buf, "Act: %.0f", reading);
            gfx_write_string (buf, &segoeUI_10ptFontInfo, 0, 64, 32, FONT_H_ALIGNMENT_CENTER, FONT_V_ALIGNMENT_MIDDLE); 
            sprintf(buf, "Duty: %lu%%", m_etna_heater_fsm_state.heater_duty);
            gfx_write_string (buf, &segoeUI_10ptFontInfo, 0, 64, 50, FONT_H_ALIGNMENT_CENTER, FONT_V_ALIGNMENT_MIDDLE); 
            gfx_update();  

        }
    }
    else 
    { 
        if (hal_timeout (&ticker, false, 250))
        {
                
            gfx_clear();
            gfx_write_string ("PDO Wait", &segoeUI_10ptFontInfo, 0, 64, 32, FONT_H_ALIGNMENT_CENTER, FONT_V_ALIGNMENT_TOP); 
            gfx_update();  

        }
    }
}
