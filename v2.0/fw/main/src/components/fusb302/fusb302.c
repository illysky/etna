/************************************************************************************************************************************* 
 * @file  	
 * @brief 	
 *************************************************************************************************************************************/

#include "hal.h"

#include "fusb302.h" 
#include "usb_pd_tcpm.h"
#include "tcpm.h"
#include "usb_pd.h"


static uint8_t _i2c = 0x00; 
static uint8_t _addr = 0x00; 
static fusb302_state_t _state; 

#define FUSB302_PACKET_IS_GOOD_CRC(head) (PD_HEADER_TYPE(head) == PD_CTRL_GOOD_CRC && PD_HEADER_CNT(head) == 0)
/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static int32_t _write8 (uint8_t reg, uint8_t data)
{
    uint8_t buf[2]; 
	buf[0] = reg; 
	buf[1] = data; 
    return hal_i2c_write (_i2c, _addr, buf, 2, false); 
} 


/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static int32_t _write_array (uint8_t* data, uint32_t size, bool nostop)
{
    return hal_i2c_write (_i2c, _addr, data, size, nostop); 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static int32_t _read_array (uint8_t* data, uint32_t size)
{
    return hal_i2c_read (_i2c, _addr, data, size); 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static int32_t _read8 (uint8_t reg, uint8_t *data)
{
    int32_t result = -1;
    if (hal_i2c_write (_i2c, _addr, &reg, 1, true) == 0) 
    {
        result = hal_i2c_read (_i2c,_addr, data, 1);  
    }
	return result; 
} 


/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static int32_t _setbits (uint8_t reg, uint8_t value, uint8_t pos, uint8_t width)
{
    int32_t result = -1; 
    uint8_t tmp = 0; 
    if (_read8 (reg, &tmp) == 0)                        // Read
    {
        uint8_t mask = FUSB302_BIT_MASK(uint8_t, width);    // Make a mask
        value &= mask;                                      // Mask off the value
        tmp &= ~(mask << pos);                              // Clear      
        tmp |= (value << pos);                              // Modify
        if (_write8 (reg, tmp) == 0) result = 0;            // Write
    }
    return result; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static int32_t _pd_reset (void)
{
    return _write8(FUSB302_REG_RESET, 0x02);
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static int32_t _sw_reset (void)
{
    return _write8(FUSB302_REG_RESET, 0x01);  
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static int32_t _get_devid (uint8_t* id)
{
    return _read8 (FUSB302_REG_DEVICE_ID, id); 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void _flush_rx_fifo(void)
{
	/*
	 * other bits in the register _should_ be 0
	 * until the day we support other SOP* types...
	 * then we'll have to keep a shadow of what this register
	 * value should be so we don't clobber it here!
	 */
	_write8(FUSB302_REG_CTRL_1, FUSB302_REG_CTRL_1_RX_FLUSH);
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void _flush_tx_fifo(void)
{
	uint8_t tmp;
	_read8(FUSB302_REG_CTRL_0, &tmp);
	tmp |= FUSB302_REG_CTRL_0_TX_FLUSH;
	_write8(FUSB302_REG_CTRL_0, tmp);
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void _auto_goodcrc_enable(uint32_t enable)
{
	uint8_t tmp;

	_read8(FUSB302_REG_SWITCH_1, &tmp);

	if (enable)
    {
	    tmp |= FUSB302_REG_SWITCH_1_AUTO_GCRC;    
    }
    else 
    {
        tmp &= ~FUSB302_REG_SWITCH_1_AUTO_GCRC;
    }

	_write8(FUSB302_REG_SWITCH_1, tmp);
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static int32_t _rx_fifo_is_empty(void)
{
	uint8_t tmp; 
    _read8(FUSB302_REG_STATUS_1, &tmp); 
    return (tmp & FUSB302_REG_STATUS_1_RX_EMPTY); 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static int32_t _enable_auto_retry (bool enabled, uint8_t retries)
{
    int32_t result = 0; 
    if (enabled)
    {
        if (_setbits (FUSB302_REG_CTRL_3, 1, 0, 1)!=0)          return -1;     // Auto Retry ON
        if (_setbits (FUSB302_REG_CTRL_3, retries, 1, 2)!=0)    return -1;     // Retries
    }
    else 
    {
        if (_setbits (FUSB302_REG_CTRL_3, 0, 0, 1)!=0)          return -1;     // Auto Retry ON
        if (_setbits (FUSB302_REG_CTRL_3, 0, 1, 2)!=0)          return -1;     // Retries
    }
    return result; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static int32_t _enable_cc_pdwn (bool cc1, bool cc2)
{
    if (cc1)
    {
        if (_setbits (FUSB302_REG_SWITCH_0, 1, 0, 1)!=0) return -1;        
    }
    else 
    {
        if (_setbits (FUSB302_REG_SWITCH_0, 0, 0, 1)!=0) return -1;   
    }

    if (cc2)
    {
        if (_setbits (FUSB302_REG_SWITCH_0, 1, 1, 1)!=0) return -1;   
    }
    else 
    {
        if (_setbits (FUSB302_REG_SWITCH_0, 0, 1, 1)!=0) return -1;   
    } 

    return 0; 
}


/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static uint32_t _get_num_bytes(uint16_t header)
{
	uint32_t rv;
	rv = PD_HEADER_CNT(header);
	rv *= 4;
	rv += 2;
	return rv;
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static int32_t _send_message(uint16_t header, const uint32_t *data, uint8_t *buf, int buf_pos)
{
	uint32_t rv;
	uint32_t reg;
	uint32_t len;

	len = _get_num_bytes(header);

	/*
	 * packsym tells the TXFIFO that the next X bytes are payload,
	 * and should not be interpreted as special tokens.
	 * The 5 LSBs represent X, the number of bytes.
	 */
	reg = fusb302_TKN_PACKSYM;
	reg |= (len & 0x1F);

	buf[buf_pos++] = reg;

	/* write in the header */
	reg = header;
	buf[buf_pos++] = reg & 0xFF;

	reg >>= 8;
	buf[buf_pos++] = reg & 0xFF;

	/* header is done, subtract from length to make this for-loop simpler */
	len -= 2;

	/* write data objects, if present */
	memcpy(&buf[buf_pos], data, len);
	buf_pos += len;

	/* put in the CRC */
	buf[buf_pos++] = fusb302_TKN_JAMCRC;

	/* put in EOP */
	buf[buf_pos++] = fusb302_TKN_EOP;

	/* Turn transmitter off after sending message */
	buf[buf_pos++] = fusb302_TKN_TXOFF;

	/* Start transmission */
	reg = fusb302_TKN_TXON;
	buf[buf_pos++] = fusb302_TKN_TXON;

	/* burst write for speed! */
    rv = _write_array(buf, buf_pos, false); 

	return rv;
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static int32_t _set_power(uint8_t power)
{   
    if (_write8 (FUSB302_REG_POWER, power) != 0) return -1;
    return 0; 
}


/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static uint8_t _convert_bc_lvl(int bc_lvl)
{

	int ret = FUSB302_TYPEC_CC_VOLT_OPEN;

	if (_state.pullup) 
    {
		if (bc_lvl == 0x00) 
        {
            ret = FUSB302_TYPEC_CC_VOLT_RA;
        }
		else if (bc_lvl < 0x03)
        {
            ret = FUSB302_TYPEC_CC_VOLT_RD;
        }
	} 
    else 
    {
		if (bc_lvl == 0x01)
        {
			ret = FUSB302_TYPEC_CC_VOLT_SNK_DEF;
        }
		else if (bc_lvl == 0x02)
        {
			ret = FUSB302_TYPEC_CC_VOLT_SNK_1_5;
        }
		else if (bc_lvl == 0x03)
        {
			ret = FUSB302_TYPEC_CC_VOLT_SNK_3_0;
        }
	}

	return ret;
}


/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static int32_t _measure_cc_source (uint32_t cc, uint8_t* level)
{
    uint8_t tmp = 0; 
    uint8_t switch0; 
    int8_t cc_lvl;
    if (_read8 (FUSB302_REG_SWITCH_0, &tmp) != 0) return -1;    
    
    switch0 = tmp; 
    // Clear measure bits
    tmp &= ~(FUSB302_BIT_MASK(uint8_t, 1) << 2);    
    tmp &= ~(FUSB302_BIT_MASK(uint8_t, 1) << 3);     

    if (cc == FUSB302_CC_PIN1)
    {
        tmp |= (1 << 6); // PU_EN1 for CC1
        tmp |= (1 << 2); // MEAS CC1
    }
    else 
    {
        tmp |= (1 << 7); // PU_EN2 for CC2
        tmp |= (1 << 3); // MEAS CC2
    }

    if (_write8 (FUSB302_REG_SWITCH_0, tmp) != 0) return -1;            // Set Switchs    
    if (_write8 (FUSB302_REG_MEASURE, _state.mdac_vnc) != 0) return -1; // Set MDAC for Open vs Rd/Ra comparison
    hal_delay_us (250);                                                 // Sleep

    // 
    if (_read8 (FUSB302_REG_STATUS_0, &tmp) != 0) return -1; 
	cc_lvl = FUSB302_TYPEC_CC_VOLT_OPEN;

    if ((tmp & (FUSB302_BIT_MASK(uint8_t, 1) << 5)) == 0)
    {   
        if (_write8 (FUSB302_REG_MEASURE, _state.mdac_rd) != 0) return -1;   
        hal_delay_us (250); 
        if (_read8 (FUSB302_REG_STATUS_0, &tmp) != 0) return -1; 
		cc_lvl = (tmp & (FUSB302_BIT_MASK(uint8_t, 1) << 5)) ? FUSB302_TYPEC_CC_VOLT_RA : FUSB302_TYPEC_CC_VOLT_RD;
    }

    *level =  cc_lvl;

    // Put back switch state
    if (_write8 (FUSB302_REG_SWITCH_0, switch0) != 0) return -1;

    return 0; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static void _detect_cc_source (uint8_t *cc1_lvl, uint8_t *cc2_lvl)
{
	if (_state.vconn_en) 
    {
		/* If VCONN enabled, measure cc_pin that matches polarity */
		if (_state.cc_polarity)
        {
            _measure_cc_source (FUSB302_CC_PIN2, cc2_lvl); 
        }
		else
        {
            _measure_cc_source (FUSB302_CC_PIN1, cc1_lvl); 
        }	
	} 
    else 
    {
		/* If VCONN not enabled, measure both cc1 and cc2 */
        _measure_cc_source (FUSB302_CC_PIN1, cc1_lvl); 
		_measure_cc_source (FUSB302_CC_PIN2, cc2_lvl); 
	}
}


/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
static int32_t _detect_cc_sink (uint8_t *cc1_lvl, uint8_t *cc2_lvl)
{

	uint8_t tmp = 0 ;
	uint8_t orig_meas_cc1;
	uint8_t orig_meas_cc2;
	uint8_t bc_lvl_cc1;
	uint8_t bc_lvl_cc2;

    if (_read8 (FUSB302_REG_SWITCH_0, &tmp) != 0) return -1;    

    /* save original state to be returned to later... */
	if (tmp & (FUSB302_BIT_MASK(uint8_t, 1) << 2))
    {
        orig_meas_cc1 = 1;
    }
	else
    {
        orig_meas_cc1 = 0;
    }

	if (tmp & (FUSB302_BIT_MASK(uint8_t, 1) << 3))
    {
        orig_meas_cc2 = 1;
    }
	else
    {
        orig_meas_cc2 = 0;
    }

    // Measure CC1 - Disable CC2 measurement switch, enable CC1 measurement switch
    if (_read8 (FUSB302_REG_SWITCH_0, &tmp) != 0) return -1;    
    tmp &= ~(FUSB302_BIT_MASK(uint8_t, 1) << 3);    
    tmp |= (1 << 2); 
    if (_write8 (FUSB302_REG_SWITCH_0, tmp) != 0) return -1;
    hal_delay_us(250);
    if (_read8 (FUSB302_REG_STATUS_0, &bc_lvl_cc1) != 0) return -1;    
    bc_lvl_cc1 &= (FUSB302_BIT_MASK(uint8_t, 2) << 0);

    // Measure CC2 - Disable CC2 measurement switch, enable CC1 measurement switch
    if (_read8 (FUSB302_REG_SWITCH_0, &tmp) != 0) return -1;    
    tmp &= ~(FUSB302_BIT_MASK(uint8_t, 1) << 2);    
    tmp |= (1 << 3); 
    if (_write8 (FUSB302_REG_SWITCH_0, tmp) != 0) return -1;
    hal_delay_us(250);
    if (_read8 (FUSB302_REG_STATUS_0, &bc_lvl_cc2) != 0) return -1;    
    bc_lvl_cc2 &= (FUSB302_BIT_MASK(uint8_t, 2) << 0);

    // Convert Measurements
	*cc1_lvl = _convert_bc_lvl(bc_lvl_cc1);
	*cc2_lvl = _convert_bc_lvl(bc_lvl_cc2);

    if (_read8 (FUSB302_REG_SWITCH_0, &tmp) != 0) return -1;    
    if (orig_meas_cc1)
    {
        tmp |= (FUSB302_BIT_MASK(uint8_t, 1) << 2);
    }
	else
    {
        tmp &= ~(FUSB302_BIT_MASK(uint8_t, 1) << 2);
    }
		
	if (orig_meas_cc2)
    {
		tmp |= (FUSB302_BIT_MASK(uint8_t, 1) << 3);
    }
	else
    {
        tmp &= ~(FUSB302_BIT_MASK(uint8_t, 1) << 3);
    }
    
    if (_write8 (FUSB302_REG_SWITCH_0, tmp) != 0) return -1;
    return 0;
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t fusb302_tcpm_init (uint8_t i2c, uint8_t addr)
{
    uint8_t id = 0; 
    _i2c = i2c; 
    _addr = addr; 

    // Not working
    if (_get_devid (&id)!=0) return -1; 

    // Set up state
    memset(&_state, 0, sizeof(fusb302_state_t)); 
    _state.cc_polarity = -1; 
	_state.mdac_vnc = FUSB302_REG_MEASURE_MDAC_MV(1600);    /* set the voltage threshold for no connect detection (vOpen) */
	_state.mdac_rd = FUSB302_REG_MEASURE_MDAC_MV(200);      /* set the voltage threshold for Rd vs Ra detection */

        
    // SW Reset
    _sw_reset(); 
    (void) _pd_reset; 

    _enable_cc_pdwn (true, true); 
    _enable_auto_retry (true, 3);

	/* Create interrupt masks */
	uint8_t reg = 0xFF;
	reg &= ~FUSB302_REG_MASK_BC_LVL;
	reg &= ~FUSB302_REG_MASK_COLLISION;
	reg &= ~FUSB302_REG_MASK_ALERT;
	reg &= ~FUSB302_REG_MASK_CRC_CHK;
	_write8(FUSB302_REG_MASK, reg);

    reg = 0xFF;
	reg &= ~FUSB302_REG_MASKA_RETRYFAIL;
	reg &= ~FUSB302_REG_MASKA_HARDSENT;
	reg &= ~FUSB302_REG_MASKA_TX_SUCCESS;
	reg &= ~FUSB302_REG_MASKA_HARDRESET;
	_write8(FUSB302_REG_MASKA, reg);

	reg = 0xFF;
	reg &= ~FUSB302_REG_MASKB_GCRCSENT;
	_write8(FUSB302_REG_MASKB, reg);

	_read8(FUSB302_REG_CTRL_0, &reg);
	reg &= ~FUSB302_REG_CTRL_0_INT_MASK;
	_write8 (FUSB302_REG_CTRL_0, reg);

    fusb302_tcpm_set_polarity (0); 
    fusb302_tcpm_set_vconn (0);            
    _set_power(0x0F);       // All on
    return 0; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t fusb302_tcpm_uninit (void)
{
    return 0; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t fusb302_tcpm_get_cc (uint8_t *cc1, uint8_t *cc2)
{
	if (_state.pullup) 
    {
		/* Source mode? */
		_detect_cc_source (cc1, cc2);
	} 
    else {
		/* Sink mode? */
		_detect_cc_sink(cc1, cc2);
	}

	return 0;
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t fusb302_tcpm_select_rp_value(uint32_t rp)
{
	uint8_t tmp = 0;
	uint8_t vnc, rd;
	
    if (_read8 (FUSB302_REG_CTRL_0, &tmp) != 0) return -1;    

	
    tmp &= ~(FUSB302_BIT_MASK(uint8_t, 3) << 2);  // Clear

	switch (rp) 
    {
        case TYPEC_RP_1A5:
        {
            tmp |= (2<<2);
            vnc = FUSB302_REG_MEASURE_MDAC_MV(PD_SRC_1_5_VNC_MV);
            rd = FUSB302_REG_MEASURE_MDAC_MV(PD_SRC_1_5_RD_THRESH_MV);
        }
        break;
        case TYPEC_RP_3A0:
        {
            tmp |= (3<<2);
            vnc = FUSB302_REG_MEASURE_MDAC_MV(PD_SRC_3_0_VNC_MV);
            rd = FUSB302_REG_MEASURE_MDAC_MV(PD_SRC_3_0_RD_THRESH_MV);
        }
        break;
        case TYPEC_RP_USB:
        default:
            tmp |= (1<<2);
            vnc = FUSB302_REG_MEASURE_MDAC_MV(PD_SRC_DEF_VNC_MV);
            rd = FUSB302_REG_MEASURE_MDAC_MV(PD_SRC_DEF_RD_THRESH_MV);
	}
	_state.mdac_vnc = vnc;
	_state.mdac_rd = rd;

    if (_write8 (FUSB302_REG_CTRL_0, tmp) != 0) return -1;
    return 0;
}



/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t fusb302_tcpm_get_vbus_level(void)
{
	uint8_t tmp = 0;
    _read8 (FUSB302_REG_STATUS_0, &tmp); 
	return (tmp & (FUSB302_BIT_MASK(uint8_t, 2) << 0)) ? 1 : 0;
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
uint32_t fusb302_tcpm_set_cc(uint32_t pull)
{
	uint8_t tmp = 0;
	/* NOTE: FUSB302 toggles a single pull-up between CC1 and CC2 */
	/* NOTE: FUSB302 Does not support Ra. */
	switch (pull) 
    {
        case TYPEC_CC_RP:
        {
            /* enable the pull-up we know to be necessary */
            _read8 (FUSB302_REG_SWITCH_0, &tmp); 

            tmp &= ~(   FUSB302_REG_SWITCH_0_CC1_PU_EN |
                        FUSB302_REG_SWITCH_0_CC2_PU_EN |
                        FUSB302_REG_SWITCH_0_CC1_PD_EN |
                        FUSB302_REG_SWITCH_0_CC2_PD_EN |
                        FUSB302_REG_SWITCH_0_VCONN_CC1 |
                        FUSB302_REG_SWITCH_0_VCONN_CC2);

            tmp |=      FUSB302_REG_SWITCH_0_CC1_PU_EN |
			            FUSB302_REG_SWITCH_0_CC2_PU_EN;


            if (_state.vconn_en)
            {
                tmp |= _state.cc_polarity ? FUSB302_REG_SWITCH_0_VCONN_CC1 : FUSB302_REG_SWITCH_0_VCONN_CC2;
            }
            
            _write8 (FUSB302_REG_SWITCH_0, tmp); 
            _state.pullup = 1;
        }
        break;
        case TYPEC_CC_RD:
        {
            // Turn off toggle
            _read8 (FUSB302_REG_CTRL_2, &tmp); 
            tmp &= ~FUSB302_REG_CTRL_2_TOGGLE;
            _write8 (FUSB302_REG_CTRL_2, tmp); 

            // Disable pull ups
            _read8  (FUSB302_REG_SWITCH_0, &tmp); 
            tmp &= ~(FUSB302_REG_SWITCH_0_CC1_PU_EN);
            tmp &= ~(FUSB302_REG_SWITCH_0_CC2_PU_EN);
            tmp |=  (FUSB302_REG_SWITCH_0_CC1_PD_EN);
            tmp |=  (FUSB302_REG_SWITCH_0_CC2_PD_EN);
            _write8 (FUSB302_REG_SWITCH_0, tmp); 
            _state.pullup = 0;
        }
        break;
        case TYPEC_CC_OPEN:
        {
            // Turn off toggle
            _read8 (FUSB302_REG_CTRL_2, &tmp); 
            tmp &= ~FUSB302_REG_CTRL_2_TOGGLE;
            _write8 (FUSB302_REG_CTRL_2, tmp); 

            // Pullups Off
            _read8  (FUSB302_REG_SWITCH_0, &tmp); 
            tmp &= ~(FUSB302_REG_SWITCH_0_CC1_PU_EN);
            tmp &= ~(FUSB302_REG_SWITCH_0_CC2_PU_EN);
            tmp &= ~(FUSB302_REG_SWITCH_0_CC1_PD_EN);
            tmp &= ~(FUSB302_REG_SWITCH_0_CC2_PD_EN);   
            _write8 (FUSB302_REG_SWITCH_0, tmp); 
            _state.pullup = 0;          
        }
        break; 
        default:
            return -1;
	}
	return 0;
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t fusb302_tcpm_set_polarity(int32_t polarity)
{
	/* Port polarity : 0 => CC1 is CC line, 1 => CC2 is CC line */
	uint8_t tmp = 0;
	_read8(FUSB302_REG_SWITCH_0, &tmp);

	/* clear VCONN switch bits */
	tmp &= ~FUSB302_REG_SWITCH_0_VCONN_CC2;
	tmp &= ~FUSB302_REG_SWITCH_0_VCONN_CC1;

	if (_state.vconn_en) 
    {
		/* set VCONN switch to be non-CC line */
		if (polarity)
        {
            tmp |= FUSB302_REG_SWITCH_0_VCONN_CC1;
        }
        else 
        {
            tmp |= FUSB302_REG_SWITCH_0_VCONN_CC2;
        }	
	}

	/* clear meas_cc bits (RX line select) */
	tmp &= ~FUSB302_REG_SWITCH_0_MEAS_CC1;
	tmp &= ~FUSB302_REG_SWITCH_0_MEAS_CC2;

	/* set rx polarity */
	if (polarity)
    {
		tmp |= FUSB302_REG_SWITCH_0_MEAS_CC2;
    }
    else 
    {
        tmp |= FUSB302_REG_SWITCH_0_MEAS_CC1;
    }
	_write8(FUSB302_REG_SWITCH_0, tmp);


	_read8(FUSB302_REG_SWITCH_1, &tmp);
	tmp &= ~FUSB302_REG_SWITCH_1_TXCC1_EN;
	tmp &= ~FUSB302_REG_SWITCH_1_TXCC2_EN;

	/* set tx polarity */
    if (polarity)
    {
        tmp |= FUSB302_REG_SWITCH_1_TXCC2_EN;
    }
    else 
    {
        tmp |= FUSB302_REG_SWITCH_1_TXCC1_EN;
    }	
	_write8(FUSB302_REG_SWITCH_1, tmp);

	/* Save the polarity for later */
	_state.cc_polarity = polarity;
	return 0;
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t fusb302_tcpm_set_vconn(uint32_t enable)
{
	uint8_t tmp;
	_state.vconn_en = enable;
	if (enable) 
    {
		fusb302_tcpm_set_polarity(_state.cc_polarity);
	} 
    else 
    {
		_read8(FUSB302_REG_SWITCH_0, &tmp);
		tmp &= ~FUSB302_REG_SWITCH_0_VCONN_CC1;
		tmp &= ~FUSB302_REG_SWITCH_0_VCONN_CC2;
		_write8(FUSB302_REG_SWITCH_0, tmp);
	}
	return 0;
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t fusb302_tcpm_set_msg_header(uint32_t power_role, uint32_t data_role)
{
	uint8_t tmp;

	_read8(FUSB302_REG_SWITCH_1, &tmp);

	tmp &= ~FUSB302_REG_SWITCH_1_POWERROLE;
	tmp &= ~FUSB302_REG_SWITCH_1_DATAROLE;

	if (power_role)
    {
	    tmp |= FUSB302_REG_SWITCH_1_POWERROLE;
    }

	if (data_role)
    {
        tmp |= FUSB302_REG_SWITCH_1_DATAROLE;
    }
		
	_write8 (FUSB302_REG_SWITCH_1, tmp);
	return 0;
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t fusb302_tcpm_set_rx_enable(uint32_t enable)
{
	uint8_t tmp;
	_state.rx_en = enable;
	_read8(FUSB302_REG_SWITCH_0, &tmp);

	/* Clear CC1/CC2 measure bits */
	tmp &= ~FUSB302_REG_SWITCH_0_MEAS_CC1;
	tmp &= ~FUSB302_REG_SWITCH_0_MEAS_CC2;

	if (enable) 
    {
		switch (_state.cc_polarity) 
        {
            case -1:
            {
                return -1;
            }
            break;
		    case 0:
            {
			    tmp |= FUSB302_REG_SWITCH_0_MEAS_CC1;
            }
			break;
            case 1:
            {
                tmp |= FUSB302_REG_SWITCH_0_MEAS_CC2;
            }
            break;
		default:
			return -1;
		}
		_write8( FUSB302_REG_SWITCH_0, tmp);

		/* Disable BC_LVL interrupt when enabling PD comm */
		if (_read8(FUSB302_REG_MASK, &tmp) == 0)
        {
            _write8(FUSB302_REG_MASK, tmp | FUSB302_REG_MASK_BC_LVL);
        }

		_flush_rx_fifo();


	} 
    else 
    {
		_write8(FUSB302_REG_SWITCH_0, tmp);

		/* Enable BC_LVL interrupt when disabling PD comm */
		if (_read8(FUSB302_REG_MASK, &tmp) == 0)
        {
            _write8(FUSB302_REG_MASK, tmp & ~FUSB302_REG_MASK_BC_LVL);
        }		
	}

	_auto_goodcrc_enable(enable);

	return 0;
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t fusb302_tcpm_get_message(uint8_t *payload, int *head)
{
	/*
	 * This is the buffer that will get the burst-read data
	 * from the fusb302.
	 *
	 * It's re-used in a couple different spots, the worst of which
	 * is the PD packet (not header) and CRC.
	 * maximum size necessary = 28 + 4 = 32
	 */
	uint8_t buf[32];
	int32_t rv, len;

	/* If our FIFO is empty then we have no packet */
	if (_rx_fifo_is_empty()) return -1;

	/* Read until we have a non-GoodCRC packet or an empty FIFO */
	do 
    {
		buf[0] = FUSB302_REG_FIFO;
        /*

		 * PART 1 OF BURST READ: Write in register address.
		 * Issue a START, no STOP.
		 */
        _write_array (buf, 1, true); 
		/*
		 * PART 2 OF BURST READ: Read up to the header.
		 * Issue a repeated START, no STOP.
		 * only grab three bytes so we can get the header
		 * and determine how many more bytes we need to read.
		 * TODO: Check token to ensure valid packet.
		 */
        _read_array (buf, 3); 


		/* Grab the header */
		*head = (buf[1] & 0xFF);
		*head |= ((buf[2] << 8) & 0xFF00);

		/* figure out packet length, subtract header bytes */
		len = _get_num_bytes(*head) - 2;

		/*
		 * PART 3 OF BURST READ: Read everything else.
		 * No START, but do issue a STOP at the end.
		 * add 4 to len to read CRC out
		 */
        rv = _read_array (buf, len+4); 

	} while (!rv && FUSB302_PACKET_IS_GOOD_CRC(*head) && _rx_fifo_is_empty());

	if (!rv) 
    {
		/* Discard GoodCRC packets */
		if (FUSB302_PACKET_IS_GOOD_CRC(*head))
        {
            rv = -1;
        }
		else
        {
			memcpy(payload, buf, len);
        }
	}

	/*
	 * If our FIFO is non-empty then we may have a packet, we may get
	 * fewer interrupts than packets due to interrupt latency.
	 */
	//if (!fusb302_rx_fifo_is_empty(port))
	//	task_set_event(PD_PORT_TO_TASK_ID(port), PD_EVENT_RX, 0);

	return rv;
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
int32_t fusb302_tcpm_transmit(enum tcpm_transmit_type type, uint16_t header, const uint32_t *data)
{
	/*
	 * this is the buffer that will be burst-written into the fusb302
	 * maximum size necessary =
	 * 1: FIFO register address
	 * 4: SOP* tokens
	 * 1: Token that signifies "next X bytes are not tokens"
	 * 30: 2 for header and up to 7*4 = 28 for rest of message
	 * 1: "Insert CRC" Token
	 * 1: EOP Token
	 * 1: "Turn transmitter off" token
	 * 1: "Start Transmission" Command
	 * -
	 * 40: 40 bytes worst-case
	 */
	uint8_t buf[40];
	uint32_t buf_pos = 0;

	uint8_t reg;

	/* Flush the TXFIFO */
	_flush_tx_fifo();

	switch (type) 
    {
        case TCPC_TX_SOP:
        {
            /* put register address first for of burst tcpc write */
            buf[buf_pos++] = FUSB302_REG_FIFO;

            /* Write the SOP Ordered Set into TX FIFO */
            buf[buf_pos++] = fusb302_TKN_SYNC1;
            buf[buf_pos++] = fusb302_TKN_SYNC1;
            buf[buf_pos++] = fusb302_TKN_SYNC1;
            buf[buf_pos++] = fusb302_TKN_SYNC2;

            _send_message(header, data, buf, buf_pos);
            hal_delay_us(600);
        }
		return 0;
        case TCPC_TX_HARD_RESET:
        {
            /* Simply hit the SEND_HARD_RESET bit */
            _read8(FUSB302_REG_CTRL_3,&reg); 
            reg |= FUSB302_REG_CTRL_3_SEND_HARDRESET;
            _write8(FUSB302_REG_CTRL_3,reg); 
        }
        break; 
	    case TCPC_TX_BIST_MODE_2:
        {
            _read8(FUSB302_REG_CTRL_1,&reg); 
            reg |= FUSB302_REG_CTRL_1_BIST_MODE2;
            _write8(FUSB302_REG_CTRL_1,reg); 

            _read8(FUSB302_REG_CTRL_0,&reg); 
            reg |= FUSB302_REG_CTRL_0_TX_FLUSH;
            _write8(FUSB302_REG_CTRL_0,reg); 

            _read8(FUSB302_REG_CTRL_1,&reg); 
            reg &= ~FUSB302_REG_CTRL_1_BIST_MODE2;
            _write8(FUSB302_REG_CTRL_1,reg); 
        }
		break;
        default:
        {
            return -1;
        }
	}
	return 0;
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void fusb302_tcpc_alert(int port)
{
	/* interrupt has been received */
	uint8_t interrupt;
	uint8_t interrupta;
	uint8_t interruptb;

	_read8(FUSB302_REG_INT, &interrupt);
	_read8(FUSB302_REG_INT_A, &interrupta);
	_read8(FUSB302_REG_INT_B, &interruptb);
	/*
		* Ignore BC_LVL changes when transmitting / receiving PD,
		* since CC level will constantly change.
		*/
	if (_state.rx_en)
    {
        interrupt &= ~FUSB302_REG_INT_BC_LVL;
    }
		

	if (interrupt & FUSB302_REG_INT_BC_LVL) 
    {
		/* CC Status change */
		//task_set_event(PD_PORT_TO_TASK_ID(port), PD_EVENT_CC, 0);
	}

	if (interrupt & FUSB302_REG_INT_COLLISION) 
    {
		/* packet sending collided */
		//pd_transmit_complete(port, TCPC_TX_COMPLETE_FAILED);
	}

	if (interrupta & FUSB302_REG_INT_A_TX_SUCCESS) 
    {
		//pd_transmit_complete(port, TCPC_TX_COMPLETE_SUCCESS);
	}

	if (interrupta & FUSB302_REG_INT_A_RETRYFAIL) 
    {
		//pd_transmit_complete(port, TCPC_TX_COMPLETE_FAILED);
	}

	if (interrupta & FUSB302_REG_INT_A_HARDSENT) 
    {
		_pd_reset();
		//pd_transmit_complete(port, TCPC_TX_COMPLETE_SUCCESS);
	}

	if (interrupta & FUSB302_REG_INT_A_HARDRESET) 
    {
		_pd_reset();
		//pd_execute_hard_reset(port);
	}

	if (interruptb & FUSB302_REG_INT_B_GCRCSENT)
    {
		if (_state.rx_en) 
        {


		} 
        else 
        {
			_flush_rx_fifo();
		}
	}
}
