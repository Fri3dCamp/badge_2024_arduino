## Arduino IDE setup using espressif's arduino-esp32 board package
In case you do not want to use the custom fri3d board package in the arduino IDE, you should follow the below steps instead to install the official arduino-esp32 board package.

### Add ESP32-S3 board to your Arduino IDE

* Open **Tools>Board>Board Manager**
* Search for the `esp32` boards from Espressif Systems and install the version 2.0.14 (remark that the display driver crashes in 2.0.15+ and 3.x.x).
* If you do not find the esp32 boards package there, then follow these steps first: 
  * In your Arduino IDE, open **File>Preferences**
  * Enter `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json` into the “Additional Board Manager URLs” field


### Uploading firmware using Arduino IDE
* Under **Tools>Board>...** select **..>ESP32 Arduino>ESP32S3 Dev Module**
* Under **Tools>USB CDC On Boot>...** select `Enabled` to enable the serial port
* Under **Tools>Flash Size>...** select `16 MB`
* Under **Tools>PSRAM>...** select `OPI PSRAM`
* follow the rest of the uploading steps from the [README.md](./README.md)