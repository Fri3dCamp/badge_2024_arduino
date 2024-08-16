#include <Arduino.h>
#include "Fri3dBadge_pins.h"
#include <TFT_eSPI.h>

TFT_eSPI    tft = TFT_eSPI();

#include "Fri3dBadge24_battery_monitor.h"

void setup() {
  Serial.begin(115200);
  delay(3000);

  tft.init(TFT_BLACK);
  tft.initDMA();
  tft.setRotation(3);
  tft.writecommand(TFT_MADCTL);
  tft.writedata(TFT_MAD_BGR | TFT_MAD_MV); //exchange red and blue bytes, and mirror x-coordinates

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(30, 10);
  tft.print("battery monitor");
}

uint16_t bat_raw;
uint8_t bat_percent;

void loop() {

  bat_raw = Battery::read_raw();
  Serial.print("Battery raw value: ");
  Serial.println(bat_raw);
  tft.setCursor(10, 30);
  tft.printf("bat_raw: %d", bat_raw);

  bat_percent = Battery::raw_to_percent(bat_raw);
  Serial.print("Battery raw value converted to percent: ");
  Serial.print(bat_percent);
  Serial.println("%");
  tft.setCursor(10, 50);
  tft.printf("bat_percent: %d%%  ", bat_percent);

  Serial.printf("Battery directly reading percent: %3d%%\n", Battery::read_percent());

  Serial.println("");



  delay(1000);
}
