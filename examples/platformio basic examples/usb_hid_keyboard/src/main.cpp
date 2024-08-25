/* 
 *  This example let the Fri3d badge 2024 act as a keyboard on USB (USB HID device keyboard)
 *  When compiling in Arduino IDE, make sure to select Tools>USB Mode>USB-OTG (TinyUSB)
 *  This example uses the Adafruit TinyUSB library that can be found in the Arduino IDE or
 *    https://github.com/adafruit/Adafruit_TinyUSB_Arduino
 *  Use Adafruit TinyUSB version 3.1.0 and ESP32 Arduino core 2.0.14
*/ 

#include <Arduino.h>
#include "Adafruit_TinyUSB.h"

#include "Fri3d_Button.h"

#define PIN_EXPANSION_RX 44
#define PIN_EXPANSION_TX 43

// HID report descriptor using TinyUSB's template
// Single Report (no ID) descriptor
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD()
};

// USB HID object. For ESP32 these values cannot be changed after this declaration
// desc report, desc len, protocol, interval, use out endpoint
Adafruit_USBD_HID usb_hid;

enum{
  BUTTON_ID_A=0,
  BUTTON_ID_B,
  BUTTON_ID_X,
  BUTTON_ID_Y,
  BUTTON_ID_MENU,
  BUTTON_ID_START,
  BUTTON_ID_UP,
  BUTTON_ID_DOWN,
  BUTTON_ID_LEFT,
  BUTTON_ID_RIGHT,
  BUTTON_ID_TOUCH,


  NUM_BUTTONS,
};

// Fri3d_Button(buttontype, pin, dbTime, mode, invert)

Fri3d_Button *buttons[NUM_BUTTONS]={
  new Fri3d_Button (FRI3D_BUTTON_TYPE_DIGITAL,      39,25,INPUT_PULLUP,true),  // BUTTON_ID_A
  new Fri3d_Button (FRI3D_BUTTON_TYPE_DIGITAL,      40,25,INPUT_PULLUP,true),  // BUTTON_ID_B
  new Fri3d_Button (FRI3D_BUTTON_TYPE_DIGITAL,      38,25,INPUT_PULLUP,true),  // BUTTON_ID_X
  new Fri3d_Button (FRI3D_BUTTON_TYPE_DIGITAL,      41,25,INPUT_PULLUP,true),  // BUTTON_ID_Y
  new Fri3d_Button (FRI3D_BUTTON_TYPE_DIGITAL,      45,25,INPUT_PULLUP,true),  // BUTTON_ID_MENU
  new Fri3d_Button (FRI3D_BUTTON_TYPE_DIGITAL,       0,25,INPUT,       true),  // BUTTON_ID_START, GPIO0 has fixed hardware pullup
  new Fri3d_Button (FRI3D_BUTTON_TYPE_ANALOGUE_HIGH, 3, 0,INPUT,       false), // BUTTON_ID_UP
  new Fri3d_Button (FRI3D_BUTTON_TYPE_ANALOGUE_LOW,  3, 0,INPUT,       false), // BUTTON_ID_DOWN
  new Fri3d_Button (FRI3D_BUTTON_TYPE_ANALOGUE_LOW,  1, 0,INPUT,       false), // BUTTON_ID_LEFT
  new Fri3d_Button (FRI3D_BUTTON_TYPE_ANALOGUE_HIGH, 1, 0,INPUT,       false), // BUTTON_ID_RIGHT
  new Fri3d_Button (FRI3D_BUTTON_TYPE_TOUCH,         2,25,INPUT,       false), // BUTTON_ID_TOUCH, touch sensor on pin 2 of connector
};

const char *button_names[NUM_BUTTONS]={"A","B","X","Y","MENU","START","UP","DOWN","LEFT","RIGHT","T"};

// For keycode definition check out https://github.com/hathach/tinyusb/blob/master/src/class/hid/hid.h
// QWERTY keyboard style: A B X Y M S (when user has azerty setup ) & cursors & T 
uint8_t hid_keys[NUM_BUTTONS]= {HID_KEY_Q, HID_KEY_B, HID_KEY_X, HID_KEY_Y, HID_KEY_SEMICOLON, HID_KEY_S, HID_KEY_ARROW_UP, HID_KEY_ARROW_DOWN, HID_KEY_ARROW_LEFT, HID_KEY_ARROW_RIGHT,HID_KEY_T};

void checkButton(Fri3d_Button *b,const char *buttonname, uint8_t hidcode) {

    b->read();

    if (b->wasPressed())
    {
      // Serial.print(buttonname);
      // Serial.println(" pressed");
      while ( ! usb_hid.ready() ) {
        delay(1);
      }

      uint8_t keycode[6] = { 0 };
      keycode[0] = hidcode;

      usb_hid.keyboardReport(0, 0, keycode);
    }
    if (b->wasReleased())
    {
      // Serial.print(buttonname);
      // Serial.println(" released");
      while ( ! usb_hid.ready() ) {
        delay(1);  
      }
      usb_hid.keyboardRelease(0);
    }
}


void setup()
{
  // Setup HID
  usb_hid.setBootProtocol(HID_ITF_PROTOCOL_KEYBOARD);
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.setStringDescriptor("Fri3d keyboard");
  usb_hid.begin();
  
  // wait until device mounted
  while( !TinyUSBDevice.mounted() ) delay(1);

  // Serial.begin(115200);
  // delay(3000); 
  // Serial.println("Fri3dbadge USB HID test");
  for (int count=0; count<NUM_BUTTONS; ++count)
  {
    buttons[count]->begin();
  } 

  // Setup UART connection to communicator
  Serial2.begin(115200, SERIAL_8N1, PIN_EXPANSION_RX, PIN_EXPANSION_TX);
  Serial2.flush(false); // Flush TX & RX
}


void checkcommunicator()
{
  unsigned char hidcode[8];

  for (int count = 0; count < 8; ++count)
  {
    if (Serial2.available())
    {
      hidcode[count] = Serial2.read();
    }
    else
    {
      return; // only send HID code when 8 bytes have been received
    }
  }
  usb_hid.sendReport(0, hidcode,8);
}

void loop()
{
  checkcommunicator();
  for (int count=0; count<NUM_BUTTONS; ++count)
  {
    checkButton(buttons[count],button_names[count],hid_keys[count]);
  }
  delay(2);
}
