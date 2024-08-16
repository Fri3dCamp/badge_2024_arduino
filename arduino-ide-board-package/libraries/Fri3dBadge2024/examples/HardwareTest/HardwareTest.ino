// Following libraries need to be installed next to the Fri3d board:
// - TFT_eSPI e.g. 2.5.43
// - FastLED e.g. 3.7.0
// - IRremoteESP8266 e.g. 2.8.6

#include <Arduino.h>
#include "Fri3dBadge_pins.h"
#include <TFT_eSPI.h>
#include <Wire.h>
#include <SPI.h>
#include <FastLED.h>
#include <IRrecv.h>
#include <IRac.h>
#include <SD.h>

#include "Fri3dBadge_Buzzer.h"
#include "Fri3dBadge_Joystick.h"
#include "Fri3dBadge_Button.h"
#include "Fri3dBadge_WSEN_ISDS.h"
#include "Fri3dBadge24_battery_monitor.h"

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

#define NUMLEDPIXELS 5
CRGB leds[NUMLEDPIXELS];

IRrecv irrecv(PIN_IR_RECEIVER, 1024, 50, true);
decode_results results;  // Somewhere to store the results

TFT_eSPI tft = TFT_eSPI();

TFT_eSprite joystick_sprite = TFT_eSprite(&tft);
#define JOYSTICK_SPRITE_WIDTH  80
#define JOYSTICK_SPRITE_HEIGHT 80

int status_sd = 0;

Sensor_ISDS imu;

TFT_eSprite horizon_sprite = TFT_eSprite(&tft);
#define HORIZON_SPRITE_WIDTH  80
#define HORIZON_SPRITE_HEIGHT 80

TFT_eSprite battery_sprite = TFT_eSprite(&tft);
#define BATTERY_SPRITE_WIDTH  50
#define BATTERY_SPRITE_HEIGHT 36

Fri3d_Button *buttons[NUM_BUTTONS] = {
  new Fri3d_Button (FRI3D_BUTTON_TYPE_DIGITAL, PIN_A,    25, INPUT_PULLUP, true), // BUTTON_ID_A
  new Fri3d_Button (FRI3D_BUTTON_TYPE_DIGITAL, PIN_B,    25, INPUT_PULLUP, true), // BUTTON_ID_B
  new Fri3d_Button (FRI3D_BUTTON_TYPE_DIGITAL, PIN_X,    25, INPUT_PULLUP, true), // BUTTON_ID_X
  new Fri3d_Button (FRI3D_BUTTON_TYPE_DIGITAL, PIN_Y,    25, INPUT_PULLUP, true), // BUTTON_ID_Y
  new Fri3d_Button (FRI3D_BUTTON_TYPE_DIGITAL, PIN_MENU, 25, INPUT_PULLUP, true), // BUTTON_ID_MENU
  new Fri3d_Button (FRI3D_BUTTON_TYPE_DIGITAL, PIN_START, 25, INPUT,       true), // BUTTON_ID_START, GPIO0 has fixed hardware pullup
  new Fri3d_Button (FRI3D_BUTTON_TYPE_ANALOGUE_HIGH, PIN_JOY_Y, 0, INPUT,       false), // BUTTON_ID_UP
  new Fri3d_Button (FRI3D_BUTTON_TYPE_ANALOGUE_LOW, PIN_JOY_Y, 0, INPUT,       false), // BUTTON_ID_DOWN
  new Fri3d_Button (FRI3D_BUTTON_TYPE_ANALOGUE_LOW, PIN_JOY_X , 0, INPUT,       false), // BUTTON_ID_LEFT
  new Fri3d_Button (FRI3D_BUTTON_TYPE_ANALOGUE_HIGH, PIN_JOY_X , 0, INPUT,       false), // BUTTON_ID_RIGHT
};

Fri3dBadge_Joystick joystick;
Fri3dBadge_Buzzer buzzer;

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

// Buzzer test functions
void buzzer_test() {
  buzzer.tone(500, 200);
  buzzer.setVolume(1);
  buzzer.tone(200, 200);
  buzzer.noTone();
}

