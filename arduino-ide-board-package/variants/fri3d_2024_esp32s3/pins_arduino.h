#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>
#include "soc/soc_caps.h"

#undef CONFIG_APP_ROLLBACK_ENABLE // Disable this because otherwise cancel_rollback will be added to the arduino programs
#undef CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE

#define USB_VID 0x303a
#define USB_PID 0x1001

#define EXTERNAL_NUM_INTERRUPTS 46
#define NUM_DIGITAL_PINS        48
#define NUM_ANALOG_INPUTS       20

static const uint8_t LED_BUILTIN = 21;
#define BUILTIN_LED  LED_BUILTIN // backward compatibility
#define LED_BUILTIN LED_BUILTIN
#define RGB_BUILTIN LED_BUILTIN
#define RGB_BRIGHTNESS 64

#define analogInputToDigitalPin(p)  (((p)<20)?(analogChannelToDigitalPin(p)):-1)
#define digitalPinToInterrupt(p)    (((p)<49)?(p):-1)
#define digitalPinHasPWM(p)         (p < 46)

static const uint8_t TX = 43;
static const uint8_t RX = 44;

static const uint8_t SDA = 9;
static const uint8_t SCL = 18;

static const uint8_t SS    = 14;
static const uint8_t MOSI  = 6;
static const uint8_t MISO  = 8;
static const uint8_t SCK   = 7;

// General Define for use in sketches or lib files
#define X_FRI3D_BADGE_2024

// Fri3d Badge 2024 WS2812
#define X_WS2812_LED 12
#define X_WS2812_NUM_LEDS 5

#define X_BATTERY_MONITOR 13
#define X_BLASTER 10
#define X_BUZZER 46
#define X_IR_RECEIVER 11

// Fri3d Badge 2024 Buttons
#define X_BUTTON_A 39
#define X_BUTTON_B 40
#define X_BUTTON_X 38
#define X_BUTTON_Y 41
#define X_BUTTON_MENU 45
#define X_BUTTON_START 0

// Fri3d Badge 2024 Joystick
#define X_JOYSTICK_VERTICAL 3
#define X_JOYSTICK_HORIZONTAL 1

// Fri3d Badge 2024 Aux Pwr
#define X_AUX_PWR 42

// Fri3d Badge 2024 Accelero Gyro
#define X_ACCELERO_GYRO 21

// I2S microphone on communicator addon
#define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT
#define I2S_MIC_SERIAL_CLOCK 17 //serial clock SCLK: pin SCK
#define I2S_MIC_LEFT_RIGHT_CLOCK 47 //left/right clock LRCK: pin WS
#define I2S_MIC_SERIAL_DATA 15 //serial data DIN: pin SD


// Fri3d Badge 2024 LCD
// For using display with TFT_eSPI library
#define USER_SETUP_LOADED // TODO check if this is really needed
#define SPI_FREQUENCY  80000000
#define ST7789_DRIVER
#define USE_HSPI_PORT

#define TFT_RGB_ORDER TFT_BGR //# swap red and blue byte order
#define TFT_INVERSION_OFF
#define TFT_WIDTH 296  //;setting these will init the eSPI lib with correct dimensions
#define TFT_HEIGHT 240 //;setting these will init the eSPI lib with correct dimensions
#define TFT_MISO MISO
#define TFT_MOSI MOSI
#define TFT_SCLK SCK
#define TFT_CS 5
#define TFT_DC 4
#define TFT_RST 48
#define LOAD_GLCD 1
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT
#define SPI_FREQUENCY 80000000


static const uint8_t A0 = 1;
static const uint8_t A1 = 2;
static const uint8_t A2 = 3;
static const uint8_t A3 = 4;
static const uint8_t A4 = 5;
static const uint8_t A5 = 6;
static const uint8_t A6 = 7;
static const uint8_t A7 = 8;
static const uint8_t A8 = 9;
static const uint8_t A9 = 10;
static const uint8_t A10 = 11;
static const uint8_t A11 = 12;
static const uint8_t A12 = 13;
static const uint8_t A13 = 14;
static const uint8_t A14 = 15;
static const uint8_t A15 = 16;
static const uint8_t A16 = 17;
static const uint8_t A17 = 18;
static const uint8_t A18 = 19;
static const uint8_t A19 = 20;

static const uint8_t T1 = 1;
static const uint8_t T2 = 2;
static const uint8_t T3 = 3;
static const uint8_t T4 = 4;
static const uint8_t T5 = 5;
static const uint8_t T6 = 6;
static const uint8_t T7 = 7;
static const uint8_t T8 = 8;
static const uint8_t T9 = 9;
static const uint8_t T10 = 10;
static const uint8_t T11 = 11;
static const uint8_t T12 = 12;
static const uint8_t T13 = 13;
static const uint8_t T14 = 14;

#endif /* Pins_Arduino_h */
