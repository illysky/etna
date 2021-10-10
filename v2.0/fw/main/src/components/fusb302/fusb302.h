/************************************************************************************************************************************* 
 * @file  	
 * @brief 	
 *************************************************************************************************************************************/
#ifndef FUSB302B_H
#define FUSB302B_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include "usb_pd_tcpm.h"
#include "usb_pd.h"

#include <limits.h>     /* CHAR_BIT */
#define FUSB302_BIT_MASK(__TYPE__, __ONE_COUNT__)((__TYPE__) (-((__ONE_COUNT__) != 0)))& (((__TYPE__) -1) >> ((sizeof(__TYPE__) * CHAR_BIT) - (__ONE_COUNT__)))

//#define FUSB302_REG_CTRL_3_POSI_	0	
//#define FUSB302_REG_CTRL_3_MASK_	FUSB302_BIT_MASK(uint8_t, 1)


#define FUSB302_REG_DEVICE_ID	0x01



#define FUSB302_REG_SWITCH_0 			0x02
#define FUSB302_REG_SWITCH_0_CC2_PU_EN	(1<<7)
#define FUSB302_REG_SWITCH_0_CC1_PU_EN	(1<<6)
#define FUSB302_REG_SWITCH_0_VCONN_CC2	(1<<5)
#define FUSB302_REG_SWITCH_0_VCONN_CC1	(1<<4)
#define FUSB302_REG_SWITCH_0_MEAS_CC2	(1<<3)
#define FUSB302_REG_SWITCH_0_MEAS_CC1	(1<<2)
#define FUSB302_REG_SWITCH_0_CC2_PD_EN	(1<<1)
#define FUSB302_REG_SWITCH_0_CC1_PD_EN	(1<<0)


#define FUSB302_REG_SWITCH_1   	 			0x03
#define FUSB302_REG_SWITCH_1_POWERROLE    (1<<7)
#define FUSB302_REG_SWITCH_1_SPECREV1     (1<<6)
#define FUSB302_REG_SWITCH_1_SPECREV0     (1<<5)
#define FUSB302_REG_SWITCH_1_DATAROLE     (1<<4)
#define FUSB302_REG_SWITCH_1_AUTO_GCRC    (1<<2)
#define FUSB302_REG_SWITCH_1_TXCC2_EN     (1<<1)
#define FUSB302_REG_SWITCH_1_TXCC1_EN     (1<<0)



#define FUSB302_REG_MEASURE     	0x04
#define FUSB302_REG_SLICE       	0x05

#define FUSB302_REG_CTRL_0					0x06
#define FUSB302_REG_CTRL_0_TX_FLUSH  		(1<<6)
#define FUSB302_REG_CTRL_0_INT_MASK  		(1<<5)
#define FUSB302_REG_CTRL_0_HOST_CUR_MASK 	(3<<2)
#define FUSB302_REG_CTRL_0_HOST_CUR_3A0		(3<<2)
#define FUSB302_REG_CTRL_0_HOST_CUR_1A5		(2<<2)
#define FUSB302_REG_CTRL_0_HOST_CUR_USB		(1<<2)
#define FUSB302_REG_CTRL_0_TX_START  		(1<<0)


#define FUSB302_REG_CTRL_1      		0x07
#define FUSB302_REG_CTRL_1_ENSOP2DB  	(1<<6)
#define FUSB302_REG_CTRL_1_ENSOP1DB  	(1<<5)
#define FUSB302_REG_CTRL_1_BIST_MODE2	(1<<4)
#define FUSB302_REG_CTRL_1_RX_FLUSH  	(1<<2)
#define FUSB302_REG_CTRL_1_ENSOP2		(1<<1)
#define FUSB302_REG_CTRL_1_ENSOP1    	(1<<0)

#define FUSB302_REG_CTRL_2      	0x08
#define FUSB302_REG_CTRL_2_MODE		(1<<1)
#define FUSB302_REG_CTRL_2_MODE_DFP	(0x3)
#define FUSB302_REG_CTRL_2_MODE_UFP	(0x2)
#define FUSB302_REG_CTRL_2_MODE_DRP	(0x1)
#define FUSB302_REG_CTRL_2_MODE_POS	(1)
#define FUSB302_REG_CTRL_2_TOGGLE	(1<<0)



