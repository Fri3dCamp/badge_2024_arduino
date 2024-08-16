#ifndef Fri3dBadge_Adafruit_ST7789_h
#define Fri3dBadge_Adafruit_ST7789_h

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#define ST77XX_MADCTL_BGR 0x08

class Fri3dBadge_Adafruit_ST7789 : public Adafruit_ST7789 {
public:
	Fri3dBadge_Adafruit_ST7789();

    void init();
private:

};

#endif