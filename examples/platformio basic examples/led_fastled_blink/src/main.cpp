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