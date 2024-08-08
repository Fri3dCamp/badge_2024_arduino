# ESP32 MP3 Player

This example is an adaptation of <https://github.com/atomic14/esp32-play-mp3-demo>
This repo contains a simple demonstration of how to play an MP3 file on the ESP32.

You'll need to use PlatformIO to run the code but you should be able to take the code accross to Arduino quite easily.

The original project could switch between built-in DAC and I2S DAC, but the code for
the built-in DAC didn't seem to compile, I was lazy, and the built-in DAC is shite
anyway, so I removed the corresponding c files. Cfr the [original project](https://github.com/atomic14/esp32-play-mp3-demo) if you want to make it work on the Fri3d
2022 (Octopus) badge.

Make sure you upload the filesystem as the sample mp3 file is stored on SPIFFS.
Sample mp3 all rights reserved Lynyrd Skynyrd; it was used for educational purposes.

## Configuration

All the configuration settings are in the `config.h` file.

```c
// comment this line out if you want to use the built-in DAC
// you can't use the built-in DAC unless you add the corresponding c files again.
#define USE_I2S

// speaker settings - as found at the back of the communicator PCB:
#define I2S_SPEAKER_SERIAL_CLOCK gpio_num_t::GPIO_NUM_2
#define I2S_SPEAKER_LEFT_RIGHT_CLOCK gpio_num_t::GPIO_NUM_47
#define I2S_SPEAKER_SERIAL_DATA gpio_num_t::GPIO_NUM_16
//#define I2S_SPEAKDER_SD_PIN gpio_num_t::GPIO_NUM_5

// volume control - if required - comment this out if you don't want it
//#define VOLUME_CONTROL ADC1_CHANNEL_7

// button - GPIO 0 is the built in button on most dev boards
// On the Fri3d badge, this is the "START" button.
#define GPIO_BUTTON GPIO_NUM_0
```
