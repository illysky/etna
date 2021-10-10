#include "platform.h"
#include "hal.h"
#include "app_timer.h"

#define TICK_SCALE_TO_MS 1

uint32_t app_timer_ms(uint32_t ticks)
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


void platform_set_vbus_lvl_enable(FSC_U8 port,
                                  VBUS_LVL level,
                                  FSC_BOOL enable,
                                  FSC_BOOL disableOthers)
{
    unsigned int i;

    /* Additional VBUS levels can be added here as needed. */
    switch (level) {
    case VBUS_LVL_5V:
        /* Enable/Disable the 5V Source */

        break;
    default:
        /* Otherwise, do nothing. */
        break;
    }

    /* Turn off other levels, if requested */
    if (disableOthers || ((level == VBUS_LVL_ALL) && (enable == FALSE)))
    {
        i = 0;

        do {
            /* Skip the current level */
            if( i == level ) continue;

            /* Turn off the other level(s) */
            // platform_set_vbus_lvl_enable( i, FALSE, FALSE );
        } while (++i < VBUS_LVL_ALL);
    }
}

FSC_BOOL platform_get_vbus_lvl_enable(FSC_U8 port, VBUS_LVL level)
{
    /* Additional VBUS levels can be added here as needed. */
    switch (level) {
    case VBUS_LVL_5V:
        /* Return the state of the 5V VBUS Source. */

        break;
    default:
        /* Otherwise, return FALSE. */
        break;
    }

    return FALSE;
}

void platform_set_vbus_discharge(FSC_U8 port, FSC_BOOL enable)
{
    /* Enable/Disable the discharge path */
    /* TODO - Implement as needed on platforms that support this feature. */
}

FSC_BOOL platform_get_device_irq_state(FSC_U8 port)
{
    /* Return the state of the device interrupt signal. */
    return TRUE;
}

FSC_BOOL platform_i2c_write(FSC_U8 SlaveAddress,
                            FSC_U8 RegAddrLength,
                            FSC_U8 DataLength,
                            FSC_U8 PacketSize,
                            FSC_U8 IncSize,
                            FSC_U32 RegisterAddress,
                            FSC_U8* Data)
{
    /* Write some data! */

    /*
     * Return TRUE if successful,
     * Return FALSE otherwise.
     */

    uint8_t result = -1;
    uint8_t buf[32]; 
    buf[0] = (uint8_t) RegisterAddress;
    memcpy(&buf[1], Data, DataLength); 
    result = hal_i2c_write (0, SlaveAddress, buf, DataLength+RegAddrLength, false);
    //NRF_LOG_INFO("Write");
    //NRF_LOG_HEXDUMP_DEBUG(buf, DataLength+RegAddrLength);
    return ((result == 0) ? TRUE : FALSE);
}

FSC_BOOL platform_i2c_read( FSC_U8 SlaveAddress,
                            FSC_U8 RegAddrLength,
                            FSC_U8 DataLength,
                            FSC_U8 PacketSize,
                            FSC_U8 IncSize,
                            FSC_U32 RegisterAddress,
                            FSC_U8* Data)
{
    /* Read some data! */

    /*
     * Return TRUE if successful,
     * Return FALSE otherwise.
     */

    uint8_t reg = (uint8_t) RegisterAddress; 
    uint8_t result = -1;
    if (hal_i2c_write(0, SlaveAddress, &reg, RegAddrLength, true) == 0)
    {
        result = hal_i2c_read (0, SlaveAddress, Data, DataLength);
       // NRF_LOG_INFO("Read Addr %02X", reg);
       //NRF_LOG_HEXDUMP_DEBUG(Data, DataLength);
    }
    return ((result == 0) ? TRUE : FALSE);
}

/*****************************************************************************
* Function:        platform_delay_10us
* Input:           delayCount - Number of 10us delays to wait
* Return:          None
* Description:     Perform a software delay in intervals of 10us.
******************************************************************************/
void platform_delay_10us(FSC_U32 delayCount)
{
    /*fusb_Delay10us(delayCount); */
    hal_delay_us(10*delayCount); 
}

void platform_set_pps_voltage(FSC_U8 port, FSC_U32 mv)
{
}

FSC_U16 platform_get_pps_voltage(FSC_U8 port)
{
    return 0;
}

void platform_set_pps_current(FSC_U8 port, FSC_U32 ma)
{
}

FSC_U16 platform_get_pps_current(FSC_U8 port)
{
    return 0;
}

FSC_U32 platform_get_system_time(void)
{
    uint32_t time = app_timer_ms(app_timer_cnt_get());
    return time; 
}

FSC_U32 platform_get_log_time(void){
    uint32_t time = app_timer_ms(app_timer_cnt_get());
    return time; 
}