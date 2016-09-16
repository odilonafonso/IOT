/*
 * http://www.seeedstudio.com/wiki/Grove_-_Gas_Sensor%28MQ2%29
 * 
 * The Grove - Gas Sensor(MQ2) module is useful for gas leakage detection (in home and industry). 
 * It is suitable for detecting H2, LPG, CH4, CO, Alcohol, Smoke or Propane. 
 * Due to its high sensitivity and fast response time, measurements can be taken as soon as possible. 
 * The sensitivity of the sensor can be adjusted by using the potentiometer. 
 * 
 * Measurement : Approximation
 * 
 * This examples demonstrates a way to know the approximate concentration of Gas.
 * As per the data-sheet of the MQx sensors, these equations are tested for 
 * standard conditions and are not calibrated. 
 * It may vary based on change in temperature or humidity.
 *
 * 1.Keep the Gas Sensor in clean air environment.
 * Upload the below program.
 * 
 */

float sensor_volt; 
float RS_air; //  Get the value of RS via in a clear air
float R0;  // Get the value of R0 via in H2
float sensorValue;

unsigned long initialMillis;
// constants won't change :
const long interval = 5000;

//----------------------------------------------------------------
//Programa : Teste display Nokia 5110 usando biblioteca Adafruit
//Autor : Arduino e Cia

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

// pin 8 - Serial clock out (SCLK)
// pin 9 - Serial data out (DIN)
// pin 10 - Data/Command select (D/C)
// pin 11 - LCD chip select (CS/CE)
// pin 12 - LCD reset (RST)

/*
 * reconfigurado - ver abaixo 
 *
Adafruit_PCD8544 display = Adafruit_PCD8544(8, 9, 10, 11, 12); 
 */
 

/*
 * configurei completamente diferente e está funcionando
 *
 * Pin 1 RST (RST)   -> Arduino digital pin 6
 * Pin 2 CE  (CS/CE) -> Arduino digital pin 7
 * Pin 3 DC  (D/C)   -> Arduino digital pin 5
 * Pin 4 DIN (DIN)   -> Arduino digital pin 4
 * Pin 5 CLK (SCLK)  -> Arduino digital pin 3
 * Pin 6 VCC         -> +3.3V 
 * Pin 7 LIGHT       -> +3.3V com resistor 1K e potenciometro - Lcd backlight
 * Pin 8 GND         -> Ground
 * 
 */

Adafruit_PCD8544 display = Adafruit_PCD8544(3, 4, 5, 7, 6);


void setup() {
   Serial.begin(9600);
   initialMillis = millis();
   display.begin();
   display.setContrast(35); //Ajusta o contraste do display
   display.clearDisplay();   //Apaga o buffer e o display
   display.setTextSize(1);  //Seta o tamanho do texto
   display.setTextColor(BLACK); //Seta a cor do texto
}
 
void loop() {
//   Serial.print("millis() ");Serial.println((int)(millis()/1000));
   if (millis() < (interval - initialMillis))
      getR0();
   else {
      getGasSensor();
      getDHT11();
      displayLCD();
   }
   delay(2000);
}

void getR0() {
 
/*--- Get a average data by testing 100 times ---*/   
   for(int x = 0 ; x < 100 ; x++)
   {
      sensorValue = sensorValue + analogRead(A0);
   }
   sensorValue = sensorValue/100.0;
/*-----------------------------------------------*/
 
   sensor_volt = sensorValue/1024*5.0;
   RS_air = (5.0-sensor_volt)/sensor_volt; // omit *RL
   R0 = RS_air/9.8; // The ratio of RS/R0 is 9.8 in a clear air from Graph (Found using WebPlotDigitizer)
 
   Serial.print("sensor_volt = ");
   Serial.print(sensor_volt);
   Serial.print("V\t");
 
   Serial.print("R0 = ");
   Serial.println(R0);
 
   delay(2000);
}

/*
 * 2. Then, open the serial monitor of Arduino IDE. 
 * Write down the value of R0 and this needs to be used in the next program. 
 * 
 * Please node down the R0 after the reading stabilizes.
 * Replace the R0 below with value of R0 tested above. 
 * Expose the sensor to any one of the gas listed above.
 * 
*/
float RS_gas;       // Get value of RS in a GAS
float ratio;        // Get ratio RS_GAS/RS_air
void getGasSensor() {
  
   int sensorValue = analogRead(A0);
   sensor_volt=(float)sensorValue/1024*5.0;
   RS_gas = (5.0-sensor_volt)/sensor_volt; // omit *RL
 
   /*-Replace the name "R0" with the value of R0 in the demo of First Test -
    * Hear I use the global R0; then, R0 contains the value from the loop above (First Test)
    */
   ratio = RS_gas/R0;  // ratio = RS/R0 
   /*-----------------------------------------------------------------------*/
 
   Serial.print("sensor_volt = ");
   Serial.print(sensor_volt);
   Serial.print(" RS_ratio = ");
   Serial.print(RS_gas);
   Serial.print(" Rs/R0 = ");
   Serial.println(ratio);
//  Serial.println("");
 
//   delay(2000);
}

// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain
#include "DHT.h"

#define DHTPIN 15     // what digital pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

bool firstDHT11 = true;

float h, t, f, hif, hic;
void getDHT11() {
  if (firstDHT11){
    dht.begin();
    firstDHT11 = false;
  }
  // Wait a few seconds between measurements.
//  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");
  Serial.println("\n");
}

void displayLCD() {
   display.begin();
   display.clearDisplay();   //Apaga o buffer e o display
   display.setCursor(0,0);  //Seta a posição do cursor
//   display.println("Robuino");
   display.print("V:");
   display.print(sensor_volt,1);
   display.print(" R:");
   display.println(ratio,1);

   display.print("Hum: ");
   display.print(h,1);
   display.println(" %\n");
   //display.print("Temp: ");
   display.print(t,1);
   display.print("C ");
   display.print(f,1);
   display.println("F");
   display.print("Max:\n");
   display.print(hic);
   display.print("C ");
   display.print(hif);
   display.println("F");
   //display.println("\n");
}