void i2c_scanner() {
  byte error, address;
  int nDevices;

  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  Serial.println("Scanning I2C ...");
  Serial.print("SDA: ");
  Serial.println(PIN_I2C_SDA);
  Serial.print("SCL: ");
  Serial.println(PIN_I2C_SCL);

  nDevices = 0;
  for (address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknow error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}

void led_rgb_test() {
  static int count = 0;
  int color[3] = {25, 0, 0};

  FastLED.addLeds<WS2812, PIN_WS2812, GRB>(leds, NUMLEDPIXELS);
  for (int i = 0; i < NUMLEDPIXELS; i++) {
    leds[i] = CRGB(color[count % 3], color[(count + 1) % 3], color[(count + 2) % 3]);
  }
  FastLED.show();
  ++count;
}

void ir_init() {
  irrecv.enableIRIn();  // Start the receiver
}


void ir_loop() {
  if (irrecv.decode(&results)) {
    led_rgb_test();

    // Display the basic output of what we found.
    Serial.print(resultToHumanReadableBasic(&results));

    // Display any extra A/C info if we have it.
    String description = IRAcUtils::resultAcToString(&results);
    if (description.length()) Serial.println(description);
    yield();  // Feed the WDT as the text output can take a while to print.
    // Output the results as source code
    Serial.println(resultToSourceCode(&results));
    Serial.println();    // Blank line between entries
    yield();             // Feed the WDT (again)
  }
}

void display_test()
{
  tft.init();
  tft.writecommand(TFT_MADCTL);
  tft.writedata(TFT_MAD_BGR | TFT_MAD_MV); //exchange red and blue bytes, and mirror x-coordinates
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(tft.width() / 2 - 90, 20);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.println("Fri3d Badge 2024");
}

void buttonshow(int buttonid)
{
  Fri3d_Button *b = buttons[buttonid];
  int x = 0;
  int y = 0;
  int w = 0;
  uint32_t col;
  const char *buttonname = "";

  switch (buttonid)
  {
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
  }
  Serial.print(buttonname);
  if (b->isPressed())
  {
    Serial.println(" pressed");
    col = TFT_GREEN;
  }
  else {
    Serial.println(" released");
    col = TFT_BLACK;
  }
  if (x > 0)
  {
    tft.fillRoundRect(x, y, w, 18, 2, col);
    tft.drawRoundRect(x, y, w, 18, 2, TFT_WHITE);
    tft.setCursor(x + 5, y + 2);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.println(buttonname);
  }
}

void buttonshowall()
{
  buttonshow(BUTTON_ID_MENU);
  buttonshow(BUTTON_ID_START);
  buttonshow(BUTTON_ID_A);
  buttonshow(BUTTON_ID_B);
  buttonshow(BUTTON_ID_X);
  buttonshow(BUTTON_ID_Y);
}

void button_check(int buttonid)
{
  Fri3d_Button *b = buttons[buttonid];
  if (b->wasPressed() || b->wasReleased())
  {
    buttonshow(buttonid);
  }
}

void button_loop()
{
  button_check(BUTTON_ID_MENU);
  button_check(BUTTON_ID_START);
  button_check(BUTTON_ID_A);
  button_check(BUTTON_ID_B);
  button_check(BUTTON_ID_X);
  button_check(BUTTON_ID_Y);
}

void init_sd()
{
  SPIClass& spix = SPI;  // Create a class variable to hold the SPI class instance (default instance is SPI port 0 pins only)
  spix = tft.getSPIinstance(); // Set to instance used by TFT library (may be SPI port 0 or 1)

  if (!SD.begin(PIN_SDCARD_CS, spix, SPI_FREQUENCY)) {
    Serial.println("Initialisatie van SD-kaart mislukt!");
  }
  else
  {
    Serial.println("Initialisatie SD-kaart klaar.");
    // hier mag geen tft commando
    status_sd = 1;
  }
}

void show_sd()
{
  if (status_sd)
  {
    tft.fillCircle(225, 165, 8, TFT_GREEN);
  }
  tft.drawCircle(225, 165, 8, TFT_WHITE);
  tft.setCursor(240, 160);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.println("SD");
}

void joystick_setup()
{
  joystick_sprite.createSprite(JOYSTICK_SPRITE_WIDTH, JOYSTICK_SPRITE_HEIGHT);
}

void joystick_show()
{
  const int radius = 10;
  const int maxx  = JOYSTICK_SPRITE_WIDTH - 2 * radius;
  const int maxy = JOYSTICK_SPRITE_HEIGHT - 2 * radius;
  int x, y;

  joystick_sprite.fillSprite(TFT_BLACK);
  joystick.getXY(&x, &y);
  // TODO map gebruiken
  int newx = radius + (x + 2048) * maxx / 4096;
  int newy = JOYSTICK_SPRITE_HEIGHT - ((y + 2048) * maxy / 4096 + radius);
  joystick_sprite.drawRect(0, 0, JOYSTICK_SPRITE_WIDTH, JOYSTICK_SPRITE_HEIGHT, TFT_WHITE );
  joystick_sprite.drawWideLine(JOYSTICK_SPRITE_WIDTH / 2, JOYSTICK_SPRITE_HEIGHT / 2, newx, newy, 5, TFT_DARKGREY, TFT_BLACK);
  joystick_sprite.fillCircle(newx, newy, radius, TFT_GREEN);
  joystick_sprite.pushSprite(30, 150);
}

void imu_setup()
{
  // Wire is al opgestard

  imu.init(ISDS_ADDRESS_I2C_1);
  imu.SW_RESET();
  imu.select_ODR(2);
  imu.set_Mode(2);

  horizon_sprite.createSprite(HORIZON_SPRITE_WIDTH, HORIZON_SPRITE_HEIGHT);
}

double dotprod(int16_t x1, int16_t  y1, int16_t x2, int16_t y2)
{
  double res = (double) x1 * x2 + (double) y1 * y2;
  return res;
}

void imu_show_horizon()
{
  int status;
  int16_t acc_X, acc_Y, acc_Z;

  horizon_sprite.fillSprite(TFT_BLACK);
  horizon_sprite.drawRect(0, 0, HORIZON_SPRITE_WIDTH, HORIZON_SPRITE_HEIGHT, TFT_WHITE );

  status = imu.get_accelerations(&acc_X, &acc_Y, &acc_Z);
  if (status == WE_FAIL) {
    // cross
    horizon_sprite.drawWideLine(0, 0, HORIZON_SPRITE_WIDTH, HORIZON_SPRITE_HEIGHT, 5, TFT_RED, TFT_BLACK);
    horizon_sprite.drawWideLine(0, HORIZON_SPRITE_HEIGHT, HORIZON_SPRITE_WIDTH, 0, 5, TFT_RED, TFT_BLACK);
  }
  else {
    //assumption: acc(X,Y,Z) is a vector that points down (to gravity)
    //we then calculate angle with the vector that we want to see, i.e. perfect down, (-1000, 0, 0)
    double mag_vector = sqrt((double)acc_X * acc_X + (double)acc_Y * acc_Y);
    double cosangle = dotprod(-1000, 0, acc_X, acc_Y) / 1000.0 / mag_vector;
    double sign = abs(acc_Y) / acc_Y;
    double angle_radians = sign * (cosangle < 0 ? 2 * PI - acos(-cosangle) : acos(cosangle));
    //given the angle, calculate #pixels we need to deviate from middle to draw our horizon:
    //once the angle goes over 45, this doesn't work and we should actually calculate xdiff
    float ydiff = sin(angle_radians) * HORIZON_SPRITE_WIDTH / 2.0;

    horizon_sprite.drawWideLine(0, HORIZON_SPRITE_HEIGHT / 2 + ydiff, HORIZON_SPRITE_WIDTH, HORIZON_SPRITE_HEIGHT / 2 - ydiff, 5, TFT_GREEN, TFT_BLACK);
  }
  horizon_sprite.pushSprite(120, 150);
}

void led_init()
{
  pinMode(PIN_LED, OUTPUT);
}

void led_update()
{
  digitalWrite(PIN_LED, (millis() % 1000) > 500 ? true  : false);
}


void battery_sprite_setup()
{
  battery_sprite.createSprite(BATTERY_SPRITE_WIDTH, BATTERY_SPRITE_HEIGHT);
  battery_sprite.setTextColor(TFT_WHITE);
  battery_sprite.setTextSize(2);
}

void battery_sprite_show()
{
  uint16_t bat_raw;
  uint8_t bat_percent;
  static unsigned long next_show = millis();

  if (millis() < next_show) return;

  battery_sprite.fillSprite(TFT_BLACK);
  bat_raw = Battery::read_raw();
  bat_percent = Battery::raw_to_percent(bat_raw);
  battery_sprite.setCursor(0, 2);
  battery_sprite.print("BAT");
  battery_sprite.setCursor(0, 20);
  battery_sprite.printf("%d%%", bat_percent);
  battery_sprite.pushSprite(217, 190);
  next_show += 1000; // 1 second
}


void setup()
{
  Serial.begin(115200);

  // delay(5000);
  // Serial.println("Fri3d badge hardware test");
  // Serial.print("My IDF Version is: ");
  // Serial.println(esp_get_idf_version());

  buttons_init();
  buzzer_test();
  i2c_scanner();
  led_rgb_test();
  led_init();
  ir_init();
  display_test();
  init_sd(); // moet na display_test (want spi moet al begonnen zijn)
  buttonshowall();
  show_sd(); // mag dit onmiddellijk na init_sd ???
  joystick_setup();
  imu_setup();
  battery_sprite_setup();
}

void loop()
{
  ir_loop();
  led_update();
  buttons_update();
  button_loop();
  joystick_show();
  imu_show_horizon();
  battery_sprite_show();
}
