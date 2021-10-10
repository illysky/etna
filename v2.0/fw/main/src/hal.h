#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "nrf_log.h" 

int32_t     hal_i2c_init (uint8_t instance, bool list);
int32_t     hal_i2c_read (uint8_t instance, uint8_t device, uint8_t* data, uint32_t size); 
int32_t     hal_i2c_write (uint8_t instance, uint8_t device, uint8_t* data, uint32_t size, bool nostop); 
void        hal_delay_ms (uint32_t ms); 
void        hal_delay_us (uint32_t us); 
void        hal_gpio_config_output (uint32_t pin, uint32_t init); 
void        hal_gpio_config_input (uint32_t pin, uint32_t pull); 
void        hal_gpio_write (uint32_t pin, uint32_t value);
void        hal_gpio_tog (uint32_t pin); 
uint32_t    hal_gpio_read (uint32_t pin); 
int32_t     hal_uart_init(void); 
int32_t     hal_uart_get (char* buf, uint32_t size); 
int32_t     hal_uart_putn (char* buf, uint32_t size); 
int32_t     hal_uart_puts (char* buf);
int32_t     hal_uart_putc (char c); 
bool        hal_timeout (uint32_t* timestamp, bool init, uint32_t expiry); 

int32_t hal_spi_transfer (uint8_t spi, uint8_t* tx, uint32_t tx_size, uint8_t* rx, uint32_t rx_size, uint8_t mode);
int32_t hal_spi_init (uint8_t spi, uint8_t miso, uint8_t mosi, uint8_t sck, uint8_t mode); 