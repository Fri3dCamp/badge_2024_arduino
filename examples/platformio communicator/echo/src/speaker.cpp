#include <Arduino.h>
#include <driver/i2s.h>
#include "pins2024.h"
#include "echo.h"

i2s_config_t i2s_spk = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_8BIT, //mike didn't seem to work with 16 bits
    .channel_format = I2S_CHANNEL_FMT_ALL_LEFT, //this format seems to equal mike raw format
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

i2s_pin_config_t i2s_spk_pins = {
    .mck_io_num = 0, // Unused
    .bck_io_num = I2S_SPEAKER_SERIAL_CLOCK,
    .ws_io_num = I2S_SPEAKER_LEFT_RIGHT_CLOCK,
    .data_out_num = I2S_SPEAKER_SERIAL_DATA,
    .data_in_num = I2S_PIN_NO_CHANGE
};

void playback(int8_t *buffer, size_t length) {
    if (i2s_driver_install(I2S_NUM_0, &i2s_spk, 0, NULL) != ESP_OK)
    {
        Serial.println("ERROR: Unable to install I2S drives\n");
        return;
    }
    i2s_set_pin(I2S_NUM_0, &i2s_spk_pins);

    size_t written;
    i2s_write(I2S_NUM_0, buffer, length, &written, portMAX_DELAY);
    Serial.print(written);
    Serial.println(" bytes written");

    i2s_driver_uninstall(I2S_NUM_0);
}