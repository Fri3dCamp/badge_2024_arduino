#include <Arduino.h>
#include <driver/i2s.h>
#include "pins2024.h"
#include "echo.h"

i2s_config_t i2s_mike = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT, //I couldn't get it to work with 16bits
    .channel_format = I2S_MIC_CHANNEL,
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 2, 0)
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
#else
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S),
#endif
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    //  .dma_buf_len = 1024,
    .dma_buf_len = 128,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0,
    /* // probably not necessary
    #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 0)
      .mclk_multiple = I2S_MCLK_MULTIPLE_DEFAULT, // Unused
      .bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT // Use bits per sample
    #endif
    */
};

i2s_pin_config_t i2s_mic_pins = {
    .bck_io_num = I2S_MIC_SERIAL_CLOCK,
    .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_SERIAL_DATA};

void record(int32_t *buffer) {
  // start up the I2S peripheral
  if (ESP_OK != i2s_driver_install(I2S_NUM_0, &i2s_mike, 0, NULL))
  {
    Serial.println("i2s_driver_install: error");
    return;
  }
  if (ESP_OK != i2s_set_pin(I2S_NUM_0, &i2s_mic_pins))
  {
    Serial.println("i2s_set_pin: error");
    return;
  }

  size_t read;
  i2s_read(I2S_NUM_0, buffer, SAMPLE_BUFFER_SIZE*sizeof(int32_t), &read, portMAX_DELAY);
  Serial.print(read);
  Serial.println(" bytes read");

  //boost volume
  for (int i=0; i<SAMPLE_BUFFER_SIZE; ++i) {
    buffer[i] *= 16;
  }

  i2s_driver_uninstall(I2S_NUM_0);
}