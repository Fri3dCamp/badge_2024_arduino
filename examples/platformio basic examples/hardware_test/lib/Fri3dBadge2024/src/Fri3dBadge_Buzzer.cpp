#include "Fri3dBadge_Buzzer.h"
#include "Arduino.h"

Fri3dBadge_Buzzer::Fri3dBadge_Buzzer() {
  volume = 255;
  ledcSetup( CHANNEL_BUZZER, 3000, 8 );
  ledcWrite( CHANNEL_BUZZER, 0 );
  ledcAttachPin(PIN_BUZZER, CHANNEL_BUZZER);
}

void 
Fri3dBadge_Buzzer::tone( int frequency, int duration ) {
  ledcWriteTone( CHANNEL_BUZZER, frequency );
  ledcWrite( CHANNEL_BUZZER, volume );
  delay(duration);
}

void 
Fri3dBadge_Buzzer::setVolume( int _volume ) {
  volume = _volume;
  ledcWrite( CHANNEL_BUZZER, volume );
}

void 
Fri3dBadge_Buzzer::noTone(  ) {
  ledcWrite( CHANNEL_BUZZER, 0 );
}
