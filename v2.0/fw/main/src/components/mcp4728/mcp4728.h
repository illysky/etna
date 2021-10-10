/************************************************************************************************************************************* 
 * @file  	
 * @brief 	
 *************************************************************************************************************************************/
#ifndef MCP4728_H
#define MCP4728_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define MCP4728_CMD_FAST                0x00
#define MCP4728_CMD_INPUT               0x02
#define MCP4728_CMD_ADDR                0x03
#define MCP4728_CMD_VREF                0x04
#define MCP4728_CMD_PD                  0x05
#define MCP4728_CMD_GAIN                0x06

#define MCP4728_CMD_WR_MULTI            0x00
#define MCP4728_CMD_WR_SEQ              0x02
#define MCP4728_CMD_WR_SINGLE           0x03

#define MCP4728_PD_NORMAL               0x00
#define MCP4728_PD_1K                   0x01
#define MCP4728_PD_100K                 0x02
#define MCP4728_PD_500K                 0x03

#define MCP4728_GAIN_1                  0x00
#define MCP4728_GAIN_2                  0x01

#define MCP4728_UDAC_LATCH              0x0
#define MCP4728_UDAC_NOLATCH            0x01

#define MCP4728_VREF_VDD                0x00
#define MCP4728_VREF_2048               0x01

int32_t mcp4728_init (uint8_t i2c, uint8_t device); 
int32_t mcp4728_config_channel (uint8_t i2c, uint8_t device, uint8_t channel, uint8_t vref, uint8_t gain, uint8_t pd, uint16_t value); 
int32_t mcp4728_set (uint8_t i2c, uint8_t device, uint8_t channel, uint16_t value); 
int32_t mcp4728_channel_vref (uint8_t i2c, uint8_t device, uint8_t channel, uint8_t vref); 
int32_t mcp4728_channel_gain (uint8_t i2c, uint8_t device, uint8_t channel, uint8_t gain); 
int32_t mcp4728_channel_pd (uint8_t i2c, uint8_t device, uint8_t channel, uint8_t pd); 


#ifdef __cplusplus
}
#endif

#endif //MCP4728_H