#include <Arduino.h>

#include "blaster_link.hpp"

static const uint8_t blaster_link_pin = 10;
BlasterLink blasterLink = BlasterLink(blaster_link_pin);


void setup() {
  Serial.begin(115200);
  delay(3000);
 
  blasterLink.start_listen();

  // set the blaster to silent (mute=true)
  bool success = blasterLink.set_settings(true);
  if (!success) log_e("failed setting blaster silent, present?");
}

static uint8_t team = 0;

static unsigned long last_millis = millis();
static const unsigned long change_team_interval_millis = 1000 * 500;

void loop() {
  // call this regulary
  blasterLink.process_buffer();

  // did we receive somthing from the blaster?
  while (blasterLink.message_available()) {
    DataPacket message = blasterLink.pop_message();
    char s[120];
    message.print(s, 120);
    log_d("-Blaster_link: %s", s);
  }

  // ready to change team
  if (millis() - last_millis > change_team_interval_millis) {
    blasterLink.set_team(TeamColor(team));
    team++;
    if (team >= 7) team = 0; // only 8 TeamColor defined
    
    last_millis = millis();
  }

  // sleep a little bit, so that other tasks can do stuff
  delay(10);
}

