# badge_2024_arduino
Arduino support code for the Fri3dcamp 2024 badge
# Arduino library for the Fri3d Camp 2024 Badge

This repository contains C++ examples for the [Fri3d Camp 2024 badge](https://github.com/Fri3dCamp/badge_2024), to be used with the Arduino IDE or other editors like Visual Studio Code that support Arduino libraries (PlatformIO). 

If you prefer Python over C++, check out the [MicroPython repository](https://github.com/Fri3dCamp/badge_2024_micropython)

## Arduino IDE Settings

The badge carries an ESP32-S3 with some peripherals and custom pin settings. In order to work easily with the board in Arduino IDE, you should install the esp32-fri3d board package.

### Installing the custom esp32-fri3d board package
* In your Arduino IDE, open **File>Preferences** or **Settings**
* Enter `https://github.com/Fri3dCamp/badge_2024_arduino/releases/latest/download/package_fri3d-esp32_index.json` into the “Additional Board Manager URLs” field
* Open **Tools>Board>Board Manager**
* Search for the `fri3d-esp32` boards manager from Fri3d Vzw and install the latest version.

#### Alternative option: using official espressif esp32 boards package
* If you for some reason want to use the [official espressif esp32 boards package](https://espressif.github.io/arduino-esp32) instead of our modified package, then follow the instructions in [README_arduino_esp32_espressif.md](./README_arduino_esp32_espressif.md).

### Uploading firmware using Arduino IDE
* Connect the badge to your computer with a USB-C cable
* Select the correct USB port under **Tools>Port** (on a Mac it's along the lines of `/dev/cu.usbserial-FFFFFFFF`)
  * Troubleshooting tip: if you cannot see your board, make sure it's turned on and plugged in with a good usb cable.
* Compile and upload the code with **Sketch>Upload**
  *  Troubleshooting tip: If upload fails even though compilation succeeds, then you might need to manually put it in boot mode. To do that, hold the boot button and then press the reset button, then after a second you can release the boot button. 
* Change and mix the examples and have fun!


## PlatformIO settings

Following settings can be used. Write following lines of code in platformio.ini

```
platform = espressif32@^6.3.2
board = esp32-s3-devkitc-1

; Configure options for the N16R8V variant
board_build.arduino.memory_type = qio_opi 
board_build.partitions = default_16MB.csv
board_upload.flash_size = 16MB

framework = arduino
monitor_speed = 115200

build_flags =
    ; N16R8V has PSRAM
   -D BOARD_HAS_PSRAM 
    ; necessary for serial port
   -D ARDUINO_USB_CDC_ON_BOOT=1
```
## Gotchas
* Normally the badge automatically goes into bootloader mode. If it doesn't (you might see the error "The chip needs to be in download mode."), you can do it manually:  *press and hold* the start button, press the reset button, then release the start button. You can do this before uploading the firmware at any moment. After uploading press reset again. In case you have a serial monitor, close and open it again.
