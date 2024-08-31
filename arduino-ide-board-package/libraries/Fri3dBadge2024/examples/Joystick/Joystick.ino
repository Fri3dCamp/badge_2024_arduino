#include <Arduino.h>
#include "Fri3dBadge_pins.h"

#include <TFT_eSPI.h> // Library Dependency: `TFT_eSPI` by Bodmer v2.5.43
TFT_eSPI    tft = TFT_eSPI();

#define PIN_JOY_X 1
#define PIN_JOY_Y 3

void setup()
{
  Serial.begin(115200);
  tft.init(TFT_BLACK);
  tft.initDMA();
  tft.writecommand(TFT_MADCTL);
  tft.writedata(TFT_MAD_BGR | TFT_MAD_MV); //exchange red and blue bytes, and mirror x-coordinates
  tft.fillScreen(TFT_BLACK);
}


void loop()
{
  static int32_t prevx = tft.width() / 2;
  static int32_t prevy = tft.height() / 2;

  //erase circle at previous position:
  tft.drawWideLine(tft.width()/2, tft.height()/2, prevx, prevy, 5, TFT_BLACK, TFT_BLACK);
  tft.fillCircle(prevx, prevy, 20, TFT_BLACK);

  int newx = (analogRead(PIN_JOY_X)) * tft.width() / 4096;
  int newy = tft.height() - (analogRead(PIN_JOY_Y)) * tft.height() / 4096;
  tft.drawWideLine(tft.width()/2, tft.height()/2, newx, newy, 5, TFT_DARKGREY, TFT_BLACK);
  tft.fillCircle(newx, newy, 20, TFT_RED);
  delay(10);

  prevx = newx;
  prevy = newy;
}

