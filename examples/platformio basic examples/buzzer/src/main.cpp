#include <Arduino.h>

#define SPEAKER_PIN 46
const int CHANNEL = 0;

void ArduinoCuckoo();
void ESP32WolfWhistle();

void setup() {
}

void loop() {
  //ArduinoCuckoo();
  //delay(random(1000, 30000));

  ESP32WolfWhistle();
  delay(random(1000, 30000));
}

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

//this only works for ESP32 in Arduino-compatible mode:
void ESP32WolfWhistle()
{
  ledcSetup( CHANNEL, 3000, 8 );
  ledcWrite( CHANNEL, 0 ); //volume on channel 0 to 0
  ledcAttachPin(SPEAKER_PIN, CHANNEL); //link speaker pin to channel 0

  ledcWrite(CHANNEL, 50); //volume 50%
  for (int f=500; f<1400; f+=50) {
    ledcWriteTone(CHANNEL, f);
    delay(10);
  }
  
  ledcWrite(CHANNEL, 0); //silence
  delay(300);

  ledcWrite(CHANNEL, 50); //volume 50%
  for (int f=500; f<1400; f+=50) {
    ledcWriteTone(CHANNEL, f);
    delay(10);
  }
  for (int f=1400; f>500; f-=50) {
    ledcWriteTone(CHANNEL, f);
    delay(10);
  }

  ledcWrite(CHANNEL, 0); //silence
}
