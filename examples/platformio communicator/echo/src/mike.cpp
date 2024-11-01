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

size_t record(int8_t *buffer, bool (*canContinue)()) {
  // start up the I2S peripheral
  if (ESP_OK != i2s_driver_install(I2S_NUM_0, &i2s_mike, 0, NULL))
  {
    Serial.println("i2s_driver_install: error");
    return 0;
  }
  if (ESP_OK != i2s_set_pin(I2S_NUM_0, &i2s_mic_pins))
  {
    Serial.println("i2s_set_pin: error");
    return 0;
  }

  size_t read;
  size_t totalRead = 0;
  const size_t INCREMENT = 100;
  int32_t temp[INCREMENT];

  int8_t lowest = 0;
  int8_t highest = 0;
  while (totalRead < SAMPLE_BUFFER_SIZE-INCREMENT && canContinue()) {
    i2s_read(I2S_NUM_0, temp, INCREMENT*sizeof(int32_t), &read, portMAX_DELAY);
    
    //copy to actual buffer, and boost volume in the process:
    for (int i=0; i<read; ++i) {
      buffer[totalRead + i] = (int8_t) (temp[i] >> 24);
      if (buffer[totalRead + i] < lowest) lowest = buffer[totalRead +i];
      if (buffer[totalRead + i] > highest) highest = buffer[totalRead +i];
    }

    totalRead += (read>>2);
  }
  Serial.printf(" %u bytes read, lo = %hhd, hi = %hhd \n", totalRead, lowest, highest);

  i2s_driver_uninstall(I2S_NUM_0);
  return totalRead;
}