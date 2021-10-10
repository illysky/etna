/************************************************************************************************************************************* 
 * @file  	
 * @brief 	
 *************************************************************************************************************************************/
#ifndef MCP4726_H
#define MCP4726_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <string.h>


// Programmable Gain definitions
#define MCP4726_GAIN_1X	                0x00
#define MCP4726_GAIN_2X	                0x01

// Power Down Mode definitions
#define MCP4726_AWAKE                   0x00
#define MCP4726_PWRDN_1K                0x01
#define MCP4726_PWRDN_100K              0x02
#define MCP4726_PWRDN_500K              0x03

// Reference Voltage definitions
#define MCP4726_VREF_VDD                0x00
#define MCP4726_VREF_VREFPIN	        0x02
#define MCP4726_VREF_VREFPIN_BUFFERED   0x03

// Command definitioins
#define MCP4726_CMD_VOLDAC              0x00
#define MCP4726_CMD_VOLALL              0x40
#define MCP4726_CMD_VOLCONFIG           0x80
#define MCP4726_CMD_ALL                 0x60

int32_t mcp4726_init (uint8_t addr); 
int32_t mcp4726_write_dac(uint16_t value); 
int32_t mcp4726_write_conf (uint8_t gain, uint8_t vref); 
int32_t mcp4726_write_pd (uint8_t pd); 



#ifdef __cplusplus
}
#endif

#endif //MCP4726_H