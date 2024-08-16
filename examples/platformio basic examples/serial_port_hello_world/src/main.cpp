#include <Arduino.h>
#include "pins_arduino.h"

void setup()
{
  Serial.begin(115200);
  delay(3000);
  Serial.println("Hello world");
}

void loop()
{
  Serial.println("Hello world again");
  delay(1000);
}
