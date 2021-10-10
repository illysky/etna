#ifndef __SST26_H
#define __SST26_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define SST26_RSTEN            0x66        // Reset Enable
#define SST26_RST              0x99        // Reset 

#define SST26_WREN             0x06        // Write enable
#define SST26_WRDI             0x04        // Write disable

#define SST26_BLOCKERASE_4K    0x20        // Erase one 4K block of flash memory
#define SST26_BLOCKERASE_32K   0x52        // Erase one 32K block of flash memory
#define SST26_BLOCKERASE_64K   0xD8        // Erase one 64K block of flash memory
#define SST26_CHIPERASE        0x60        // Chip erase (may take several seconds depending on size)
                                              // but no actual need to wait for completion (instead need to check the status register BUSY bit)
#define SST26_STATUSREAD       0x05        // Read status register
#define SST26_STATUSWRITE      0x01        // Write status register
#define SST26_ARRAYREAD        0x0B        // Read array (fast, need to add 1 dummy byte after 3 address bytes)
#define SST26_ARRAYREADLOWFREQ 0x03        // Read array (low frequency)


#define SST26_RBPR            0x72
#define SST26_ULBPR           0x98

#define SST26_SLEEP            0xB9        // Deep power down
#define SST26_WAKE             0xAB        // Deep power wake up
#define SST26_PAGEPROGRAM      0x02        // Write (1 to 256bytes)
#define SST26_IDREAD           0x9F        // Read JEDEC manufacturer and device ID (2 bytes, specific bytes for each manufacturer and device)
                                              // Example for Atmel-Adesto 4Mbit AT25DF041A: 0x1F44 (page 27: http://www.adestotech.com/sites/default/files/datasheets/doc3668.pdf)
                                              // Example for Winbond 4Mbit W25X40CL: 0xEF30 (page 14: http://www.winbond.com/NR/rdonlyres/6E25084C-0BFE-4B25-903D-AE10221A0929/0/W25X40CL.pdf)
#define SST26_MACREAD          0x4B        // Read unique ID number (MAC)
#define SST26_SFDP             0x5A        // Read unique ID number (MAC)




typedef void (*sst26_cs_gpio_t)(uint8_t value);

void    sst26_init (sst26_cs_gpio_t cs);  
void    sst26_cs_assert (void); 
void    sst26_cs_assert (void); 
void sst26_status (uint8_t * status); 
void    sst26_wren (void); 
void    sst26_read (uint32_t addr, uint8_t* buf, uint32_t len ); 
void    sst26_write (uint32_t addr, uint8_t* data, uint32_t len ); 
void    sst26_readid (uint16_t* id);
void sst26_wrdi (void); 
void sst26_rbpr (uint8_t* rbpr); 
void sst26_ulbpr(void); 
void sst26_sfdp (uint32_t addr, uint8_t* sfdp, uint32_t size);

void sst26_program (uint32_t addr, uint8_t* data, uint32_t len); 
void sst26_sector_erase (uint32_t addr); 
void sst26_reset (void); 


#ifdef __cplusplus
}
#endif
#endif





