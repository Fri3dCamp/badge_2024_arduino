#include <Arduino.h>
// Define the width and height according to the TFT and the
// available memory. The sprites will require:
//     DWIDTH * DHEIGHT * 2 bytes of RAM
// Note: for a 240 * 320 area this is 150 Kbytes!
#define DWIDTH  296
#define DHEIGHT 240
//#define USE_DMA_TO_TFT
// Color depth has to be 16 bits if DMA is used to render image
#define COLOR_DEPTH 16

#include <TFT_eSPI.h>
TFT_eSPI    tft = TFT_eSPI();

#include "battery_monitor.h"

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
