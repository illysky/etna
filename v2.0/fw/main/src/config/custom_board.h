#ifndef BOARD_CUSTOM_H
#define BOARD_CUSTOM_H

#ifdef __cplusplus
extern "C" {
#endif


//#define HWFC           false

// ADC
#define ADC_INTERNAL_VREF       		    600.0f    
#define ADC_RESOLUTION                 		(1<<14)     
#define ADC_RAW_TO_MS(ADC_VALUE, ADC_GAIN)  ((((ADC_VALUE) * ADC_INTERNAL_VREF) / ADC_RESOLUTION) * ADC_GAIN)

// Peripherals 
#define I2C0_SDA_IO                         NRF_GPIO_PIN_MAP(0, 31)
#define I2C0_SCL_IO                         NRF_GPIO_PIN_MAP(0, 30)

#define SPI1_MOSI_IO                        NRF_GPIO_PIN_MAP(0, 2)
#define SPI1_MISO_IO                        NRF_GPIO_PIN_MAP(0, 3)
#define SPI1_SCK_IO                         NRF_GPIO_PIN_MAP(0, 28)

#define BUT_0_IO                            NRF_GPIO_PIN_MAP(0, 9)
#define BUT_1_IO                            NRF_GPIO_PIN_MAP(0, 20)

#define OLED_CS_IO                          NRF_GPIO_PIN_MAP(0, 4)
#define OLED_RST_IO                         NRF_GPIO_PIN_MAP(0, 5)
#define OLED_DC_IO                          NRF_GPIO_PIN_MAP(1, 9)

#define ADS1118_CS_IO                       NRF_GPIO_PIN_MAP(0, 15)
#define CARTR_DET_IO                        NRF_GPIO_PIN_MAP(0, 17)
#define AW9_INT                             NRF_GPIO_PIN_MAP(0, 29)
#define HEATER_PWM_IO                       NRF_GPIO_PIN_MAP(0, 11)

#ifdef __cplusplus
}
#endif

#endif 

