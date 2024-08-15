#pragma once

// library interface description
class Fri3dBadge_Buzzer {
public:
  Fri3dBadge_Buzzer();
  void tone( int frequency, int duration=0 );
  void setVolume( int _volume );

  void noTone();

  private:
    int volume;
};