#define FUSB302_REG_CTRL_3					0x09
#define FUSB302_REG_CTRL_3_SEND_HARDRESET    (1<<6)
#define FUSB302_REG_CTRL_3_BIST_TMODE        (1<<5) /* 302B Only */
#define FUSB302_REG_CTRL_3_AUTO_HARDRESET    (1<<4)
#define FUSB302_REG_CTRL_3_AUTO_SOFTRESET    (1<<3)
/* two-bit field */
#define FUSB302_REG_CTRL_3_N_RETRIES     	(1<<1)
#define FUSB302_REG_CTRL_3_N_RETRIES_POS     (1)
#define FUSB302_REG_CTRL_3_N_RETRIES_SIZE    (2)
#define FUSB302_REG_CTRL_3_AUTO_RETRY        (1<<0)


#define FUSB302_REG_MASK    			0x0A
#define FUSB302_REG_MASK_VBUSOK        	(1<<7)
#define FUSB302_REG_MASK_ACTIVITY      	(1<<6)
#define FUSB302_REG_MASK_COMP_CHNG     	(1<<5)
#define FUSB302_REG_MASK_CRC_CHK       	(1<<4)
#define FUSB302_REG_MASK_ALERT     		(1<<3)
#define FUSB302_REG_MASK_WAKE      		(1<<2)
#define FUSB302_REG_MASK_COLLISION     	(1<<1)
#define FUSB302_REG_MASK_BC_LVL        	(1<<0)

#define FUSB302_REG_POWER       0x0B
#define FUSB302_REG_RESET       0x0C
#define FUSB302_REG_OC_PREG     0x0D

#define FUSB302_REG_MASKA      0x0E
#define FUSB302_REG_MASKA_OCP_TEMP     (1<<7)
#define FUSB302_REG_MASKA_TOGDONE      (1<<6)
#define FUSB302_REG_MASKA_SOFTFAIL     (1<<5)
#define FUSB302_REG_MASKA_RETRYFAIL    (1<<4)
#define FUSB302_REG_MASKA_HARDSENT     (1<<3)
#define FUSB302_REG_MASKA_TX_SUCCESS   (1<<2)
#define FUSB302_REG_MASKA_SOFTRESET    (1<<1)
#define FUSB302_REG_MASKA_HARDRESET    (1<<0)

#define FUSB302_REG_MASKB      0x0F
#define FUSB302_REG_MASKB_GCRCSENT     (1<<0)

#define FUSB302_REG_CTRL_4      0x10
#define FUSB302_REG_STATUS_0A   0x3C
#define FUSB302_REG_STATUS_1A   0x3D

#define FUSB302_REG_INT_A       		0x3E
#define FUSB302_REG_INT_A_OCP_TEMP    	(1<<7)
#define FUSB302_REG_INT_A_TOGDONE 		(1<<6)
#define FUSB302_REG_INT_A_SOFTFAIL    	(1<<5)
#define FUSB302_REG_INT_A_RETRYFAIL   	(1<<4)
#define FUSB302_REG_INT_A_HARDSENT    	(1<<3)
#define FUSB302_REG_INT_A_TX_SUCCESS  	(1<<2)
#define FUSB302_REG_INT_A_SOFTRESET   	(1<<1)
#define FUSB302_REG_INT_A_HARDRESET   	(1<<0)


#define FUSB302_REG_INT_B       		0x3F
#define FUSB302_REG_INT_B_GCRCSENT		(1<<0)

#define FUSB302_REG_STATUS_0    0x40

#define FUSB302_REG_STATUS_1    0x41
#define FUSB302_REG_STATUS_1_RXSOP2     (1<<7)
#define FUSB302_REG_STATUS_1_RXSOP1     (1<<6)
#define FUSB302_REG_STATUS_1_RX_EMPTY   (1<<5)
#define FUSB302_REG_STATUS_1_RX_FULL    (1<<4)
#define FUSB302_REG_STATUS_1_TX_EMPTY   (1<<3)
#define FUSB302_REG_STATUS_1_TX_FULL    (1<<2)


