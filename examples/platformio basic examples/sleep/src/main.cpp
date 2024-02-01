#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI    tft = TFT_eSPI();

// Pins 0-21 are RTC pins, others cannot be used. We'll use pin 0 (START button) to wake up
#define BUTTON_PIN_BITMASK 0x000000001 // 2^0 in hex

void fri3d_sleep()
{
  esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ANY_LOW);
  esp_deep_sleep_start();
}

void setup()
{
  // put your setup code here, to run once:
  tft.init(TFT_BLACK);
  tft.setRotation(3);
  tft.writecommand(TFT_MADCTL);
  tft.writedata(TFT_MAD_BGR | TFT_MAD_MV); //exchange red and blue bytes, and mirror x-coordinates
  tft.fillScreen(TFT_BLACK);

  tft.setTextSize(2);
  tft.setCursor(40,30); 
  tft.println("Going to sleep");
  tft.setCursor(40,60); 
  tft.println("in 5 seconds");
  tft.setCursor(40,130); 
  tft.println("Press START or RESET");
  tft.setCursor(40,160); 
  tft.println("to wake me up");
  delay(5000);
  fri3d_sleep();
  // code will never be reached
}

void loop()
{
  // code will never be reached
  delay(1000);
}
