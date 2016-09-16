/*
 * Sample based on WriteVoltage from ThingSpeak Library
 * write the color from CheerLights channel and write to a private channel
 * 
  WriteVoltage
  
  Reads an analog voltage from pin 0, and writes it to a channel on ThingSpeak every 20 seconds.
  
  ThingSpeak ( https://www.thingspeak.com ) is a free IoT service for prototyping
  systems that collect, analyze, and react to their environments.
  
  Copyright 2015, The MathWorks, Inc.
  
  Documentation for the ThingSpeak Communication Library for Arduino is in the extras/documentation folder where the library was installed.
  See the accompaning licence file for licensing information.
*/
/// ***********************************************************************************************************
// This example selects the correct library to use based on the board selected under the Tools menu in the IDE.
// Yun, Wired Ethernet shield, wi-fi shield, esp8266, and Spark are all supported.
// With Uno and Mega, the default is that you're using a wired ethernet shield (http://www.arduino.cc/en/Main/ArduinoEthernetShield)
// If you're using a wi-fi shield (http://www.arduino.cc/en/Main/ArduinoWiFiShield), uncomment the line below
// ***********************************************************************************************************
//#define USE_WIFI_SHIELD
#ifdef ARDUINO_ARCH_AVR
  // On Arduino:  0 - 1023 maps to 0 - 5 volts
  #define VOLTAGE_MAX 5.0
  #define VOLTAGE_MAXCOUNTS 1023.0
#endif

#ifdef ARDUINO_ARCH_ESP8266
   #define VOLTAGE_MAX 1.0
   #define VOLTAGE_MAXCOUNTS 1023.0
#endif

#ifdef SPARK
    #define VOLTAGE_MAX 3.3
    #define VOLTAGE_MAXCOUNTS 4095.0
#endif

/*
  *****************************************************************************************
  **** Visit https://www.thingspeak.com to sign up for a free account and create
  **** a channel.  The video tutorial http://community.thingspeak.com/tutorials/thingspeak-channels/ 
  **** has more information. You need to change this to your channel, and your write API key
  **** IF YOU SHARE YOUR CODE WITH OTHERS, MAKE SURE YOU REMOVE YOUR WRITE API KEY!!
  *****************************************************************************************/
unsigned long myChannelNumber = 140135;
const char * myWriteAPIKey = "L4Q9QIJZRPJ3GJDM";
#define COLOR_FIELD 6
#define VOLTAGE_FIELD 7
void writeColor(String color, int iColor) {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // Convert the analog reading 
  // On Arduino:  0 - 1023 maps to 0 - 5 volts
  // On ESP8266:  0 - 1023 maps to 0 - 1 volts
  // On Particle: 0 - 4095 maps to 0 - 3.3 volts
  float voltage = sensorValue * (VOLTAGE_MAX / VOLTAGE_MAXCOUNTS);

  // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
  // pieces of information in a channel.  Here, we write to field 6.
  ThingSpeak.writeField(myChannelNumber, VOLTAGE_FIELD, voltage, myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, COLOR_FIELD, iColor, myWriteAPIKey);
   Serial.print("ThingSpeak.writeField(myChannelNumber, VOLTAGE_FIELD, voltage, myWriteAPIKey)");
   Serial.print(" voltage:");Serial.println(voltage);
   Serial.print("ThingSpeak.writeField(myChannelNumber, COLOR_FIELD, color, myWriteAPIKey)");
   Serial.print(" color:");Serial.println(color);
  delay(10000); // ThingSpeak will only accept updates every 15 seconds.
}

