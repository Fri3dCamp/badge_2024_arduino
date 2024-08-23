// Following libraries need to be installed next to the Fri3d board:
// - TFT_eSPI e.g. 2.5.43

#include <Arduino.h>
#include <TFT_eSPI.h>  // Library Dependency: `TFT_eSPI` by Bodmer v2.5.43
#include <Wire.h>
#include <SPI.h>

#include "Fri3dBadge_pins.h"
#include "Fri3dBadge_Button.h"

enum {
  BUTTON_ID_A = 0,
  BUTTON_ID_B,
  BUTTON_ID_X,
  BUTTON_ID_Y,
  BUTTON_ID_MENU,
  BUTTON_ID_START,
  BUTTON_ID_UP,
  BUTTON_ID_DOWN,
  BUTTON_ID_LEFT,
  BUTTON_ID_RIGHT,

  NUM_BUTTONS,
};

TFT_eSPI tft = TFT_eSPI();

Fri3d_Button *buttons[NUM_BUTTONS] = {
  new Fri3d_Button(FRI3D_BUTTON_TYPE_DIGITAL, PIN_A, 25, INPUT_PULLUP, true),     // BUTTON_ID_A
  new Fri3d_Button(FRI3D_BUTTON_TYPE_DIGITAL, PIN_B, 25, INPUT_PULLUP, true),     // BUTTON_ID_B
  new Fri3d_Button(FRI3D_BUTTON_TYPE_DIGITAL, PIN_X, 25, INPUT_PULLUP, true),     // BUTTON_ID_X
  new Fri3d_Button(FRI3D_BUTTON_TYPE_DIGITAL, PIN_Y, 25, INPUT_PULLUP, true),     // BUTTON_ID_Y
  new Fri3d_Button(FRI3D_BUTTON_TYPE_DIGITAL, PIN_MENU, 25, INPUT_PULLUP, true),  // BUTTON_ID_MENU
  new Fri3d_Button(FRI3D_BUTTON_TYPE_DIGITAL, PIN_START, 25, INPUT, true),        // BUTTON_ID_START, GPIO0 has fixed hardware pullup
  new Fri3d_Button(FRI3D_BUTTON_TYPE_ANALOGUE_HIGH, PIN_JOY_Y, 0, INPUT, false),  // BUTTON_ID_UP
  new Fri3d_Button(FRI3D_BUTTON_TYPE_ANALOGUE_LOW, PIN_JOY_Y, 0, INPUT, false),   // BUTTON_ID_DOWN
  new Fri3d_Button(FRI3D_BUTTON_TYPE_ANALOGUE_LOW, PIN_JOY_X, 0, INPUT, false),   // BUTTON_ID_LEFT
  new Fri3d_Button(FRI3D_BUTTON_TYPE_ANALOGUE_HIGH, PIN_JOY_X, 0, INPUT, false),  // BUTTON_ID_RIGHT
};

void buttonshow(int buttonid) {
  Fri3d_Button *b = buttons[buttonid];
  int x = 0;
  int y = 0;
  int w = 0;
  uint32_t col;
  const char *buttonname = "";

  switch (buttonid) {
    case BUTTON_ID_MENU:
      x = 40;
      y = 58;
      w = 70;
      buttonname = "MENU";
      break;

    case BUTTON_ID_START:
      x = 40;
      y = 80;
      w = 70;
      buttonname = "START";
      break;

    case BUTTON_ID_X:
      x = 200;
      y = 58;
      w = 30;
      buttonname = "X";
      break;

    case BUTTON_ID_Y:
      x = 160;
      y = 80;
      w = 30;
      buttonname = "Y";
      break;

    case BUTTON_ID_A:
      x = 240;
      y = 80;
      w = 30;
      buttonname = "A";
      break;

    case BUTTON_ID_B:
      x = 200;
      y = 92;
      w = 30;
      buttonname = "B";
      break;

    case BUTTON_ID_UP:
      x = 100;
      y = 120;
      w = 70;
      buttonname = "UP";
      break;

    case BUTTON_ID_DOWN:
      x = 100;
      y = 160;
      w = 70;
      buttonname = "DOWN";
      break;

    case BUTTON_ID_LEFT:
      x = 40;
      y = 140;
      w = 70;
      buttonname = "LEFT";
      break;

    case BUTTON_ID_RIGHT:
      x = 160;
      y = 140;
      w = 70;
      buttonname = "RIGHT";
      break;
  }
  Serial.print(buttonname);
  if (b->isPressed()) {
    Serial.println(" pressed");
    col = TFT_GREEN;
  } else {
    Serial.println(" released");
    col = TFT_BLACK;
  }
  if (x > 0) {
    tft.fillRoundRect(x, y, w, 18, 2, col);
    tft.drawRoundRect(x, y, w, 18, 2, TFT_WHITE);
    tft.setCursor(x + 5, y + 2);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.println(buttonname);
  }
}

void button_check(int buttonid) {
  Fri3d_Button *b = buttons[buttonid];
  if (b->wasPressed() || b->wasReleased()) {
    buttonshow(buttonid);
  }
}

void setup() {
  Serial.begin(115200);

  // delay(5000);
  // Serial.println("Fri3d badge hardware test");
  // Serial.print("My IDF Version is: ");
  // Serial.println(esp_get_idf_version());

  tft.init();
  tft.writecommand(TFT_MADCTL);
  tft.writedata(TFT_MAD_BGR | TFT_MAD_MV);  //exchange red and blue bytes, and mirror x-coordinates
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(tft.width() / 2 - 90, 20);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.println("Fri3d Badge 2024");

  for (int count = 0; count < NUM_BUTTONS; ++count) {
    buttons[count]->begin();
    buttonshow(count);
  }
}

void loop() {
  for (int count = 0; count < NUM_BUTTONS; ++count) {
    buttons[count]->read();
    button_check(count);
  }
}
