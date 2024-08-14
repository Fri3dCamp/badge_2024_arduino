#include "Fri3dBadge_Joystick.h"
#include "Fri3dBadge_pins.h"

Fri3dBadge_Joystick::Fri3dBadge_Joystick()
{
    pinMode(PIN_JOY_X,INPUT);
    pinMode(PIN_JOY_Y,INPUT);
    centerx = analogRead(PIN_JOY_X)-2048;
    centery = analogRead(PIN_JOY_Y)-2048;
}

void 
Fri3dBadge_Joystick::getXY(int *x,int *y) {
  *x = analogRead(PIN_JOY_X)-2048;
  *y = analogRead(PIN_JOY_Y)-2048;
  if (abs(*x-centerx)<10) { *x=0; }
  if (abs(*y-centery)<10) { *y=0; }
}

