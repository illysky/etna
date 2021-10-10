/*****************************************************************************************************************************************************************************************
 * @file						
 * @brief: 	
 * @version:			
 * @author: 		 Created by Dominic Moffat on 11/12/2017. Copyright © 2017 Illysky. All rights reserved.										
*****************************************************************************************************************************************************************************************/

#include "sst26.h"
#include "hal.h"

static sst26_cs_gpio_t 	cs_gpio_func = NULL; 

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void sst26_init (sst26_cs_gpio_t cs)
{
	cs_gpio_func = cs; 
	if (cs_gpio_func) cs_gpio_func(1);  // Deassert 

	sst26_reset (); 
	hal_delay_ms(150);

}


/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void sst26_cs_assert (void) 
{
	if (cs_gpio_func) cs_gpio_func(0);  // Assert
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void sst26_cs_deassert (void) 
{
	if (cs_gpio_func) cs_gpio_func(1);  // Deassert 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void sst26_wren (void) 
{
	uint8_t command = SST26_WREN; 
	sst26_cs_assert ();
	hal_spi_transfer (&command, 1, NULL, 0); 
	sst26_cs_deassert();
} 


/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void sst26_readid (uint16_t* id)
{
	uint8_t tx[8]; 
	uint8_t rx[8]; 
	sst26_cs_assert ();
	tx[0] = SST26_IDREAD; 
	hal_spi_transfer (tx, 4, rx, 4); 
	sst26_cs_deassert();

	*id = ((uint16_t) rx[1]) << 8; 
	*id |= ((uint16_t) rx[2]) << 0; 
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void sst26_sfdp (uint32_t addr, uint8_t* sfdp, uint32_t size)
{
	uint8_t buf[8] = {};
	buf[0] = SST26_SFDP; 
	buf[1] = addr >> 16; 
	buf[2] = addr >> 8; 
	buf[3] = addr >> 0; 
	buf[4] = 0x00;  	// Dummy!
	sst26_cs_assert ();
	hal_spi_transfer (buf, 5, NULL, 0);  		// Addr + Dummy
	hal_spi_transfer (NULL, 0, sfdp, size); 	// Read
	sst26_cs_deassert();
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void sst26_rbpr(uint8_t* rbpr)
{
	uint8_t command = SST26_RBPR; 
	sst26_cs_assert ();
	hal_spi_transfer (&command, 1, NULL, 0); 
	hal_spi_transfer (NULL, 0, rbpr, 6); 
	sst26_cs_deassert();
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void sst26_ulbpr(void)
{
	
	sst26_wren (); 

	uint8_t command = SST26_ULBPR; 
	sst26_cs_assert ();
	hal_spi_transfer (&command, 1, NULL, 0); 
	sst26_cs_deassert();
}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void sst26_reset (void)
{
	uint8_t command = SST26_RSTEN; 
	sst26_cs_assert ();
	hal_spi_transfer (&command, 1, NULL, 0); 
	sst26_cs_deassert();

	command = SST26_RST; 
	sst26_cs_assert ();
	hal_spi_transfer (&command, 1, NULL, 0); 
	sst26_cs_deassert();

}

/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void sst26_status (uint8_t * status) 
{
	uint8_t command = SST26_STATUSREAD; 
	sst26_cs_assert ();
	hal_spi_transfer (&command, 1, NULL, 0); 
	hal_spi_transfer (NULL, 0, status, 1); 
	sst26_cs_deassert();
}



/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
void sst26_sector_erase (uint32_t addr)
{
	sst26_wren (); 

	uint8_t buf[8];
	buf[0] = SST26_BLOCKERASE_4K; 
	buf[1] = addr >> 16; 
	buf[2] = addr >> 8; 
	buf[3] = addr >> 0; 
	sst26_cs_assert ();
	hal_spi_transfer (buf, 4, NULL, 0); 
	sst26_cs_deassert();

	uint8_t status = 0; 
	do 
	{
		sst26_status (&status); 
	} while (status & 0x01); 

}

/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
void sst26_program (uint32_t addr, uint8_t* data, uint32_t len)
{
	sst26_wren (); 
	
	uint8_t buf[8];
	buf[0] = SST26_PAGEPROGRAM; 
	buf[1] = addr >> 16; 
	buf[2] = addr >> 8; 
	buf[3] = addr >> 0; 
	sst26_cs_assert ();
	hal_spi_transfer (buf, 4, NULL, 0); 
	hal_spi_transfer (data, len, NULL, 0); 
	sst26_cs_deassert();
	
	uint8_t status = 0; 
	do 
	{
		sst26_status (&status); 
	} while (status & 0x01); 

}

/***********************************************************************************************************************************************************//**
 * @function     	
 * @brief      	
***************************************************************************************************************************************************************/
void sst26_read (uint32_t addr, uint8_t* data, uint32_t len )
{
	uint8_t buf[8]; 
	sst26_cs_assert ();
	buf[0] = SST26_ARRAYREADLOWFREQ; 
	buf[1] = addr >> 16; 
	buf[2] = addr >> 8; 
	buf[3] = addr >> 0; 
	hal_spi_transfer (buf, 4, NULL, 0); 
	hal_spi_transfer (NULL, 0, data, len); 
	sst26_cs_deassert();
}


/************************************************************************************** 
 * @brief	
 * 	
 **************************************************************************************/
void sst26_wrdi (void) 
{
	uint8_t command = SST26_WRDI; 
	sst26_cs_assert ();
	hal_spi_transfer (&command, 1, NULL, 0); 
	sst26_cs_deassert();
} 
