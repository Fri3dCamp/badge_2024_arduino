#include <Arduino.h>

// Updated version of https://github.com/atomic14/esp32-i2s-mic-test/blob/main/i2s_mic_test/i2s_mic_test.ino

#include <driver/i2s.h>

// you shouldn't need to change these settings
#define SAMPLE_BUFFER_SIZE 512
#define SAMPLE_RATE 8000

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
    /* // probably not necessary
    #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 0)
      .mclk_multiple = I2S_MCLK_MULTIPLE_DEFAULT, // Unused
      .bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT // Use bits per sample
    #endif
    */
};

// and don't mess around with this
i2s_pin_config_t i2s_mic_pins = {
    .bck_io_num = I2S_MIC_SERIAL_CLOCK,
    .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_SERIAL_DATA};


void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(3000);
  Serial.println("ESP32S3 - INMP441 - I2S microphone test");

  Serial.print("I2S_MIC_SERIAL_CLOCK: ");
  Serial.println(I2S_MIC_SERIAL_CLOCK);
  Serial.print("I2S_MIC_LEFT_RIGHT_CLOCK: ");
  Serial.println(I2S_MIC_LEFT_RIGHT_CLOCK);
  Serial.print("I2S_MIC_SERIAL_DATA: ");
  Serial.println(I2S_MIC_SERIAL_DATA);

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

int32_t raw_samples[SAMPLE_BUFFER_SIZE];
void loop()
{
  // read from the I2S device
  size_t bytes_read = 0;
  i2s_read(I2S_NUM_0, raw_samples, sizeof(int32_t) * SAMPLE_BUFFER_SIZE, &bytes_read, portMAX_DELAY);
  int samples_read = bytes_read / sizeof(int32_t);
  // dump the samples out to the serial channel.
  for (int i = 0; i < samples_read; i++)
  {
    Serial.printf("%15ld\n", raw_samples[i] >> 19);
  }
}
