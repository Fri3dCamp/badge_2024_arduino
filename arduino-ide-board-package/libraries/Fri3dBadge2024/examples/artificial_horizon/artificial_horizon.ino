#include <Arduino.h>
#include "Fri3dBadge_pins.h"
#include <Wire.h>

#include "Fri3dBadge_WSEN_ISDS.h"

Sensor_ISDS sensor;
int status;
int16_t acc_X, acc_Y, acc_Z;
#define TOLERANCE 20

// Color depth has to be 16 bits if DMA is used to render image
#define COLOR_DEPTH 16
#include <TFT_eSPI.h> // Library Dependency: `TFT_eSPI` by Bodmer v2.5.43
TFT_eSPI    tft = TFT_eSPI();

void horizon(bool erase=false);
double dotprod(int16_t x1, int16_t  y1, int16_t x2, int16_t y2);
void cross(uint32_t color);

void setup() {
  tft.init(TFT_BLACK);
  //workaround 1/2: call "setRotation" to fix width and height dimensions
  //this workaround is not necessary if you define TFT_HEIGHT and TFT_WIDTH in platformio.ini
  //tft.setRotation(1);
  //workaround 2/2: command MADCTL ("memory address control") fixes orientation and mirroring
  tft.writecommand(TFT_MADCTL);
  tft.writedata(TFT_MAD_BGR | TFT_MAD_MV); //exchange red and blue bytes, and mirror x-coordinates
  tft.setTextSize(3);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.fillScreen(TFT_BLACK);

  //Serial.begin(115200);
  Wire.setPins(PIN_I2C_SDA, PIN_I2C_SCL);
  // Initialize the I2C interface
  sensor.init(ISDS_ADDRESS_I2C_1);
  sensor.SW_RESET();
  sensor.select_ODR(2);
  sensor.set_Mode(2);
}

void loop() {
  //use previous values to erase last lines:
  if (status == WE_FAIL) {
    cross(TFT_BLACK);
  }
  else {
    horizon(true); //true indicates erase line
  }

  status = sensor.get_accelerations(&acc_X,&acc_Y,&acc_Z);
  if (status == WE_FAIL) {
    cross(TFT_RED);
  }
  else {
    horizon();
  }
  delay(20);
}

void cross(uint32_t color)
{
  tft.drawWideLine(0, 0, tft.width(), tft.height(), 5, color, TFT_BLACK);
  tft.drawWideLine(0, tft.height(), tft.width(), 0, 5, color, TFT_BLACK);
}

//if erase==true, we will draw line in black to erase it
void horizon(bool erase)
{
  uint32_t defcolor = (erase? TFT_BLACK: TFT_RED);
  if (!erase && abs(acc_Y) < TOLERANCE) defcolor = TFT_GREEN;

  //assumption: acc(X,Y,Z) is a vector that points down (to gravity)
  //we then calculate angle with the vector that we want to see, i.e. perfect down, (-1000, 0, 0)
  double mag_vector = sqrt((double)acc_X*acc_X + (double)acc_Y*acc_Y);
  double cosangle = dotprod(-1000, 0, acc_X, acc_Y) / 1000.0 / mag_vector;
  double sign = abs(acc_Y) / acc_Y;
  double angle_radians = sign * (cosangle < 0? 2*PI - acos(-cosangle): acos(cosangle));
  //given the angle, calculate #pixels we need to deviate from middle to draw our horizon:
  //once the angle goes over 45, this doesn't work and we should actually calculate xdiff
  float ydiff = sin(angle_radians) * tft.width()/2.0;

  //Serial.printf("%5d, %5d, %5d,     ", acc_X, acc_Y, acc_Z);
  //Serial.printf("%7.2f, %7.2f, %4.3f, %4.0f", mag_vector, cosangle, angle, ydiff);
  //Serial.println();

  tft.setCursor(70, 50); //top middle of screen
  tft.printf(" %5.2f ", -180.0 / PI * angle_radians); //display angle, converted to degrees
  tft.drawWideLine(0, tft.height()/2+ydiff, tft.width(), tft.height()/2-ydiff, 5, defcolor, TFT_BLACK);
}

double dotprod(int16_t x1, int16_t  y1, int16_t x2, int16_t y2)
{
  double res = (double) x1 * x2 + (double) y1 * y2;
  //Serial.print(res); Serial.print(",\t");
  return res;
}
