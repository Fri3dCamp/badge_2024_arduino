#ifndef _ADAFRUIT_ST7789_FRI3D2024_H_
#define _ADAFRUIT_ST7789_FRI3D2024_H_

#include "Adafruit_ST77xx.h"

/// Subclass of ST77XX type display for GC9307/ST7789 TFT Driver of Fri3d badge 2024 display
class Adafruit_GFX_Fri3dBadge2024_TFT : public Adafruit_ST77xx {
public:
  Adafruit_GFX_Fri3dBadge2024_TFT(int8_t cs, int8_t dc, int8_t mosi, int8_t sclk,
                  int8_t rst = -1);
  Adafruit_GFX_Fri3dBadge2024_TFT(int8_t cs, int8_t dc, int8_t rst);
#if !defined(ESP8266)
  Adafruit_GFX_Fri3dBadge2024_TFT(SPIClass *spiClass, int8_t cs, int8_t dc, int8_t rst);
#endif // end !ESP8266

  void setRotation(uint8_t m);
  void init(uint16_t width, uint16_t height, uint8_t spiMode = SPI_MODE0);

protected:
  uint8_t _colstart2 = 0, ///< Offset from the right
      _rowstart2 = 0;     ///< Offset from the bottom

private:
  uint16_t windowWidth;
  uint16_t windowHeight;
};

#define TFT_BLACK ST77XX_BLACK
#define TFT_WHITE ST77XX_WHITE
#define TFT_RED ST77XX_RED
#define TFT_GREEN ST77XX_GREEN
#define TFT_BLUE ST77XX_BLUE
#define TFT_CYAN ST77XX_CYAN
#define TFT_MAGENTA ST77XX_MAGENTA
#define TFT_YELLOW ST77XX_YELLOW
#define TFT_ORANGE ST77XX_ORANGE

#endif // _ADAFRUIT_ST7789H_
