/*------------------------------------------------------------------------
  Simple ESP8266 test.  Requires SoftwareSerial and an ESP8266 that's been
  flashed with recent 'AT' firmware operating at 9600 baud.  Only tested
  w/Adafruit-programmed modules: https://www.adafruit.com/product/2282

  The ESP8266 is a 3.3V device.  Safe operation with 5V devices (most
  Arduino boards) requires a logic-level shifter for TX and RX signals.
  ------------------------------------------------------------------------*/

#include <Adafruit_ESP8266.h>
#include <SoftwareSerial.h>

#define ESP_RX   3
#define ESP_TX   2
#define ESP_RST  4
SoftwareSerial softser(ESP_RX, ESP_TX);

// Must declare output stream before Adafruit_ESP8266 constructor; can be
// a SoftwareSerial stream, or Serial/Serial1/etc. for UART.
Adafruit_ESP8266 wifi(&softser, &Serial, ESP_RST);
// Must call begin() on the stream(s) before using Adafruit_ESP8266 object.

#define ESP_SSID "PONESA-3G" // Your network name here
#define ESP_PASS "meiadeseda" // Your network password here

#define HOST     "www.adafruit.com"     // Host to contact
#define PAGE     "/testwifi/index.html" // Web page to request
#define PORT     80                     // 80 = HTTP default port

#define LED_PIN  7

void setup() {
  char buffer[150];
  String msg;

  // Flash LED on power-up
  pinMode(LED_PIN, OUTPUT);
  for(uint8_t i=0; i<13; i++) {
    digitalWrite(LED_PIN, HIGH); delay(50);
    digitalWrite(LED_PIN, LOW);  delay(100);
  }

  // This might work with other firmware versions (no guarantees)
  // by providing a string to ID the tail end of the boot message:
  
//  comment/replace this if you are using something other than v 0.9.2.4!
//  wifi.setBootMarker(F("Version:0.9.2.4]\r\n\r\nready"));
//  wifi.setBootMarker(F("Version:0.40.0.0]\r\n\r\nready"));

  softser.begin(9600); // Soft serial connection to ESP8266
  Serial.begin(9600);
  while(!Serial); // UART serial debug

  Serial.println(F("Adafruit ESP8266 Demo"));

//  // Test if module is ready
//  Serial.print(F("Hard reset..."));
//  if(!wifi.hardReset()) {
//    Serial.println(F("no response from module."));
////    for(;;);
//  }
//  Serial.println(F("OK."));

  Serial.print(F("Soft reset..."));
  if(!wifi.softReset()) {
    Serial.println(F("no response from module."));
//    for(;;);
  }
  Serial.println(F("OK."));

  Serial.print(F("Checking firmware version..."));
  wifi.println(F("AT+GMR"));
  if(wifi.readLine(buffer, sizeof(buffer))) {
    Serial.println(buffer);
//    wifi.find(); // Discard the 'OK' that follows
  } else {
    Serial.println(F("error"));
  }

  Serial.println("Altera modo para client/ap...");
  msg = "AT+CWMODE=1\r\n";
  sendData(msg, 2000, true);

  wifi.setTimeouts(5000,5000,5000,5000);
  Serial.println(F("Connecting to WiFi..."));
  if(wifi.connectToAP(F(ESP_SSID), F(ESP_PASS))) {

    // IP addr check isn't part of library yet, but
    // we can manually request and place in a string.
    Serial.println(F("OK\nChecking IP addr..."));
    wifi.println(F("AT+CIFSR"));
    if(wifi.readLine(buffer, sizeof(buffer))) {
      Serial.println(buffer);
      wifi.find(); // Discard the 'OK' that follows

      Serial.print(F("Connecting to host..."));
      if(wifi.connectTCP(F(HOST), PORT)) {
        Serial.print(F("OK\nRequesting page..."));
        if(wifi.requestURL(F(PAGE))) {
          Serial.println("OK\nSearching for string...");
          // Search for a phrase in the open stream.
          // Must be a flash-resident string (F()).
          if(wifi.find(F("working"), true)) {
            Serial.println(F("found!"));
          } else {
            Serial.println(F("not found."));
          }
        } else { // URL request failed
          Serial.println(F("error"));
        }
        wifi.closeTCP();
      } else { // TCP connect failed
        Serial.println(F("D'oh!"));
      }
    } else { // IP addr check failed
      Serial.println(F("error"));
    }
    wifi.closeAP();
  } else { // WiFi connection failed
    Serial.println(F("FAIL"));
  }
}

void loop() {
}

void sendData(String command, const int timeout, boolean debug)
{
  Serial.print ("Enviando: ");
  Serial.println (command);
  // Envio dos comandos AT para o modulo
  String response = "";
  softser.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())  {
    while (softser.available())    {
      // The esp has data so display its output to the serial window
      char c = softser.read(); // read the next character.
      response += c;
    }
  }
  if (debug)  {
    Serial.print("response ");
    Serial.println(response);
  }
}

