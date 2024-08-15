#include <Arduino.h>

#include <Wire.h>
#include "Fri3dBadge_pins.h"
#include "Fri3dBadge_WSEN_ISDS.h"

Sensor_ISDS sensor;

int status;

void setup() {
  Serial.begin(115200);
   
  // USB-serial needs a bit of time for the connection after reset
  delay(3000);

  Wire.setPins(PIN_I2C_SDA, PIN_I2C_SCL);

  // Initialize the I2C interface
  sensor.init(ISDS_ADDRESS_I2C_1);

  // Get the device ID for this sensor
  int sensor_ID = sensor.get_DeviceID();
  if (WE_FAIL == sensor_ID)
  {
    Serial.println("Error: get_DeviceID(). Stop!");
    while(1);
  }
  
  Serial.print("Sensor ID: ");

  // Print the device ID in hexadecimal on the serial monitor
  Serial.print(sensor_ID, HEX);
  Serial.println();

  // Check if the determined device ID matches the correct device ID (->0x6A) of this sensor
  if (sensor_ID == ISDS_DEVICE_ID_VALUE)
  {
    Serial.println("Communication successful !");
  }
  else
  {
    Serial.println("No communication with the device !");
  }

  // Reset sensor
  status = sensor.SW_RESET();
  if (WE_FAIL == status)
  {
    Serial.println("Error:  SW_RESET(). Stop!");
    while(1);  
  }

  // Set FIFO ODR to 26Hz
  status = sensor.select_ODR(2);
  if (WE_FAIL == status)
    {
    Serial.println("Error:  select_ODR(). Stop!");
    while(1);  
  }

  // Set high performance mode
  status = sensor.set_Mode(2);
  if (WE_FAIL == status)
  {
    Serial.println("Error:  set_Mode(). Stop!");
    while(1);  
  }  
}

void loop() {
  // Temperature
  // Check if sensor is ready to measure the temperature
  status = sensor.is_Temp_Ready();
    
  if (WE_FAIL == status)
  {
    Serial.println("Error: is_Temp_Ready(). Stop!");
    while(1);
  }
  else if (1 == status)
  {
    //Print the temperature on the serial monitor
    
    float temperature;
    if (WE_FAIL == sensor.get_temperature(&temperature))
      {
    Serial.println("Error:  get_temperature(). Stop!");
    while(1);  
    }
    Serial.print("Temperature in [°C]: ");
    Serial.println(temperature);
  }
  else
  {
    Serial.println("Sensor not ready.");
  }

  // Gyro
  status = sensor.is_Gyro_Ready_To_Read();
 if (WE_FAIL == status)
  {
    Serial.println("Error: is_Gyro_Ready_To_Read(). Stop!");
    while(1);
  } 
  else if (1 == status)
  {
    int32_t gyro_X;
    int32_t gyro_Y;
    int32_t gyro_Z;
#if 0
    // Get X-axis angular rate in [mdps]
   
    status = sensor.get_angular_rate_X(&gyro_X);
    if (WE_FAIL == status)
    {
    Serial.println("Error:  get_angular_rate_X(). Stop!");
    while(1);  
  }
    Serial.println("Angular rate in X axis in [mdps]: ");
    Serial.println(gyro_X);

    // Get Y-axis angular rate in [mdps]
    status = sensor.get_angular_rate_Y(&gyro_Y);
    if (WE_FAIL == status)
    {
    Serial.println("Error:  get_angular_rate_Y(). Stop!");
    while(1);  
  }
    Serial.println("Angular rate in  Y axis in [mdps]: ");
    Serial.println(gyro_Y);

    // Get Z-axis angular rate in [mdps]
    status = sensor.get_angular_rate_Z(&gyro_Z);
    if (WE_FAIL == status)
    {
    Serial.println("Error:  get_angular_rate_Z(). Stop!");
    while(1);  
  }
    Serial.println("Angular rate in  Z axis in [mdps]: ");
    Serial.println(gyro_Z);
#else
    status = sensor.get_angular_rates(&gyro_X,&gyro_Y,&gyro_Z);
    if (WE_FAIL == status)
    {
    Serial.println("Error:  get_angular_rates(). Stop!");
    while(1);  
  }
    Serial.print("Angular rate in X,Y,Z axis in [mdps]: ");
    Serial.print(gyro_X);
    Serial.print(" ");
    Serial.print(gyro_Y);
    Serial.print(" ");
    Serial.print(gyro_Z);
    Serial.println(" ");
#endif
  }
  else
  {
    Serial.println("No data in output register.");
  }


  // Accelero
  status = sensor.is_ACC_Ready_To_Read();
  if (WE_FAIL == status)
  {
    Serial.println("Error: is_ACC_Ready_To_Read(). Stop!");
    while(1);
  } 
  else if (1 == status)
  {
    int16_t acc_X;
    int16_t acc_Y;
    int16_t acc_Z;

#if 0
    // Get acceleration along X axis in mg
   status = sensor.get_acceleration_X(&acc_X);
   if (WE_FAIL == status)
  {
    Serial.println("Error:  get_acceleration_X(). Stop!");
    while(1);  
  }
    Serial.println("Acceleration along X axis in [mg]: ");
    Serial.println(acc_X);

    // Get acceleration along Y axis in mg
    status = sensor.get_acceleration_Y(&acc_Y);
    if (WE_FAIL == status)
  {
    Serial.println("Error:  get_accel(). Stop!");
    while(1);  
  }
    Serial.println("Acceleration along Y axis in [mg]: ");
    Serial.println(acc_Y);

    // Get acceleration along Z axis in mg
    status = sensor.get_acceleration_Z(&acc_Z);
    if (WE_FAIL == status)
  {
    Serial.println("Error:  get_acceleration_Z(). Stop!");
    while(1);  
  }
    Serial.println("Acceleration along Z axis in [mg]: ");
    Serial.println(acc_Z);
#else
    
    status = sensor.get_accelerations(&acc_X,&acc_Y,&acc_Z);
    if (WE_FAIL == status)
  {
    Serial.println("Error:  get_accelerations(). Stop!");
    while(1);  
  }
    Serial.print("Acceleration along X,Y,Z axis in [mg]: ");
    Serial.print(acc_X);
    Serial.print(" ");
    Serial.print(acc_Y);
    Serial.print(" ");
    Serial.print(acc_Z);
    Serial.println(" ");
#endif
  }
  else /* status == '0' -> data not ready */
  {
    Serial.println("No data in output register.");
  }

  // separator
  Serial.println(".");
  delay(1000);
}
