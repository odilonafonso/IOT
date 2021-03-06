/*
 * Retrieve the MAC address from a Microchip 24AA125E48 I2C ROM, and report it
 * to the serial console at 57600bps. The I2C address of the ROM is set to 0x50,
 * which assumes both the address pins are tied to 0V.
 */

#define I2C_ADDRESS 0x50
#include <Wire.h>

static uint8_t mac[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; 

void setup()
{
  Wire.begin();         // Join i2c bus (I2C address is optional for the master)
  Serial.begin(9600);
  for(int i = 0; i < 30; i++)
  {
    Serial.println(" ");
  }
  Serial.println("Starting test for MAC address ROM");
  Serial.print("Getting MAC: ");
  
  mac[0] = readRegister(0xFA);
  mac[1] = readRegister(0xFB);
  mac[2] = readRegister(0xFC);
  mac[3] = readRegister(0xFD);
  mac[4] = readRegister(0xFE);
  mac[5] = readRegister(0xFF);
  
  char tmpBuf[17];
  sprintf(tmpBuf, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.println(tmpBuf);
  Serial.println(" TEST OK");
}

void loop()
{
  // Do nothing
}

byte readRegister(byte r)
{
  unsigned char v;
  Wire.beginTransmission(I2C_ADDRESS);
  Wire.write(r);  // Register to read
  Wire.endTransmission();

  Wire.requestFrom(I2C_ADDRESS, 1); // Read a byte
  while(!Wire.available())
  {
    // Wait
  }
  v = Wire.read();
  return v;
} 

