/*
   Spawns pictures of cats on the TFT with rainbow LEDs
   Press start to start spawning cats, or use ABXY to manually spawn
*/

#include "Fri3dBadge_pins.h"
#include "Fri3dBadge_Button.h"
#include <TFT_eSPI.h>
#include <FastLED.h>
#include <PNGdec.h>

#include "cat_images.h"

enum {
  BUTTON_ID_A = 0,
  BUTTON_ID_B,
  BUTTON_ID_X,
  BUTTON_ID_Y,
  BUTTON_ID_MENU,
  BUTTON_ID_START,

  NUM_BUTTONS,
};

#define NUM_BUTTONS 6
#define NEOPIXEL_PIN 12
#define NUMLEDPIXELS 5
#define RGB_ORDER GRB

#define CAT_SIZE 72

#define MAX_IMAGE_WIDTH 296 // TFT width

PNG png; // PNG decoder

TFT_eSPI tft = TFT_eSPI();

CRGB leds[NUMLEDPIXELS];

bool autokitty = false;
bool help_drawn = false;

Fri3d_Button *buttons[NUM_BUTTONS] = {
  new Fri3d_Button (FRI3D_BUTTON_TYPE_DIGITAL, PIN_A,    25, INPUT_PULLUP, true), // BUTTON_ID_A
  new Fri3d_Button (FRI3D_BUTTON_TYPE_DIGITAL, PIN_B,    25, INPUT_PULLUP, true), // BUTTON_ID_B
  new Fri3d_Button (FRI3D_BUTTON_TYPE_DIGITAL, PIN_X,    25, INPUT_PULLUP, true), // BUTTON_ID_X
  new Fri3d_Button (FRI3D_BUTTON_TYPE_DIGITAL, PIN_Y,    25, INPUT_PULLUP, true), // BUTTON_ID_Y
  new Fri3d_Button (FRI3D_BUTTON_TYPE_DIGITAL, PIN_MENU, 25, INPUT_PULLUP, true), // BUTTON_ID_MENU
  new Fri3d_Button (FRI3D_BUTTON_TYPE_DIGITAL, PIN_START, 25, INPUT,       true), // BUTTON_ID_START, GPIO0 has fixed hardware pullup
};

void buttons_init()
{
  for (int count = 0; count < NUM_BUTTONS; ++count)
  {
    buttons[count]->begin();
  }
}

void buttons_update()
{
  for (int count = 0; count < NUM_BUTTONS; ++count)
  {
    buttons[count]->read();
  }
}

void rainbow_wave(uint8_t thisSpeed, uint8_t deltaHue) {
  uint8_t thisHue = beatsin8(thisSpeed, 0, 255);

  fill_rainbow(leds, NUMLEDPIXELS, thisHue, deltaHue);
}


void draw_help() {
  help_drawn = true;
  
  draw_centered_text("Cat Spawner", 20, 15, 4);

  draw_centered_text("A, B, X, Y to spawn cat", 5, 50, 2);

  draw_centered_text("Start to auto-spawn cats", 5, 70, 2);

  draw_centered_text("Menu to clear the screen", 5, 90, 2);
}

void draw_centered_text(String text, uint8_t x_pos, uint8_t y_pos,uint8_t size) {
  //uint8_t text_width = tft.textWidth(text, size * 2);

  // Drop shadow
  tft.setCursor(x_pos + 2, y_pos + 2);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(size);
  tft.println(text);

  // Main text
  tft.setCursor(x_pos, y_pos);
  tft.setTextColor(TFT_MAGENTA);
  tft.setTextSize(size);
  tft.println(text);
}

int16_t xpos = 0;
int16_t ypos = 0;

void png_draw(PNGDRAW *pDraw) {

  uint16_t lineBuffer[MAX_IMAGE_WIDTH];          // Line buffer for rendering
  uint8_t  maskBuffer[1 + MAX_IMAGE_WIDTH / 8];  // Mask buffer

  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);

  if (png.getAlphaMask(pDraw, maskBuffer, 255)) {
    // Note: pushMaskedImage is for pushing to the TFT and will not work pushing into a sprite
    tft.pushMaskedImage(xpos, ypos + pDraw->y, pDraw->iWidth, 1, lineBuffer, maskBuffer);
  }
}

void draw_kitty(uint8_t cat_number) {
  if (help_drawn) {
    help_drawn = !help_drawn;
    tft.fillScreen(TFT_BLACK);
  }

  int16_t rc = 0;

  xpos = random(0, tft.width()) - (CAT_SIZE / 2);
  ypos = random(0, tft.height()) - (CAT_SIZE / 2);

  if (cat_number == 1) {
    rc = png.openFLASH((uint8_t *)cat1, sizeof(cat1), png_draw);
  } else if (cat_number == 2) {
    rc = png.openFLASH((uint8_t *)cat2, sizeof(cat2), png_draw);
  } else if (cat_number == 3) {
    rc = png.openFLASH((uint8_t *)cat3, sizeof(cat3), png_draw);
  }

  if (rc == PNG_SUCCESS) {
    // Serial.println("Successfully opened png file");
    // Serial.printf("image specs: (%d x %d), %d bpp, pixel type: %d\n", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
    tft.startWrite();
    png.decode(NULL, 0);
    tft.endWrite();
    // png.close(); //p not needed for memory->memory decode
  }
}

void button_pressed(int buttonid)
{
  Fri3d_Button *b = buttons[buttonid];

  switch (buttonid)
  {
    case BUTTON_ID_START:
      autokitty = !autokitty;
      break;

    case BUTTON_ID_X:
      draw_kitty(3);
      break;

    case BUTTON_ID_Y:
      draw_kitty(1);
      break;

    case BUTTON_ID_A:
      draw_kitty(2);
      break;

    case BUTTON_ID_B:
      draw_random_kitty();
      break;

    case BUTTON_ID_MENU:
      tft.fillScreen(TFT_BLACK);
      draw_help();
      break;
  }
}

void button_check(int buttonid)
{
  Fri3d_Button *b = buttons[buttonid];
  if (b->wasPressed())
  {
    button_pressed(buttonid);
  }
}

void button_loop()
{
  button_check(BUTTON_ID_START);
  button_check(BUTTON_ID_MENU);
  button_check(BUTTON_ID_A);
  button_check(BUTTON_ID_B);
  button_check(BUTTON_ID_X);
  button_check(BUTTON_ID_Y);
}

void draw_random_kitty() {
  uint8_t cat_number = random(1, 5);
  if (cat_number >= 4) {
    cat_number = 3;
  } else if (cat_number == 3) {
    cat_number = 2;
  } else {
    cat_number = 1;
  }

  draw_kitty(cat_number);
}

void setup()
{
  buttons_init();

  tft.init();
  tft.writecommand(TFT_MADCTL);
  tft.writedata(TFT_MAD_BGR | TFT_MAD_MV); //exchange red and blue bytes, and mirror x-coordinates
  tft.fillScreen(TFT_BLACK);

  FastLED.addLeds<WS2812, NEOPIXEL_PIN, RGB_ORDER>(leds, NUMLEDPIXELS);
  FastLED.setBrightness(128);

  draw_help();
}

int16_t loop_number = 0;

void loop()
{
  buttons_update();
  button_loop();

  loop_number++;

  if (loop_number % 100 == 0) {
    loop_number = 0;

    if (autokitty) {
      draw_random_kitty();
    }
  }

  rainbow_wave(5, 30);
  FastLED.show();
}
