#ifndef __SSSD1309_H
#define __SSSD1309_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define SSD1309_WIDTH 					128
#define SSD1309_HEIGHT 					64
#define SSD1309_PAGE_HEIGHT 		    8
#define SSD1309_SET_COL_HI			    0x10
#define SSD1309_SET_COL_LO			    0x00
#define SSD1309_SET_LINE				0x40
#define SSD1309_SET_CONTRAST		    0x81
#define SSD1309_SET_BRIGHTNESS          0x82
#define SSD1309_SET_LOOKUP              0x91
#define SSD1309_SET_SEG_REMAP0          0xA0
#define SSD1309_SET_SEG_REMAP1	        0xA1
#define SSD1309_EON_OFF					0xA4
#define SSD1309_EON_ON					0xA5
#define SSD1309_DISP_NOR				0xA6
#define SSD1309_DISP_REV				0xA7
#define SSD1309_MULTIPLEX               0xA8
#define SSD1309_MASTER_CONFIG           0xAD
#define SSD1309_CHARGE_PUMP    	        0x8D
#define SSD1309_PUMP_OFF                0x10
#define SSD1309_PUMP_ON     		    0x14
#define SSD1309_DISP_OFF 				0xAE
#define SSD1309_DISP_ON					0xAF
#define SSD1309_SET_PAGE				0xB0
#define SSD1309_SET_SCAN_FLIP           0xC0
#define SSD1309_SET_SCAN_NOR		    0xC8
#define SSD1309_SET_OFFSET			    0xD3
#define SSD1309_SET_RATIO_OSC		    0xD5
#define SSD1309_SET_AREA_COLOR          0xD8
#define SSD1309_SET_CHARGE  		    0xD9
#define SSD1309_SET_PADS    		    0xDA
#define SSD1309_SET_VCOM    		    0xDB
#define SSD1309_NOP     				0xE3
#define SSD1309_SCROLL_RIGHT		    0x26
#define SSD1309_SCROLL_LEFT			    0x27
#define SSD1309_SCROLL_VR	    	    0x29
#define SSD1309_SCROLL_VL				0x2A
#define SSD1309_SCROLL_OFF			    0x2E
#define SSD1309_SCROLL_ON   		    0x2F
#define SSD1309_SCROLL_ON   		    0x2F
#define SSD1309_VERT_SCROLL_A  	        0xA3
#define SSD1309_MEM_ADDRESSING 	        0x20
#define SSD1309_SET_COL_ADDR		    0x21
#define SSD1309_SET_PAGE_ADDR		    0x22

#define  WHITE          0
#define  BLACK          1    
#define  INVERSE        2


void    ssd1309_init (uint32_t spi, uint8_t dc, uint8_t cs, uint8_t rst); 
void 	ssd1309_set_line (uint8_t line); 
void 	ssd1309_set_page (uint8_t page); 
void 	ssd1309_set_start_column (uint8_t d); 
void 	ssd1309_clear_display (void); 
void 	ssd1309_update_frame (uint8_t* buffer, uint32_t size); 
void    ssd1309_write_data (uint8_t data); 
void    ssd1309_write_command (uint8_t command); 
void    ssd1309_reset (void); 
#ifdef __cplusplus
}
#endif
#endif





