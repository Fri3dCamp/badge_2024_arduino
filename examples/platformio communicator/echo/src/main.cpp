#include <Arduino.h>
#include <driver/i2s.h>

//pin numbers for the 2024 badge, including communicator:
#include "pins2024.h"

//defines for this specific project:
#include "echo.h"

int8_t buffer1[2*SAMPLE_BUFFER_SIZE] = {0};
//uint16_t buffer2[SAMPLE_BUFFER_SIZE] = {0};

bool isButtonPressed() {
  return digitalRead(PIN_A) == LOW;
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_A, INPUT_PULLUP);
  LEDsetup();
}

void loop() {
  showBlue();
  Serial.println("waiting for PIN_A ...");
  while (!isButtonPressed()) {
    delay(1); //give schedular a chance to interrupt busy loop
  }

  showRed();
  size_t recordedBytes = record(buffer1, isButtonPressed);

  //Serial.println("playing ...");
  showGreen();
  playback(buffer1, recordedBytes);
}
