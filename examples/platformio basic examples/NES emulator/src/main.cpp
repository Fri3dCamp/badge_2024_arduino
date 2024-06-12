/* Arduino Nofrendo
 * Please check hw_config.h and display.cpp for configuration details
 */
#include <Arduino.h>

#include <esp_wifi.h>
#include <esp_task_wdt.h>
#include <FFat.h>
#include <SPIFFS.h>
#include <SD.h>
#include <SD_MMC.h>

#include <Arduino_GFX_Library.h>

#include "hw_config.h"

extern "C"
{
#include <nofrendo.h>
}

int16_t bg_color;
extern Arduino_TFT *gfx;
extern void display_begin();

void setup()
{
    Serial.begin(115200);
    Serial.println("arduino-nofrendo-fri3d-camp-badge starting...");

    // turn off WiFi
    esp_wifi_deinit();

    // disable Core 0 WDT
    TaskHandle_t idle_0 = xTaskGetIdleTaskHandleForCPU(0);
    esp_task_wdt_delete(idle_0);

    // start display
    display_begin();

    // filesystem defined in hw_config.h
    FILESYSTEM_BEGIN

    // find first rom file (*.nes)
    File root = filesystem.open("/");
    char *argv[1];
    if (!root)
    {
        Serial.println("Filesystem mount failed! Please check hw_config.h settings.");
        gfx->println("Filesystem mount failed! Please check hw_config.h settings.");
    }
    else
    {
        int fileToUse = 0; // which .nes file to use. 0 means use the first one.
        int fileCounter = 0;
        bool foundRom = false;

        Serial.println("Files found:");
        File file = root.openNextFile();
        while(file){
            Serial.print("FILE: ");
            Serial.println(file.name());
            file = root.openNextFile();
        }

        root = filesystem.open("/");
        file = root.openNextFile();
        while (file)
        {
            if (file.isDirectory())
            {
                // skip
            }
            else
            {
                char *filename = (char *)file.name();
                int8_t len = strlen(filename);
                if (strstr(strlwr(filename + (len - 4)), ".nes"))
                {
                    if (fileCounter == fileToUse) {
                    foundRom = true;
                    char fullFilename[256];
                    sprintf(fullFilename, "%s/%s", FSROOT, filename);
                    Serial.println(fullFilename);
                    argv[0] = fullFilename;
                    break;
                    }
                    fileCounter++;
                }
            }

            file = root.openNextFile();
        }

        if (!foundRom)
        {
            Serial.println("Failed to find rom file, please copy rom file to data folder and upload with \"ESP32 Sketch Data Upload\"");
            argv[0] = "/";
        }

        Serial.println("NoFrendo start!\n");
        nofrendo_main(1, argv);
        Serial.println("NoFrendo end!\n");
    }
}

void loop()
{
}
