#define PIN_LEDS 12
#define PIN_WS2812 12
#define PIN_BUZZER 46
#define PIN_IR_RECEIVER 11
#define PIN_BLASTER 10

//buttons:
#define PIN_JOY_Y 3
#define PIN_JOY_X 1
#define PIN_A 39
#define PIN_B 40
#define PIN_X 38
#define PIN_Y 41
#define PIN_MENU 42
#define PIN_START 0

// SPI devices:
#define SPI_MISO 8
#define SPI_MOSI 6
#define SPI_SCLK 7

#define LCD_CS 5
#define LCD_RS 4
#define LCD_RSTn 48
#define LCD_WIDTH  296
#define LCD_HEIGHT 240
#define SDCARD_CS 14

// I2C devices:
#define PIN_I2C_SCL 18
#define PIN_I2C_SDA 9
#define ACCEL_ID 0x6B
#define PIN_SAO 13

// pins related to communicator add-on:
#define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT
#define I2S_MIC_SERIAL_CLOCK         17 // serial clock SCLK: pin SCK
#define I2S_MIC_LEFT_RIGHT_CLOCK     47 // left/right clock LRCK: pin WS
#define I2S_MIC_SERIAL_DATA          15 // serial data DIN: pin SD
#define I2S_SPEAKER_SERIAL_CLOCK      2 // serial clock SCLK -> SCLK,BCLK
#define I2S_SPEAKER_LEFT_RIGHT_CLOCK 47 // left/right clock LRCK -> WSEL,LRC
#define I2S_SPEAKER_SERIAL_DATA      16 // serial data DOUT -> DIN
