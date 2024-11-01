#include <Arduino.h>
#include <FastLED.h>

#include "pins2024.h"
#include "echo.h"

CRGB leds[5];

void LEDsetup() {
  FastLED.addLeds<WS2812, PIN_LEDS, GRB>(leds, 5);
}

void showBlue() {
    leds[2] = CRGB(0, 0, 16);
    FastLED.show();
}

void showRed() {
    leds[2] = CRGB(64, 0, 0);
    FastLED.show();
}

void showGreen() {
    leds[2] = CRGB(0, 64, 0);
    FastLED.show();   
}