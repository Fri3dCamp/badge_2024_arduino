#ifndef X_FRI3D_BADGE_2024
#define X_FRI3D_BADGE_2024 // General Define for use in sketches or lib files
#define X_WS2812_NUM_LEDS 5 // Number of RBG LEDs

#define PIN_I2C_SDA     9
#define PIN_I2C_SCL     18
#define PIN_WS2812       12
#define X_WS2812_NUM_LEDS 5

#define PIN_LED          21
#define PIN_IR_RECEIVER  11
#define PIN_BLASTER      10
#define PIN_BUZZER       46
#define PIN_BATTERY      13

#define PIN_SDCARD_CS    14

#define PIN_JOY_X         1
#define PIN_JOY_Y         3

#define PIN_A            39
#define PIN_B            40
#define PIN_X            38
#define PIN_Y            41
#define PIN_MENU         45
#define PIN_START         0

#define PIN_AUX          42 // Fri3d Badge 2024 Aux Pwr

#define CHANNEL_BUZZER 0

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
#endif //X_FRI3D_BADGE_2024
