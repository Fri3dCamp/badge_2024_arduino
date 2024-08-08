#include "driver\gpio.h"
#include <freertos/FreeRTOS.h>
#include <driver/i2s.h>

// comment this out if you want to use the internal DAC
#define USE_I2S

// speaker settings - as found at the back of the communicator PCB:
#define I2S_SPEAKER_SERIAL_CLOCK gpio_num_t::GPIO_NUM_2
#define I2S_SPEAKER_LEFT_RIGHT_CLOCK gpio_num_t::GPIO_NUM_47
#define I2S_SPEAKER_SERIAL_DATA gpio_num_t::GPIO_NUM_16
//#define I2S_SPEAKDER_SD_PIN gpio_num_t::GPIO_NUM_5

// volume control - if required
//#define VOLUME_CONTROL ADC1_CHANNEL_7

// button - GPIO 0 is the built in button on most dev boards
// On the Fri3d badge, this is the "START" button.
#define GPIO_BUTTON gpio_num_t::GPIO_NUM_0

// i2s speaker pins definition
extern i2s_pin_config_t i2s_speaker_pins;
