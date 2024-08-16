#include <Arduino.h>
#include "pins_arduino.h"

// This is a hello world example of using the lovyanGFX TFT driver for the display

#include "LGFX_ESP32_Fri3dBadge_ST7789.h"

LGFX tft;

void setup(void)
{
  Serial.begin(115200);
  delay(3000);
  Serial.println("Hello world - LovyanGFX - Hello World");

  tft.init();
  tft.setRotation(TFT_ROTATION);
  tft.setTextSize((std::max(tft.width(), tft.height()) + 255) >> 8);
  tft.fillScreen(TFT_BLACK);
  tft.drawRect(0, 0, tft.width(), tft.height(), TFT_BLUE);
  tft.setCursor(10, tft.height()/2-10);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(3);
  tft.println("Hello world");

  delay(2000);
}

void loop(void) {
  delay(1000);
}
