/*
 * https://brainy-bits.com/tutorials/using-the-hmc5883l/
 * 
 */
#include <Wire.h>
 
#define NUM_LEDS 8  // Number of LEDs per Stick
#define DATA_PIN_X 2  // Pin 2 connected to RGB X
#define DATA_PIN_Y 3  // Pin 3 connected to RGB Y
 
/* The I2C address of the module */
#define HMC5803L_Address 0x1E
 
/* Register address for the X Y and Z data */
#define X 3
#define Y 7
#define Z 5

int xOld, xNew, yOld, yNew, zOld, zNew;

#define varX 3
#define varY 3
#define varZ 3
#define LAVAGEM 10

int xDif, yDif, zDif;

long initInterval;
long now;

#define INTERVAL_SIZE 800
#define DELAY_MEDIDAS 50

int xTotVar, yTotVar, zTotVar;

void setup() 
{
   Serial.begin(9600);
   Serial.println("Start..");
   Wire.begin();
   xOld = yOld = zOld = -1;

   xTotVar = yTotVar = zTotVar = 0;
     
   /* Initialise the module */ 
   Init_HMC5803L();
   initInterval = millis();
}

void loop() {
   now = millis();

   if ( (now - initInterval) > INTERVAL_SIZE ) {
      int totvar = xTotVar + yTotVar + zTotVar;
      if (totvar > 0) {
         Serial.print("TotVar=");Serial.print(totvar);
         if (totvar <= LAVAGEM)
            Serial.println("Lavagem");
         else
            Serial.println("Centrifugacao");
      }
//      Serial.print("xTotVar=");Serial.print(xTotVar);
//      Serial.print(" yTotVar=");Serial.print(yTotVar);
//      Serial.print(" zTotVar=");Serial.println(zTotVar);
      xTotVar = yTotVar = zTotVar = 0;
      initInterval = now;
   }
   /* Read each sensor axis data and output to the serial port */
   xNew = HMC5803L_Read(X);
   yNew = HMC5803L_Read(Y);
   zNew = HMC5803L_Read(Z);

   if (xOld == -1) {
      xOld = xNew;
      yOld = yNew;
      zOld = zNew;
      xTotVar = yTotVar = zTotVar = 0;
   }

   if (xNew > xOld)
      xDif = xNew - xOld;
   else
      xDif = xOld - xNew;
   if (xDif > varX) {
      xTotVar++;
//      Serial.print("xOld:");Serial.print(xOld);
//      Serial.print(" xNew:");Serial.print(xNew);
//      Serial.print(" varX:");Serial.println(xDif);
   }

   if (yNew > yOld)
      yDif = yNew - yOld;
   else
      yDif = yOld - yNew;
   if (yDif > varY) {
      yTotVar++;
//      Serial.print("yOld:");Serial.print(yOld);
//      Serial.print(" yNew:");Serial.print(yNew);
//      Serial.print(" varY:");Serial.println(yDif);
   }

   if (zNew > zOld)
      zDif = zNew - zOld;
   else
      zDif = zOld - zNew;
   if (zDif > varZ) {
      zTotVar++;
//      Serial.print("zOld:");Serial.print(zOld);
//      Serial.print(" zNew:");Serial.print(zNew);
//      Serial.print(" varZ:");Serial.println(zDif);
   }

      xOld = xNew;
      yOld = yNew;
      zOld = zNew;
/*
   Serial.print(xNew);   Serial.print(" ");
   Serial.print(yNew);   Serial.print(" ");
   Serial.print(zNew);
*/  

  /* Wait a little before reading again */
  delay(DELAY_MEDIDAS);
}
 
 
/* This function will initialise the module and only needs to be run once
   after the module is first powered up or reset */
void Init_HMC5803L(void)
{
  /* Set the module to 8x averaging and 15Hz measurement rate */
  Wire.beginTransmission(HMC5803L_Address);
  Wire.write(0x00);
  Wire.write(0x70);
          
  /* Set a gain of 5 */
  Wire.write(0x01);
  Wire.write(0xA0);
  Wire.endTransmission();
}
 
 
/* This function will read once from one of the 3 axis data registers
and return the 16 bit signed result. */
int HMC5803L_Read(byte Axis)
{
  int Result;
  
  /* Initiate a single measurement */
  Wire.beginTransmission(HMC5803L_Address);
  Wire.write(0x02);
  Wire.write(0x01);
  Wire.endTransmission();
  delay(6);
  
  /* Move modules the resiger pointer to one of the axis data registers */
  Wire.beginTransmission(HMC5803L_Address);
  Wire.write(Axis);
  Wire.endTransmission();
   
  /* Read the data from registers (there are two 8 bit registers for each axis) */  
  Wire.requestFrom(HMC5803L_Address, 2);
  Result = Wire.read() << 8;
  Result |= Wire.read();
 
  return Result;
}
