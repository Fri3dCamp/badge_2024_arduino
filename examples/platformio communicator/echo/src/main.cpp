#include <Arduino.h>
#include <driver/i2s.h>

//pin numbers for the 2024 badge, including communicator:
#include "pins2024.h"

//defines for this specific project:
#include "echo.h"

int32_t buffer1[2*SAMPLE_BUFFER_SIZE] = {0};
//uint16_t buffer2[SAMPLE_BUFFER_SIZE] = {0};

void setup() {
  Serial.begin(115200);
  LEDsetup();
}

void loop() {
  //Serial.println("recording ...");
  showRed();
  record(buffer1);
  //Serial.println("playing ...");
  showGreen();
  playback(buffer1);
}
