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

void setup() {
  tft.init(TFT_BLACK);
  tft.initDMA();
  //setRotation is no longer necessary, as long as you define TFT_WIDTH and TFT_HEIGHT in platformio.ini
  //tft.setRotation(3);
  tft.writecommand(TFT_MADCTL);
  tft.writedata(TFT_MAD_BGR | TFT_MAD_MV); //exchange red and blue bytes, and mirror x-coordinates
  //tft.fillScreen(TFT_BLACK);
}


uint32_t colors[] = {
  TFT_BLACK,TFT_NAVY,TFT_DARKGREEN,TFT_DARKCYAN,TFT_MAROON,TFT_PURPLE,TFT_OLIVE,
  TFT_LIGHTGREY,TFT_DARKGREY,TFT_BLUE,TFT_GREEN,TFT_CYAN,TFT_RED ,TFT_MAGENTA,
  TFT_YELLOW,TFT_WHITE,TFT_ORANGE,TFT_GREENYELLOW,TFT_PINK,TFT_BROWN,TFT_GOLD,
  TFT_SILVER,TFT_SKYBLUE,TFT_VIOLET
};

String colornames[] = {
  "BLACK","NAVY","DARKGREEN","DARKCYAN","MAROON","PURPLE","OLIVE",
  "LIGHTGREY","DARKGREY","BLUE","GREEN","CYAN","RED ","MAGENTA",
  "YELLOW","WHITE","ORANGE","GREENYELLOW","PINK","BROWN","GOLD",
  "SILVER","SKYBLUE","VIOLET"
};

int c=0;

void loop() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(colors[c]);
  tft.setCursor(40,20); //stay out of rounded corner
  tft.setTextSize(4);
  tft.println(colornames[c]);

  delay(2000);
  c = (c+1) % 24; //24 different colors defined in array
}

