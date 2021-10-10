#include "nrfx_twim.h"
#include "nrfx_spi.h"
#include "hal.h"
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_power.h"
#include "nrf_log.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "custom_board.h"
#include "nrfx_twim.h"
#include "app_timer.h"

//#define HAL_I2C0_ENABLED 0
//#define HAL_I2C1_ENABLED 0
//#define HAL_SPI0_ENABLED 1
#define HAL_SPI1_ENABLED 1

const nrfx_twim_t   m_twi0 = NRFX_TWIM_INSTANCE(0); 
const nrfx_spi_t    m_spi1 = NRFX_SPI_INSTANCE(1); 
nrfx_spi_config_t   m_spi_conf[4] = {0};

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t hal_i2c_init (uint8_t instance, bool list) 
{
    const nrfx_twim_t* twi = NULL; 
    int32_t err_code = 0;
    uint8_t sda = 0xFF; 
    uint8_t sck = 0xFF; 
    switch (instance)
    {
        case 0: 
        {
            twi = &m_twi0; 
            sda = I2C0_SDA_IO; 
            sck = I2C0_SCL_IO;
        }
        break; 
        default: 
        {
            return 1; 
        }
    }


    nrfx_twim_bus_recover(sck, sda);
    const nrfx_twim_config_t app_i2c_conf = 
    {
        .scl                = sck,
        .sda                = sda,
        .frequency          = NRF_TWIM_FREQ_400K,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
        .hold_bus_uninit     = true
    };

    err_code = nrfx_twim_init(twi, &app_i2c_conf, NULL, NULL);
    APP_ERROR_CHECK(err_code);
    nrfx_twim_enable(twi);
    
    if (list == true)
    {
        NRF_LOG_INFO("*** I2C%u Device List ***",instance);
        for (uint32_t a=0; a<128; a++)
        {
            uint8_t dummy = 0; 
            ret_code_t err_code = nrfx_twim_tx (twi, a, &dummy, 1, false);
            if (err_code == 0) 
            {
                NRF_LOG_INFO("I2C Device @ 0x%02X", a);  
            }
        }
    }

    return 0;
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t hal_i2c_read (uint8_t instance, uint8_t device, uint8_t* data, uint32_t size) 
{
    int32_t result = 0;
    const nrfx_twim_t* twi = NULL; 
	switch (instance)
    {
        case 0: 
        {
            twi = &m_twi0; 
        }
        break; 
        default: 
        {
            return 1; 
        }
    }
    
    ret_code_t err_code = nrfx_twim_rx(twi, device, data, size);
    if (err_code) 
    {
        result = -1;  
    }
    return result;
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t hal_i2c_write (uint8_t instance, uint8_t device, uint8_t* data, uint32_t size, bool nostop) 
{
    const nrfx_twim_t* twi = NULL; 
	switch (instance)
    {
        case 0: 
        {
            twi = &m_twi0; 
        }
        break; 
        default: 
        {
            return 1; 
        }
    }
    
    int32_t result = 0;
    ret_code_t err_code = nrfx_twim_tx (twi, device, data, size, nostop);
    if (err_code) 
    {
        result = -1;  
    }
    return result; 
}



/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t hal_spi_init (uint8_t spi, uint8_t miso, uint8_t mosi, uint8_t sck, uint8_t mode)
{
    
    
    nrfx_spi_t* hal_spi = NULL; 

    m_spi_conf[spi].sck_pin = sck; 
    m_spi_conf[spi].mosi_pin = mosi; 
    m_spi_conf[spi].miso_pin = miso; 
    m_spi_conf[spi].ss_pin = NRFX_SPI_PIN_NOT_USED; 
    m_spi_conf[spi].irq_priority = NRFX_SPI_DEFAULT_CONFIG_IRQ_PRIORITY; 
    m_spi_conf[spi].orc = 0xFF; 
    m_spi_conf[spi].frequency = NRF_SPI_FREQ_4M; 
    m_spi_conf[spi].mode = mode; 
    m_spi_conf[spi].bit_order = NRF_SPI_BIT_ORDER_MSB_FIRST;

    switch (spi)
    {
     #ifdef HAL_SPI0_ENABLED
        case 0: 
        {
            hal_spi = (nrfx_spi_t*) &m_spi0; 
        }
        break; 
    #endif
    #ifdef HAL_SPI1_ENABLED
        case 1: 
        {
            hal_spi = (nrfx_spi_t*) &m_spi1;     
        }
        break; 
    #endif
        default: 
        {
            // Not Implemented
            return 1; 
        }
    }

    int32_t err_code = 0; 
    err_code =  nrfx_spi_init(hal_spi, &m_spi_conf[spi], NULL, NULL);
    APP_ERROR_CHECK(err_code);
    return err_code;
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t hal_spi_transfer (uint8_t spi, uint8_t* tx, uint32_t tx_size, uint8_t* rx, uint32_t rx_size, uint8_t mode)
{

    
    nrfx_spi_t* hal_spi = NULL; 
    switch (spi)
    {
        case 0: 
        {
#ifdef HAL_SPI0_ENABLED
            hal_spi = (nrfx_spi_t*)&m_spi0; 
#endif
        }
        break; 
        case 1: 
        {
#ifdef HAL_SPI1_ENABLED
            hal_spi = (nrfx_spi_t*)&m_spi1; 
#endif
        }
        break; 
        default: 
        {
            return 1; 
        }
    }

    // Mode changed so lets reconfigure
    if (m_spi_conf[spi].mode != mode)
    {
        m_spi_conf[spi].mode = mode; 
        nrfx_spi_uninit (hal_spi); 
        int32_t err_code = 0; 
        err_code =  nrfx_spi_init(hal_spi, &m_spi_conf[spi], NULL, NULL);
        APP_ERROR_CHECK(err_code); 
    }


    
    nrfx_spi_xfer_desc_t transfer = NRFX_SPI_XFER_TRX(tx, tx_size, rx, rx_size); 
    int32_t error =  nrfx_spi_xfer(hal_spi, &transfer, 0);
    if (error == 0) 
    {
        return 0; 
    }
    else 
    {
        return -1; 
    }
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void hal_gpio_config_output (uint32_t pin, uint32_t init) 
{
    nrf_gpio_cfg_output(pin);
    nrf_gpio_pin_write(pin, init);
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void hal_gpio_config_input (uint32_t pin, uint32_t pull) 
{
    nrf_gpio_cfg_input(pin, pull); 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void hal_gpio_write (uint32_t pin, uint32_t value) 
{
   nrf_gpio_pin_write(pin, value); 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void hal_gpio_tog (uint32_t pin) 
{
   nrf_gpio_pin_toggle(pin); 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
uint32_t hal_gpio_read (uint32_t pin) 
{
   return nrf_gpio_pin_read(pin); 
}
/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void hal_delay_ms (uint32_t ms) 
{
    nrf_delay_ms(ms); 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void hal_delay_us (uint32_t us) 
{
    nrf_delay_us(us); 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
bool hal_timeout (uint32_t* timestamp, bool init, uint32_t expiry) 
{
	uint32_t difference = 0; 
	bool expired = false;
	
	// We want to initialise out timeout
	if (init) 
	{
		*timestamp = app_timer_cnt_get(); 
		return false; 
	}


	// Compare the difference to see if we have expired and set flag
	difference = app_timer_cnt_diff_compute(app_timer_cnt_get(),*timestamp);
	if (difference > APP_TIMER_TICKS(expiry) || expiry == 0)
	{
		expired = true; 
		*timestamp = app_timer_cnt_get(); 
	}
	
	return expired; 
}


