#include <Arduino.h>
//I'm cheating and asking for 1 more line so I can draw the next line off-screen
#define DWIDTH  297
#define DHEIGHT 240
#define COLOR_DEPTH 16
#include <TFT_eSPI.h>
#include "pins2024.h"


TFT_eSPI tft = TFT_eSPI();            // Create TFT object
TFT_eSprite txt = TFT_eSprite(&tft);  // Create Sprite object to render text small scale
uint16_t txtWidth;                    // textwidth will be calculated when choosing the text
uint16_t txtHeight = 10;              // set textHeight according to font used
uint16_t scale = 20;                  // by how much to scale the text sprite (bitshift)

uint16_t InitScrollText(const String &text);
void drawVerticalLine(uint16_t lineX, uint16_t txtX, TFT_eSprite &sprite);

void setup() {
  Serial.begin(115200);
  tft.setRotation(4); //setRotation also sets some flags inside the AdaFruit drivers. After that, we manually set MADCTL
  tft.init(TFT_BLACK);
  tft.writecommand(TFT_MADCTL);
  tft.writedata(TFT_MAD_BGR | TFT_MAD_MV); //exchange red and blue bytes, and mirror x-coordinates
  tft.fillScreen(TFT_BLACK);
  
  //the following sets up "vertical" scrolling parameters
  //only on the 2024 badge display, it actually scrolls horizontally
  //the command 0x33 can be found in the GC9307 datasheet
  tft.writecommand(0x33); //set vertical scroll address
  tft.writedata(0); //high byte
  tft.writedata(0); //no "stable" non-scrolling portion at top
  tft.writedata(DWIDTH >> 8); //high byte
  tft.writedata(DWIDTH & 0xFF);//240 lines in our display; all scrolling. 240 < 255 so fits in a single byte
  tft.writedata(0); //high byte
  tft.writedata(320 -DWIDTH); //chip has memory for 320 lines, but my display only has 240, so keep remaining lines non-scrolling

  InitScrollText("   *** Ask me about Fri3d Camp! ***   ");
}

uint16_t scrollpos = DWIDTH;
uint16_t txtpos = 0;
void loop() {

  //the only line that is off-screen is the one at scrollpos-1 (which wraps around)
  drawVerticalLine((scrollpos-1)%DWIDTH, txtpos/scale, txt);
  scrollpos++;
  if (scrollpos > DWIDTH) {
    Serial.println("Reset scroll");
    scrollpos = 0;
  }

  txtpos++;
  if (txtpos >= txtWidth*scale) {
    txtpos = 0;
  }


  tft.writecommand(0x37); //set vertical scroll start address
  tft.writedata(scrollpos >> 8); //high byte
  tft.writedata(scrollpos & 0xFF); //low byte
  delay(1);
}

uint16_t InitScrollText(const String &text) {
  txt.setTextFont(1); //choose a font with height smaller than txtHeight
  txt.setTextSize(1); //no use in scaling here.
  txtWidth = txt.textWidth(text);
  txt.createSprite(txtWidth, txtHeight);
  txt.setTextColor(TFT_CYAN, TFT_BLACK); //choose your color here. It will be copied in the big version
  txt.setCursor(0, 2); // y coordinate should be chosen such that font fits in txtHeight
  txt.fillSprite(TFT_BLACK);
  txt.setTextWrap(false);
  txt.print(text); //render text once. We will read pixels from this sprite later in drawVerticalLine()
  return txt.width();
}

void drawVerticalLine(uint16_t lineX, uint16_t txtX, TFT_eSprite &sprite) {
  tft.setAddrWindow(lineX, 10, 1, txtHeight*scale);
  for (uint16_t y = 0; y < txtHeight; y++) {
    uint16_t txtY = y; //lerp to smaller size
    uint16_t color = sprite.readPixel(txtX, txtY);
    for (int i = 0; i < scale; i++) tft.pushColor(color);
  }
}