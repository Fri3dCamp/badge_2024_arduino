
#ifndef FRI3d_BUTTON_H_INCLUDED
#define FRI3d_BUTTON_H_INCLUDED

// Based on 
// Arduino Button Library
// https://github.com/JChristensen/JC_Button
// Copyright (C) 2018 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html

#include <Arduino.h>

enum Fri3DButton_type
{
 FRI3D_BUTTON_TYPE_DIGITAL=1,
 FRI3D_BUTTON_TYPE_ANALOGUE_HIGH,
 FRI3D_BUTTON_TYPE_ANALOGUE_LOW,
 FRI3D_BUTTON_TYPE_TOUCH,
};

class Fri3d_Button
{
    public:
        // Button(pin, dbTime, puEnable, invert) instantiates a button object.
        //
        // Required parameter:
        // pin      The Arduino pin the button is connected to
        //
        // Optional parameters:
        // dbTime   Debounce time in milliseconds (default 25ms)
        // puEnable true to enable the AVR internal pullup resistor (default true)
        // invert   true to interpret a low logic level as pressed (default true)
        Fri3d_Button(Fri3DButton_type buttontype, uint8_t pin, uint32_t dbTime=25, uint8_t mode=INPUT_PULLUP, uint8_t invert=true)
            : m_buttontype(buttontype), m_pin(pin), m_dbTime(dbTime), m_mode(mode), m_invert(invert) {}

        // Initialize a Button object
        void begin();

        // Returns the state of the button, true if pressed, false if released.
        // Does debouncing, captures and maintains times, previous state, etc.
        // Call this function frequently to ensure the sketch is responsive to user input.
        bool read();

        // Returns true if the button state was pressed at the last call to read().
        // Does not cause the button to be read.
        bool isPressed() const {return m_state;}

        // Returns true if the button state was released at the last call to read().
        // Does not cause the button to be read.
        bool isReleased() const {return !m_state;}

        // These functions check the button state to see if it changed
        // between the last two reads and return true or false accordingly.
        // These functions do not cause the button to be read.
        bool wasPressed() const {return m_state && m_changed;}
        bool wasReleased() const {return !m_state && m_changed;}

        // Returns true if the button state at the last call to read() was pressed,
        // and has been in that state for at least the given number of milliseconds.
        // This function does not cause the button to be read.
        bool pressedFor(uint32_t ms) const {return m_state && m_time - m_lastChange >= ms;}

        // Returns true if the button state at the last call to read() was released,
        // and has been in that state for at least the given number of milliseconds.
        // This function does not cause the button to be read.
        bool releasedFor(uint32_t ms) const {return !m_state && m_time - m_lastChange >= ms;}

        // Returns the time in milliseconds (from millis) that the button last
        // changed state.
        uint32_t lastChange() const {return m_lastChange;}

    private:
        bool readstate();

        uint8_t m_pin;                  // arduino pin number connected to button
        uint32_t m_dbTime;              // debounce time (ms)
        uint8_t m_mode;                 // pinMode mode 
        bool m_invert;                  // if true, interpret logic low as pressed, else interpret logic high as pressed
        bool m_state = false;           // current button state, true=pressed
        bool m_lastState = false;       // previous button state
        bool m_changed = false;         // state changed since last read
        uint32_t m_time = 0;            // time of current state (ms from millis)
        uint32_t m_lastChange = 0;      // time of last state change (ms)
        Fri3DButton_type m_buttontype;  
};

#endif
