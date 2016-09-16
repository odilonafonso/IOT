// Programa de teste de comunicação Arduino-ESP8266 
// Gustavo Murta - 15/dez/2015
// Arduino 2009 - IDE 1.6.5
// ESP8266-ESP201 - AT version:0.50.0.0 - SDK version:1.4.0
// Baseado em http://arduino.stackexchange.com/questions/12500/esp8266-and-arduino-interfacing

#include <SoftwareSerial.h>

//RX pino 2, TX pino 3
#define RX 2
#define TX 3
#define RST 4

#define VELOCIDADE_SERIAL 9600
#define VELOCIDADE_ESP8266 9600
SoftwareSerial esp8266(RX, TX, false); 
// Arduino D2 => SoftSerial RX
// Arduino D3 => SoftSerial TX                           

void resetESP8266() {
  pinMode(RST, OUTPUT);
  digitalWrite(RST,LOW);
  delay(100);
  pinMode(RST, INPUT);
//  digitalWrite(RST, HIGH);
  Serial.println("Reset logico ESP... ");
}  

void setup()
{
  Serial.begin(VELOCIDADE_SERIAL);    // Baud Rate da Console IDE Arduino 
  resetESP8266();                     //tenta dar reset no módulo ESP8266
  esp8266.begin(VELOCIDADE_ESP8266);  // Baud Rate da Serial do ESP8266
}                                     // Condição ideal, os dois baud rates serem iguais 

void loop()
{
  if(esp8266.available())        // Verifique se o ESP8266 esta enviando mensagem 
  {
    while(esp8266.available())
    {
      // Se o ESP8266 enviar mensagem, sera mostrada na console da IDE Arduino 
      
      char c = esp8266.read(); // leia o proximo caracter
      Serial.write(c);
    }  
  }



  if(Serial.available())
  {    
    delay(50);                         // atraso para permitir sincronismo no envio das mensagens 
    String command="";
    
    while(Serial.available())          // leia o comando - caracter por caracter 
    {       
      command+=(char)Serial.read();    // leia um caracter 
    }  
    esp8266.println(command);         // envia o caracter da Console IDE Arduino para o ESP8266
    
  }
}
