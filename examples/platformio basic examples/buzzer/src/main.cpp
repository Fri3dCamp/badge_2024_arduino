#include <Arduino.h>

#define SPEAKER_PIN 46

void ArduinoCuckoo()
{
  //this is the simples way to generate a tone.
  //You only need Arduino.h so it works on all Arduinos (not just ESP32):
  tone(SPEAKER_PIN, 1400);
  delay(100);
  noTone(SPEAKER_PIN);
  delay(100);
  tone(SPEAKER_PIN, 1000);
  delay(150);
  noTone(SPEAKER_PIN);
  delay(2000);
}

void setup() {
  pinMode(SPEAKER_PIN, OUTPUT);
}

void loop() {
  ArduinoCuckoo();
  delay(1000);
}
