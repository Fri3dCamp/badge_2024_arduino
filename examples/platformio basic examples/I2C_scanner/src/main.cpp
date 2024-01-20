#include <Arduino.h>

// --------------------------------------
// i2c_scanner
//
// This sketch tests the standard 7-bit addresses
// Devices with higher bit address might not be seen properly.
//

// Remark that the I2C connector on the Fri3dbadge is the Stemma QT / QWIIC
// Remark that this looks the same as the SH1.0 connector on Wemos chips, but the wires are in the opposite order!

#include <Wire.h>

void setup()
{
  Serial.begin(115200);
  delay(3000);
  Serial.println("I2C Scanner");

  // Wire.begin(); 
  Wire.begin(I2C_SDA, I2C_SCL);
}


void loop()
{
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");
  Serial.print("SDA: ");
  Serial.println(I2C_SDA);
  Serial.print("SCL: ");
  Serial.println(I2C_SCL);

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

  delay(1000);           // wait 5 seconds for next scan
}
