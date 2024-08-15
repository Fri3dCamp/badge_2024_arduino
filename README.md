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

### Selecting the Fri3d Badge under boards
* If you have successfully installed the library, then you should now be able to select the `Fri3d Badge 2024 (ESP32-S3-WROOM-1)` board under **Tools>Board>fri3d-esp32**

### Arduino IDE examples
* After you have selected the Fri3d Badge 2024 board, you should also find a `Fri3d Badge 2024` section under **File>Examples**
* Code for these examples can also be found under [arduino-ide-board-package/libraries/Fri3dBadge2024/examples](./arduino-ide-board-package/libraries/Fri3dBadge2024/examples)

### Uploading firmware using Arduino IDE
* Connect the badge to your computer with a USB-C cable
* Select the correct USB port under **Tools>Port** (on a Mac it's along the lines of `/dev/cu.usbserial-FFFFFFFF`)
  * Troubleshooting tip: if you cannot see your board, make sure it's turned on and plugged in with a good usb cable.
* Compile and upload the code with **Sketch>Upload**
  *  Troubleshooting tip: If upload fails even though compilation succeeds, then you might need to manually put it in boot mode. To do that, hold the boot button and then press the reset button, then after a second you can release the boot button. 
* Change and mix the examples and have fun!

#### Starting your sketch
* For sketches to work, you need to have the main firmware installed.
  * If you don't, see https://fri3dcamp.github.io/badge_2024/reset/#web-interface for more info about that.
* Then, in the main firmware launcher menu on the badge, select "MicroPython".
  * Your sketch will start now.
  * Resetting the esp will return to main firmware.

#### Running sketches without main firmware
* When you are using our fri3d-esp32 boards manager package, esp_tool will be configured to only write to the micropython partition (at address 0x410000).
  * The main firmware then needs to direct the esp32 to start from that partition.
* If you want to overwrite the main firmware instead, then follow choose **Sketch>Upload Using Programmer** in the Arduino IDE.
  * The first time, you also need to select the **EspTool** option via **Tools>Programmer** in the Arduino IDE.
  * If you do this, you will later have to install the main firmware again yourself.




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
