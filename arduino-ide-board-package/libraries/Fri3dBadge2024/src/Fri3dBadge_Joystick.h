#pragma once

#include <Arduino.h>

#define JOYSTICK_MAXVALUE 2048

class Fri3dBadge_Joystick {
public:
  Fri3dBadge_Joystick();
  void getXY(int *x,int *y);

  private:
    int centerx;
    int centery;
};
