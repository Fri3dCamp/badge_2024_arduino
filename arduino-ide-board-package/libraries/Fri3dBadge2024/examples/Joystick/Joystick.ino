#include <Arduino.h>
#include "Fri3dBadge_pins.h"

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

#define PIN_JOY_X 1
#define PIN_JOY_Y 3

void setup()
{
  Serial.begin(115200);
  tft.init(TFT_BLACK);
  tft.initDMA();
  tft.setRotation(3);
  tft.writecommand(TFT_MADCTL);
  tft.writedata(TFT_MAD_BGR | TFT_MAD_MV); //exchange red and blue bytes, and mirror x-coordinates
  tft.fillScreen(TFT_BLACK);
}

int32_t prevx = DWIDTH / 2;
int32_t prevy = DHEIGHT / 2;

void loop()
{

  //erase circle at previous position:
  tft.drawWideLine(DWIDTH/2, DHEIGHT/2, prevx, prevy, 5, TFT_BLACK, TFT_BLACK);
  tft.fillCircle(prevx, prevy, 20, TFT_BLACK);

  int newx = (analogRead(PIN_JOY_X)) * DWIDTH / 4096;
  int newy = DHEIGHT - (analogRead(PIN_JOY_Y)) * DHEIGHT / 4096;
  tft.drawWideLine(DWIDTH/2, DHEIGHT/2, newx, newy, 5, TFT_DARKGREY, TFT_BLACK);
  tft.fillCircle(newx, newy, 20, TFT_RED);
  delay(10);

  prevx = newx;
  prevy = newy;
}

