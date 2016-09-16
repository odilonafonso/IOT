#include <SoftwareSerial.h>

#define RX 2
#define TX 3

SoftwareSerial Serial2(RX,TX);

void setup()
{
      Serial2.begin(9600);
      Serial.begin(9600);
}


void loop()
{
     while (Serial2.available()) { Serial.write(Serial2.read()); }
     while (Serial.available()) { Serial2.write(Serial.read()); }
}


