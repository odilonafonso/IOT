/*
 * send a humidity and temperature obtained for a DHT11 sensor
 * to a ESP8266 via serial port.
 * odilon cenamo. May/2016
 */
#include <SoftwareSerial.h>

// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include "DHT.h"

#define DHTPIN 8     // what digital pin we're connected to

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

#define RX 2
#define TX 3
#define UART_SERIAL 9600
#define UART_ESP8266 9600

SoftwareSerial esp8266(RX, TX, false);

void setup() {
   Serial.begin(UART_SERIAL);
   dht.begin();
}

#define MSG_FROM_MQTT "getTemperature..."
void loop() {
   String msgFromEsp8266 = readEsp8266();
   if (msgFromEsp8266 != "") {
      Serial.println("<" + msgFromEsp8266);
      String respToEsp8266 = getTemp();
      Serial.print(">" + respToEsp8266);
//      String x = "°";
//      for (int i=0; i<x.length(); i++){
//         Serial.print(x.charAt(i),HEX);
//         Serial.print("/");
//      }
//      Serial.println("");
      esp8266.print(respToEsp8266);      
   }
   delay(150);
}

/*
 * msg from esp8266
 */
#define TIMEOUT_ESP8266 5000
String readEsp8266(){
/*
 * serial with esp8266 must be reinitialized
 */
   esp8266.begin(UART_ESP8266);
   esp8266.setTimeout(TIMEOUT_ESP8266);
   String resp = "";
   if ( (resp = esp8266.readStringUntil(10)) != '\0' ) {
      if (resp.length() == 1) {
         Serial.print(resp.charAt(0),HEX);
      }
      Serial.print("length:"); Serial.println(resp.length());
//      Serial.print("{");
//      Serial.print(resp);
//      Serial.println ("}");
   }
   else {
      resp = "noMsgFromEsp8266";
   }
   esp8266.end();
   return resp;
}

String getTemp(){
   // Reading temperature or humidity takes about 250 milliseconds!
   // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
   float h = dht.readHumidity();
   // Read temperature as Celsius (the default)
   float t = dht.readTemperature();
   // Read temperature as Fahrenheit (isFahrenheit = true)
   float f = dht.readTemperature(true);
   
   // Check if any reads failed and exit early (to try again).
   if (isnan(h) || isnan(t) || isnan(f)) {
      return F("Failed to read from DHT sensor!\n");
   }
/*   
   // Compute heat index in Fahrenheit (the default)
   float hif = dht.computeHeatIndex(f, h);
   // Compute heat index in Celsius (isFahreheit = false)
   float hic = dht.computeHeatIndex(t, h, false);
*/
   String temp = ("Hum:") + String(h,0) + ("%");
   temp += (";") + String(t,0) + ("*C");
   temp += (";") + String(f,0) + ("*F");
   temp += ("\n");
//   temp += ("Heat index: ");
//   temp += (hic);
//   temp += (" *C ");
//   temp += (hif);
//   temp += (" *F");
   return temp;
}

