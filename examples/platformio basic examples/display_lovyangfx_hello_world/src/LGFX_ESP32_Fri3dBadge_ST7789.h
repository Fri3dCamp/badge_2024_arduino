#pragma once

#define LGFX_USE_V1

#include <LovyanGFX.hpp>

// Setting to use LovyanGFX on Fri3d badge

class LGFX : public lgfx::LGFX_Device
{
lgfx::Panel_ST7789   _panel_instance;
lgfx::Bus_SPI        _bus_instance;   

public:

  LGFX(void)
  {
    { // Configure bus control settings.
      auto cfg = _bus_instance.config();    // Get the bus configuration structure.

// Configuring the SPI bus
      // cfg.spi_host = VSPI_HOST;          // Select the SPI to use : ESP32 : VSPI_HOST or HSPI_HOST
      // With the ESP-IDF version update, the description of VSPI_HOST and HSPI_HOST will be deprecated, so if an error occurs, please use SPI2_HOST and SPI3_HOST instead.
      // cfg.spi_mode = 0;                  // Set SPI communication mode (0 ~ 3)
      cfg.freq_write = SPI_FREQUENCY;       // SPI clock when transmitting (maximum 80MHz, rounded to 80MHz divided by an integer)
      // cfg.freq_read  = 16000000;         // SPI clock when receiving
      // cfg.spi_3wire  = true;             // Set true if receiving is done using the MOSI pin.
      // cfg.use_lock   = true;             // Set true to use transaction locking
      // cfg.dma_channel = SPI_DMA_CH_AUTO; // Set the DMA channel to use (0=DMA not used / 1=1ch / 2=ch / SPI_DMA_CH_AUTO=automatic setting)
      // With the ESP-IDF version upgrade, SPI_DMA_CH_AUTO (automatic setting) is recommended for the DMA channel. Specifying 1ch or 2ch is not recommended.
      cfg.pin_sclk = TFT_SCLK;              // Set SPI SCLK pin number
      cfg.pin_mosi = TFT_MOSI;              // Set SPI MOSI pin number
      cfg.pin_miso = TFT_MISO;              // Set SPI MISO pin number (-1 = disable)
      cfg.pin_dc   = TFT_DC;                // Set SPI D/C pin number (-1 = disable)
      // If you use the same SPI bus as the SD card, be sure to set MISO without omitting it.

      _bus_instance.config(cfg);    // Reflects the setting value on the bus.
      _panel_instance.setBus(&_bus_instance);      // Place the bus on the panel.
    }

    { // Configure display panel control settings.
      auto cfg = _panel_instance.config();    // Gets the structure for display panel settings.

      cfg.pin_cs           =    TFT_CS;  // Pin number to which CS is connected (-1 = disable)
      cfg.pin_rst          =    TFT_RST;  // Pin number to which RST is connected (-1 = disable)
      // cfg.pin_busy         =    -1;  // Pin number to which BUSY is connected (-1 = disable)

      // The following setting values are general default values set for each panel, so please comment out any items you are unsure of and try again.

      cfg.panel_width      =   TFT_WIDTH;  // Actual displayable width
      cfg.panel_height     =  TFT_HEIGHT;  // Actual display height
      cfg.offset_x         =     0;  // Panel X direction offset amount
      cfg.offset_y         =  TFT_OFFSET_Y;  // Panel Y direction offset amount
      // cfg.offset_rotation  =     0;  // Offset of value in rotation direction 0~7 (4~7 are upside down)
      // cfg.dummy_read_pixel =     8;  // Number of dummy read bits before pixel readout
      // cfg.dummy_read_bits  =     1;  // Number of bits for dummy read before reading data other than pixels
      // cfg.readable         =  true;  // Set to true if data reading is possible
      cfg.invert           = TFT_INVERT;  // Set to true if the brightness and darkness of the panel is reversed.
#ifdef TFT_RGB_ORDER      
      cfg.rgb_order        = true;  // Set to true if the red and blue colors of the panel are swapped.
#else
      cfg.rgb_order        = false;  // Set to true if the red and blue colors of the panel are swapped.
#endif
      // cfg.dlen_16bit       = false;  // Set to true for panels that transmit data length in 16-bit units using 16-bit parallel or SPI.
      cfg.bus_shared       =  true;  // Set to true when sharing the bus with the SD card (control the bus using drawJpgFile, etc.)

      _panel_instance.config(cfg);
    }

  setPanel(&_panel_instance); // Set the panel to be used.
  }
};
