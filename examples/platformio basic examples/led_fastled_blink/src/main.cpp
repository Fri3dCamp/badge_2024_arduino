/* 
 * This example demonstrates how to blink the leds (ws2812) using the FastLED library
 * This library can be found in the Arduino IDE or on https://github.com/FastLED/FastLED
 * The version of the library should be minimal 3.5.0 !
 */

#include <FastLED.h>

#define NEOPIXEL_PIN 12
#define NUMLEDPIXELS 5
#define RGB_ORDER GRB

CRGB leds[NUMLEDPIXELS];

void setup() {
  FastLED.addLeds<WS2812, NEOPIXEL_PIN, RGB_ORDER>(leds, NUMLEDPIXELS);  
}

void loop() {
  int r = 25;
  int g = 0;
  int b = 0;

  for (int i = 0; i < NUMLEDPIXELS; i++) {
    leds[i] = CRGB(r, g, b);
  }

  FastLED.show();
  delay(500);

  // Now turn the LED off, then pause
  for (int i = 0; i < NUMLEDPIXELS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  delay(500);
}
