#include <Arduino.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>

#include "Adafruit_ST7789_Fri3d2024.h" // Hardware-specific library for GC9307 (ST7789) driver of Fri3d badge 2024

// Explicitly use hardware SPI port, default is software which is too slow
SPIClass *spi = new SPIClass(HSPI);
Adafruit_GFX_Fri3dBadge2024_TFT tft(spi, TFT_CS, TFT_DC, TFT_RST);

void setup(void) {
  Serial.begin(115200);
  delay(3000); 
  Serial.println("Hello world - Adafruit GFX GC9307/ST7789 Fri3d badge");

  spi->begin(TFT_SCLK, TFT_MISO, TFT_MOSI, TFT_CS);
  tft.setSPISpeed(SPI_FREQUENCY);

  tft.init(TFT_WIDTH, TFT_HEIGHT);
  tft.setRotation( 3 );

  // Anything from the Adafruit GFX library can go here, see
  // https://learn.adafruit.com/adafruit-gfx-graphics-library
  
  tft.fillScreen(TFT_BLACK);
  tft.drawRect(0, 0, tft.width(), tft.height(), TFT_BLUE);
  tft.setCursor(10, tft.height()/2-10);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(3);
  tft.println("Hello world");

  delay(2000);
}

void loop() {
}
