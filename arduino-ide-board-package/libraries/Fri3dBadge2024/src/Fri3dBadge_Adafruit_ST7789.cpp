#include <Fri3dBadge_Adafruit_ST7789.h>

Fri3dBadge_Adafruit_ST7789::Fri3dBadge_Adafruit_ST7789() : Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST)
{

}

void Fri3dBadge_Adafruit_ST7789::init()
{
    // Call the parent class's begin method to initialize the display
    Adafruit_ST7789::init(TFT_HEIGHT, TFT_WIDTH);
      this->setSPISpeed(80000000);

      this->sendCommand(ST77XX_INVOFF); // Fix Black vs white

      Adafruit_ST7789::setColRowStart(0, 0); // Fix offset

      this->setRotation(1); // Set initial rotation

      uint8_t madctl = ST77XX_MADCTL_MV | ST77XX_MADCTL_BGR; // Fix Mirroring & Blue vs White
      this->sendCommand(ST77XX_MADCTL, &madctl, 1);
}