#define FUSB302_REG_INT         0x42
#define FUSB302_REG_INT_VBUSOK   (1<<7)
#define FUSB302_REG_INT_ACTIVITY (1<<6)
#define FUSB302_REG_INT_COMP_CHNG    (1<<5)
#define FUSB302_REG_INT_CRC_CHK  (1<<4)
#define FUSB302_REG_INT_ALERT    (1<<3)
#define FUSB302_REG_INT_WAKE     (1<<2)
#define FUSB302_REG_INT_COLLISION    (1<<1)
#define FUSB302_REG_INT_BC_LVL   (1<<0)

#define FUSB302_REG_FIFO        0x43


#define FUSB302_REG_MEASURE_MDAC_MV(mv)    (((mv)/42) & 0x3f)

/* No connect voltage threshold for sources based on Rp */
#define FUSB302_PD_SRC_DEF_VNC_MV        1600
#define FUSB302_PD_SRC_1_5_VNC_MV        1600
#define FUSB302_PD_SRC_3_0_VNC_MV        2600

/* Rd voltage threshold for sources based on Rp */
#define FUSB302_PD_SRC_DEF_RD_THRESH_MV  200
#define FUSB302_PD_SRC_1_5_RD_THRESH_MV  400
#define FUSB302_PD_SRC_3_0_RD_THRESH_MV  800




#define FUSB302_CC_PIN1 	0
#define FUSB302_CC_PIN2 	1
#define FUSB302_TYPEC_CC_VOLT_OPEN 0
#define FUSB302_TYPEC_CC_VOLT_RA  1
#define FUSB302_TYPEC_CC_VOLT_RD  2
#define FUSB302_TYPEC_CC_VOLT_SNK_DEF  5
#define FUSB302_TYPEC_CC_VOLT_SNK_1_5  6
#define FUSB302_TYPEC_CC_VOLT_SNK_3_0  7


int32_t 	fusb302_tcpm_init (uint8_t i2c, uint8_t addr); 
int32_t 	fusb302_tcpm_uninit (void);
int32_t 	fusb302_tcpm_get_cc (uint8_t *cc1, uint8_t *cc2);
int32_t 	fusb302_tcpm_select_rp_value(uint32_t rp);
int32_t 	fusb302_tcpm_get_vbus_level(void);
uint32_t 	fusb302_tcpm_set_cc(uint32_t pull);
int32_t 	fusb302_tcpm_set_polarity(int32_t polarity); 
int32_t 	fusb302_tcpm_set_vconn(uint32_t enable); 
int32_t 	fusb302_tcpm_set_msg_header(uint32_t power_role, uint32_t data_role); 



#define FUSB302_BIT_MASK(__TYPE__, __ONE_COUNT__)((__TYPE__) (-((__ONE_COUNT__) != 0)))& (((__TYPE__) -1) >> ((sizeof(__TYPE__) * CHAR_BIT) - (__ONE_COUNT__)))

/* Used for processing pd header */
#define FUSB302_PD_HEADER_EXT(header)  (((header) >> 15) & 1)
#define FUSB302_PD_HEADER_CNT(header)  (((header) >> 12) & 7)
#define FUSB302_PD_HEADER_TYPE(header) ((header) & 0xF)
#define FUSB302_PD_HEADER_ID(header)   (((header) >> 9) & 7)
#define FUSB302_PD_HEADER_REV(header)  (((header) >> 6) & 3)


typedef struct
{
	int32_t     cc_polarity;
	bool        vconn_en;
	int         pullup;              
	int         rx_en;
	uint8_t     mdac_vnc;
	uint8_t     mdac_rd;
} fusb302_state_t; 



/* Tokens defined for the FUSB302 TX FIFO */
enum fusb302_txfifo_tokens {
    fusb302_TKN_TXON = 0xA1,
    fusb302_TKN_SYNC1 = 0x12,
    fusb302_TKN_SYNC2 = 0x13,
    fusb302_TKN_SYNC3 = 0x1B,
    fusb302_TKN_RST1 = 0x15,
    fusb302_TKN_RST2 = 0x16,
    fusb302_TKN_PACKSYM = 0x80,
    fusb302_TKN_JAMCRC = 0xFF,
    fusb302_TKN_EOP = 0x14,
    fusb302_TKN_TXOFF = 0xFE,
};



#ifdef __cplusplus
}
#endif

#endif 