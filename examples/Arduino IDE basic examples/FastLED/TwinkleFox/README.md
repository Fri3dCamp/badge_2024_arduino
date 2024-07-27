# Arduino IDE Setup
TLDR; To make this work you need the ESP32S3 Dev Module board and the FastLED Library installed

## Working with the 2024 Fried badge in the Arduino IDE
- First read the general README.md for [Arduino IDE](../../README.md)

## Arduino IDE Setup Steps
- From the Library manager in the Arduino IDE, you need to install the FastLED NeoPixel package by Daniel Garcia.


### Succesful upload output
For reference here the output that you get for a succesful compilation and upload (on a mac)

```
In file included from /Users/username/Documents/Arduino/libraries/FastLED/src/FastLED.h:79,
                 from /Users/username/gitProjects/PSNL/fri3d-badge_2024_arduino/examples/arduino basic examples/FastLED/TwinkleFox/TwinkleFox.ino:4:
/Users/username/Documents/Arduino/libraries/FastLED/src/fastspi.h:157:23: note: #pragma message: No hardware SPI pins defined.  All SPI access will default to bitbanged output
 #      pragma message "No hardware SPI pins defined.  All SPI access will default to bitbanged output"
                       ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Sketch uses 243429 bytes (18%) of program storage space. Maximum is 1310720 bytes.
Global variables use 18608 bytes (5%) of dynamic memory, leaving 309072 bytes for local variables. Maximum is 327680 bytes.
esptool.py v4.5.1
Serial port /dev/cu.usbmodem101
Connecting...
Chip is ESP32-S3 (revision v0.1)
Features: WiFi, BLE
Crystal is 40MHz
MAC: 34:85:18:74:70:b8
Uploading stub...
Running stub...
Stub running...
Changing baud rate to 921600
Changed.
Configuring flash size...
Flash will be erased from 0x00000000 to 0x00003fff...
Flash will be erased from 0x00008000 to 0x00008fff...
Flash will be erased from 0x0000e000 to 0x0000ffff...
Flash will be erased from 0x00010000 to 0x0004bfff...
Compressed 15104 bytes to 10430...
Writing at 0x00000000... (100 %)
Wrote 15104 bytes (10430 compressed) at 0x00000000 in 0.3 seconds (effective 444.2 kbit/s)...
Hash of data verified.
Compressed 3072 bytes to 146...
Writing at 0x00008000... (100 %)
Wrote 3072 bytes (146 compressed) at 0x00008000 in 0.1 seconds (effective 372.4 kbit/s)...
Hash of data verified.
Compressed 8192 bytes to 47...
Writing at 0x0000e000... (100 %)
Wrote 8192 bytes (47 compressed) at 0x0000e000 in 0.1 seconds (effective 560.2 kbit/s)...
Hash of data verified.
Compressed 243792 bytes to 135941...
Writing at 0x00010000... (11 %)
Writing at 0x0001d037... (22 %)
Writing at 0x00023839... (33 %)
Writing at 0x00028cae... (44 %)
Writing at 0x0002e0fa... (55 %)
Writing at 0x0003438b... (66 %)
Writing at 0x0003e916... (77 %)
Writing at 0x000441a0... (88 %)
Writing at 0x00049b5d... (100 %)
Wrote 243792 bytes (135941 compressed) at 0x00010000 in 1.9 seconds (effective 1007.4 kbit/s)...
Hash of data verified.

Leaving...
Hard resetting via RTS pin...
```