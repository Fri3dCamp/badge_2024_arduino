#include <Arduino.h>

#include <FastLED.h>

#include "Fri3dBadge_Blaster24Data.h"

#define NEOPIXEL_PIN 12
#define NUM_LEDS 5
#define RGB_ORDER GRB
#define FRAMES_PER_SECOND 120

CRGBArray<NUM_LEDS> leds;


void handle_ir_packet(IrDataPacket packet);


void setup()
{
  Serial.begin(115200);
  delay(3000);

  Serial.println("starting.....");

  FastLED.addLeds<WS2812, NEOPIXEL_PIN, RGB_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.clear(true);

  Data.init();
  Serial.println("listening on ir for blaster shot");

}

void loop()
{
  // light effect when receiving blaster shot
  handle_ir_packet(Data.readIr());

  delay(10);
}


void handle_ir_packet(IrDataPacket packet)
{
    // Serial.printf("handle_ir_packet packet: %u, p.raw: %" PRIu32 ", p.channel: %" PRIu8 ", p.team: %" PRIu8 ", p.action: %" PRIu8 "\n", 
    // packet, packet.get_raw(), packet.get_channel(), packet.get_team(), packet.get_action());
    
    if (packet.get_raw() != 0 && packet.get_action() == eActionDamage)
    {
        CRGB color = CRGB::Black;

        switch (packet.get_team())
        {
        case eTeamRex:
            color = CRGB::Red;
            Serial.println("Team Rex.");
            break;

        case eTeamBuzz:
            color = CRGB::Blue;
            Serial.println("Team Buzz.");
            break;

        case eTeamGiggle:
            color = CRGB::Green;
            Serial.println("Team Giggle.");
            break;

        case eTeamYellow:
            color = CRGB::Yellow;
            Serial.println("Team Yellow.");
            break;

        case eTeamMagenta:
            color = CRGB::Magenta;
            Serial.println("Team Magenta.");
            break;

        case eTeamCyan:
            color = CRGB::Cyan;
            Serial.println("Team Cyan.");
            break;

        case eTeamWhite:
            color = CRGB::White;
            Serial.println("Team White.");
            break;

        default:
            color = CRGB::Black;
            break;
        }

        leds = color;
        FastLED.show();
        delay(100);

        while (leds[0].r != 0 || leds[0].g != 0 || leds[0].b != 0)
        {
            delay(1000/FRAMES_PER_SECOND);
            leds.fadeToBlackBy(7);
            FastLED.show();
        }
        delay(100);
        Data.readIr(); // clear buffer
        FastLED.clear(true);
    }
}
