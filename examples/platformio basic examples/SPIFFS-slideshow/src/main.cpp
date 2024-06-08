#include <Arduino.h>
#include "SPIFFS.h"

#define DWIDTH  296
#define DHEIGHT 240
#define USE_DMA_TO_TFT
#define USE_DMA
// Color depth has to be 16 bits if DMA is used to render image
#define COLOR_DEPTH 16
#include <TFT_eSPI.h>
TFT_eSPI    tft = TFT_eSPI();

//by declaring buffer at global scope, I seem to be able to make it bigger:
//https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/memory-types.html
DMA_ATTR uint8_t buffer[2*DWIDTH*DHEIGHT]; //enough for full screen of pixels

void loadBitMap(String name);
File root;

void setup() {
  Serial.begin(115200);
  tft.init(TFT_BLACK);
  tft.initDMA();
  //since windows bitmaps are stored bottom-up, I'm rotating the display to "1" instead of "3"
  tft.setRotation(1);
  //tft.writecommand(TFT_MADCTL);
  //tft.writedata(TFT_MAD_BGR | TFT_MAD_MV); //exchange red and blue bytes, and mirror x-coordinates
  tft.fillScreen(TFT_BLACK);
  SPIFFS.begin();
  root = SPIFFS.open("/");
  tft.startWrite(); //startWrite() is only necessary if you use DMA transfer
}

void loop() {
  bool isDir = true;
  String fname = root.getNextFileName(&isDir);

  if (fname != NULL && !isDir) {
    //Serial.println(fname);
    loadBitMap(fname);
    delay(2000);
  }
  else {
    //Serial.println("This is a very naive way of restarting the slide show")
    root.close();
    root = SPIFFS.open("/");
  }

}

void loadBitMap(String name)
{
  File bmp = SPIFFS.open(name);

  //BMP header is 14 bytes
  //first, read & ignore the first 10 bytes of BMP header:
  bmp.seek(10);
  //last 4 bytes of the bmp header contain offset for pixel array:
  uint32_t offset = 0;
  bmp.readBytes((char *)&offset, 4);

  //Serial.print("offset for pixel array = ");
  //Serial.println(offset);
  //so, ignore everything until offset:
  bmp.seek(offset);
  bmp.read(buffer, 2*DWIDTH*DHEIGHT);
  bmp.close();

  tft.setSwapBytes(true); //little-endian vs big-endian
  tft.pushImageDMA(0, 0, DWIDTH, DHEIGHT, (uint16_t*) buffer);
  //tft.endWrite(); //will block as long as DMA transfer is not ready



}
