#include <Arduino.h>
#include "pins_arduino.h"
#include <TFT_eSPI.h>

TFT_eSPI    tft = TFT_eSPI();

#define PIN_PWR_AUX GPIO_NUM_42

// Pins 0-21 are RTC pins, others cannot be used. We'll use pin 0 (START button) to wake up
#define PIN_BITMASK_BUTTON_START 0x000000001 // 2^0 in hex

void fri3d_sleep()
{
  esp_sleep_enable_ext1_wakeup(PIN_BITMASK_BUTTON_START, ESP_EXT1_WAKEUP_ANY_LOW); // alternative: esp_sleep_enable_ext0_wakeup(GPIO_NUM_0,LOW);
  pinMode(PIN_PWR_AUX,OUTPUT);
  digitalWrite(PIN_PWR_AUX,LOW);
  gpio_hold_en(PIN_PWR_AUX);
  gpio_deep_sleep_hold_en();
  delay(100);
  esp_deep_sleep_start();
}

void setup()
{
  // PIN PWR AUX (42) was kept low during sleep, but after wake up it should return to its default state HIGH
  gpio_hold_dis(PIN_PWR_AUX);
  gpio_deep_sleep_hold_dis();

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
