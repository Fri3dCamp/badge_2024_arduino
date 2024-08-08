#include <Arduino.h>
#include <driver/i2s.h>

#define PIN_EXPANSION_RX 44
#define PIN_EXPANSION_TX 43

// you shouldn't need to change these settings
#define SAMPLE_BUFFER_SIZE 512
#define SAMPLE_RATE 8000

#define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT

#define I2S_MIC_SERIAL_CLOCK         17 // serial clock SCLK: pin SCK
#define I2S_MIC_LEFT_RIGHT_CLOCK     47 // left/right clock LRCK: pin WS
#define I2S_MIC_SERIAL_DATA          15 // serial data DIN: pin SD
#define I2S_SPEAKER_SERIAL_CLOCK      2 // serial clock SCLK -> SCLK,BCLK
#define I2S_SPEAKER_LEFT_RIGHT_CLOCK 47 // left/right clock LRCK -> WSEL,LRC
#define I2S_SPEAKER_SERIAL_DATA      16 // serial data DOUT -> DIN

// don't mess around with this
i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_MIC_CHANNEL,
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 2, 0)
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
#else
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S),
#endif
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    //  .dma_buf_len = 1024,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0,
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 0)
    .mclk_multiple = I2S_MCLK_MULTIPLE_DEFAULT, // Unused
    .bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT  // Use bits per sample
#endif
};

// and don't mess around with this
i2s_pin_config_t i2s_mic_pins = {
    .bck_io_num = I2S_MIC_SERIAL_CLOCK,
    .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_SERIAL_DATA};

#include "TFT_eSPI.h"

TFT_eSPI tft = TFT_eSPI();

TFT_eSprite mic_sprite = TFT_eSprite(&tft);
#define MIC_SPRITE_WIDTH 220
#define MIC_SPRITE_HEIGHT 100

TFT_eSprite uart_sprite = TFT_eSprite(&tft);
#define UART_SPRITE_WIDTH 260
#define UART_SPRITE_HEIGHT 100

#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorRTTTL.h"
#include "AudioOutputI2S.h"

const char song[] PROGMEM =
    "Beep:d=8,o=5,b=250:g,4a";

void uart_sprite_setup()
{
  uart_sprite.createSprite(UART_SPRITE_WIDTH, UART_SPRITE_HEIGHT);
  uart_sprite.setFreeFont(&FreeMono9pt7b);
  uart_sprite.setTextSize(1);
  Serial2.begin(115200, SERIAL_8N1, PIN_EXPANSION_RX, PIN_EXPANSION_TX);
  Serial2.flush(false); // Flush TX & RX
}

void uart_sprite_update()
{
  static int count = 0;
  boolean updatesprite = false;

  while (Serial2.available())
  {
    char ch = Serial2.read();

    if (count % 40 == 0)
    {
      uart_sprite.fillSprite(TFT_BLACK);
      // uart_sprite.drawRect(0, 0, UART_SPRITE_WIDTH, UART_SPRITE_HEIGHT, TFT_WHITE);
      uart_sprite.setCursor(0, 10);
    }

    uart_sprite.printf("%02X ", (unsigned char)ch);

    updatesprite = true;
    ++count;
  }
  if (updatesprite)
  {
    uart_sprite.pushSprite(20, 140);
  }
}

void mic_sprite_setup()
{
  mic_sprite.createSprite(MIC_SPRITE_WIDTH, MIC_SPRITE_HEIGHT);

  // start up the I2S peripheral
  if (ESP_OK != i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL))
  {
    Serial.println("i2s_driver_install: error");
  }
  if (ESP_OK != i2s_set_pin(I2S_NUM_0, &i2s_mic_pins))
  {
    Serial.println("i2s_set_pin: error");
  }
}

void mic_sprite_show()
{
  int32_t raw_samples[SAMPLE_BUFFER_SIZE];

  mic_sprite.fillSprite(TFT_BLACK);
  mic_sprite.drawRect(0, 0, MIC_SPRITE_WIDTH, MIC_SPRITE_HEIGHT, TFT_WHITE);

  // read from the I2S device
  size_t bytes_read = 0;
  i2s_read(I2S_NUM_0, raw_samples, sizeof(int32_t) * SAMPLE_BUFFER_SIZE, &bytes_read, portMAX_DELAY);
  int num_samples = bytes_read / sizeof(int32_t);
  // dump the samples out to the serial channel.

  if (num_samples > 100)
  {
    const float x_step = (float)MIC_SPRITE_WIDTH / (float)num_samples;
    const float height2 = (float)MIC_SPRITE_HEIGHT / 2;

    float sample_x = 0;
    float prev_sample_y = 0;
    for (int i = 4; i < num_samples; i += 4)
    {
      float sample_y = (float)(raw_samples[i] >> 19);
      if (sample_y > height2)
      {
        sample_y = height2;
      }
      if (sample_y < -height2)
      {
        sample_y = -height2;
      }
      mic_sprite.drawLine(sample_x, height2 + prev_sample_y, sample_x + x_step * 4, height2 + sample_y, 0xfff);
      prev_sample_y = sample_y;
      sample_x += x_step * 4;
    }
    mic_sprite.pushSprite(40, 30);
  }
}

void setup_display()
{
  tft.init();
  tft.writecommand(TFT_MADCTL);
  tft.writedata(TFT_MAD_BGR | TFT_MAD_MV); // exchange red and blue bytes, and mirror x-coordinates
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(40, 10);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.println("Fri3d Communicator");
}

void speaker_test()
{
  AudioFileSourcePROGMEM file(song, strlen_P(song));
  AudioOutputI2S out;
  out.SetPinout(I2S_SPEAKER_SERIAL_CLOCK, I2S_SPEAKER_LEFT_RIGHT_CLOCK, I2S_SPEAKER_SERIAL_DATA);
  AudioGeneratorRTTTL rtttl;
  rtttl.begin(&file, &out);
  while (rtttl.loop())
  {
  }
  rtttl.stop();
  Serial.printf("RTTTL done\n");
  i2s_driver_uninstall(I2S_NUM_0);
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  // delay(3000);
  Serial.println("Fri3d Badge 2024 - Communicator test");

  setup_display();
  speaker_test();
  mic_sprite_setup();
  uart_sprite_setup();
}

void loop()
{
  mic_sprite_show();
  uart_sprite_update();
}
