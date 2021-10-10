/*****************************************************************************************************************************************************************************************
 * @file						
 * @brief: 	
 * @version:			
 * @author: 		 Created by Dominic Moffat on 11/12/2017. Copyright © 2017 Illysky. All rights reserved.										
*****************************************************************************************************************************************************************************************/

#include "ssd1309.h"
#include "hal.h"

static uint8_t ssd1309_pin_dc = 0xFF; 
static uint8_t ssd1309_pin_cs = 0xFF; 
static uint8_t ssd1309_pin_rst = 0xFF; 
static uint8_t ssd1309_spi = 0xFF; 
/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void ssd1309_init (uint32_t spi, uint8_t dc, uint8_t cs, uint8_t rst) 
{

	// Config HAL
	ssd1309_pin_dc = dc; 
	ssd1309_pin_cs = cs; 
	ssd1309_pin_rst = rst; 
	ssd1309_spi = spi;  

	ssd1309_reset (); 
	ssd1309_write_command (0xFD); 
	ssd1309_write_data (0x12); 
	ssd1309_write_command(SSD1309_DISP_OFF);
	ssd1309_write_command(SSD1309_SET_RATIO_OSC); 
	ssd1309_write_command(0xA0); 
	ssd1309_write_command(SSD1309_MULTIPLEX);
	ssd1309_write_command(0x3F);
	ssd1309_write_command(SSD1309_SET_OFFSET);
	ssd1309_write_command(0x00);
	ssd1309_write_command(SSD1309_SET_LINE | 0x00);
	ssd1309_write_command(SSD1309_MEM_ADDRESSING);
	ssd1309_write_command(0x02);
	ssd1309_write_command(SSD1309_SET_PADS);
	ssd1309_write_command(0x12);
	ssd1309_write_command(SSD1309_SET_CHARGE);
	ssd1309_write_command(0x25);
	ssd1309_write_command(SSD1309_SET_VCOM);
	ssd1309_write_command(0x34);
	ssd1309_write_command(SSD1309_SET_SEG_REMAP1);
	ssd1309_write_command(SSD1309_SET_SCAN_NOR);
	ssd1309_write_command(SSD1309_EON_OFF);
	ssd1309_write_command(SSD1309_SET_CONTRAST);
	ssd1309_write_command (255);
	ssd1309_write_command(SSD1309_MASTER_CONFIG);
	ssd1309_write_command(0x8E);
	ssd1309_write_command(SSD1309_SET_AREA_COLOR);
	ssd1309_write_command(0x05);
	ssd1309_write_command(SSD1309_DISP_NOR);
	ssd1309_clear_display(); 
	ssd1309_write_command(SSD1309_DISP_ON);
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void ssd1309_set_line (uint8_t line) 
{
	ssd1309_write_command(SSD1309_SET_LINE | line);		
}

/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
void ssd1309_set_start_column (uint8_t d)
{
	ssd1309_write_command(0x00 + d%16);		
	ssd1309_write_command(0x10 + d/16);		
					
}

/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
void ssd1309_set_page (uint8_t page) 
{
	ssd1309_write_command(SSD1309_SET_PAGE | page);	
}

/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
void ssd1309_update_frame (uint8_t* buffer, uint32_t size) 
{
	unsigned char i,j;

	for(i=0; i<8 ;i++)
	{
		ssd1309_set_page(i);
		ssd1309_set_start_column(0x00);

		for(j=0;j<128;j++)
		{
			ssd1309_write_data(*buffer++);
		}
	}

}

/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
void ssd1309_clear_display (void)
{
	
	unsigned char i,j;

	for(i=0; i<8 ;i++)
	{
		ssd1309_set_page(i);
		ssd1309_set_start_column(0x00);

		for(j=0;j<128;j++)
		{
			ssd1309_write_data(0x00);
		}
	}
}


/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
void ssd1309_write_data (uint8_t data)
{

	hal_gpio_write(ssd1309_pin_dc, 1); 
	hal_gpio_write(ssd1309_pin_cs, 0); 
	hal_spi_transfer (ssd1309_spi, &data, 1, NULL, 0, 0);  
	hal_gpio_write(ssd1309_pin_cs, 1);  

}

/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
void ssd1309_write_command (uint8_t command)
{

	hal_gpio_write(ssd1309_pin_dc, 0); 
	hal_gpio_write(ssd1309_pin_cs, 0);
	hal_spi_transfer (ssd1309_spi, &command, 1, NULL, 0, 0); 
	hal_gpio_write(ssd1309_pin_cs, 1); 
}

/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
void ssd1309_reset (void)
{	
	hal_gpio_write(ssd1309_pin_rst, 0);
	hal_delay_ms (500);
	hal_gpio_write(ssd1309_pin_rst, 1);
	hal_delay_ms (500);
}


