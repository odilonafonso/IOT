/**
 * @example TCPServer.ino
 * @brief The TCPServer demo of library WeeESP8266. 
 * @author Wu Pengfei<pengfei.wu@itead.cc> 
 * @date 2015.02
 * 
 * @par Copyright:
 * Copyright (c) 2015 ITEAD Intelligent Systems Co., Ltd. \n\n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version. \n\n
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <RobuinoWithoutSensor.h>
#include <SoftwareSerial.h>
#include "ESP8266.h"

#define PROMINI_BOARD
#ifdef PROMINI_BOARD
/*
 * 2,3 = SoftwareSerial ESP8266
 * 13 = status Led
 */
byte invPinButtons[] = {2,3,13,18,19,255};
#endif

RobuinoCore robuinoCore(invPinButtons);

/*
 * algumas variáveis globais para economizar memória e facilitar o uso
 */
bool modeAP = false;
bool ret8266;
String resp, str;
String ssidName = "";
String ssidPass = "";

//status do Robuino; utilizado para o display do status de conexão
#define NOAP                0
#define AP_CONNECTED        1
#define SERVER_AP_STARTED   2
#define STA_CONNECTED       3
#define SERVER_STA_STARTED  4
#define CLIENT_CONNECTED    5
#define CLIENT_DISCONNECTED 6
#define STATUS_LED 13

//RX pino 2, TX pino 3
#define APSSID "Majuino"
#define APPSW "21121966"
#define APSERVER_PORT 8000
#define ESP8266_BAUD 9600
#define SERIAL_BAUD  9600
#define RX 2
#define TX 3
#define RST 10

#define SSID        "PONESA-3G"   //"FELINA"   //"ModNet3"    //"PONESA-3G"   //"ITEAD"
#define PASSWORD    "meiadeseda"  //"00005555" //"1011121314" //"meiadeseda"  //"12345678"
#define SERVERPORT  90

SoftwareSerial serial8266(RX, TX, false);
ESP8266 wifi(serial8266);

void softwareReset() {
/*
 * modo de fazer reset programaticamente:
 * ligar o pino RST do ESP8266 com resistor 2K no GND, 1K no pino RST do ProMini
 * iniciar com LOW por algum tempo (DELAY_RST)
 * em seguida deixar com HIGH
 * 
 * esse delay deve ser testado melhor; pode ser que funcione com menos tempo
 */
#define RST 12
#define DELAY_RST 1
  pinMode(RST, OUTPUT);
  Serial.print("softRST");
  for (int i=0; i<1; i++) {
    Serial.print(".");
    digitalWrite(RST, LOW);
    delay(DELAY_RST);
    digitalWrite(RST, HIGH);
    delay(DELAY_RST);
  }
  Serial.println("");
//  Serial.println("restart()");
//  wifi.restart();
}

bool ret;
void setup(void)
{
serial8266.begin(9600);
    Serial.begin(9600);
    Serial.print("setup begin\r\n");

//    softwareReset();
    
    while (!wifi.setOprToStationSoftAP()) {
      softwareReset();
    }
    Serial.println("to station + softap ok");
 
    Serial.print("FW Version:");
    Serial.println(wifi.getVersion().c_str());
      
//    if (wifi.setOprToStationSoftAP()) {
//        Serial.print("to station + softap ok\r\n");
//    } else {
//        Serial.print("to station + softap err\r\n");
//    }
 
    while (!wifi.joinAP(SSID, PASSWORD)) {
    }
//    else {
//        Serial.print("Join AP failure\r\n");
//    }

    Serial.print("Join AP success\r\n");
    Serial.print("IP: ");
    Serial.println(wifi.getLocalIP().c_str());    
    
    if (wifi.enableMUX()) {
        Serial.print("multiple ok\r\n");
    } else {
        Serial.print("multiple err\r\n");
    }

    Serial.print(F("server ")); Serial.print(SERVERPORT);
    ret = wifi.startTCPServer(SERVERPORT);
    Serial.println( ret ? " ok" : " err" );

//    ret = wifi.startTCPServer(SERVERPORT);
//    if (ret) {
//        Serial.print("start tcp server ok\r\n");
//    } else {
//        Serial.print("start tcp server err\r\n");
//    }
    
    if (wifi.setTCPServerTimeout(10)) { 
        Serial.print("set tcp server timeout 10 seconds\r\n");
    } else {
        Serial.print("set tcp server timeout err\r\n");
    }
    
    Serial.print("setup end\r\n");
}

/*
bool originalSend (uint8_t mux_id, uint8_t *buffer, uint32_t len) {
   return wifi.send(mux_id, buffer, len);
}
/*
void waitESP8266(int timeout){
    unsigned long start = millis();
//    Serial.print("Wait");Serial.println(timeout);
//    String buff;
Serial.println("waitESP8266");
    while (millis() - start < timeout) {
      while(serial8266.available() > 0) {
         char c = (char)serial8266.read();
//         buff += c;
         Serial.print(c);
      }
    }
//    Serial.println(buff);
    Serial.println("");
}
*/

/*
 * com o webserver iniciado em modo AP, o usuario deverá seguir os procedimentos abaixo:
 * 
 * 1. Para informar a rede wifi desejada, password e opcionalmente IP desejado:
 * http://192.168.4.1:8000/wifi/?sid=SSID&psw=PASSWORD e opcionalmente &ip=IPDESEJADO
 * 
 * 2. Após informar os parametros acima, o usuario deverá executar novamente uma
 *    chamada ao webserver em modo AP da seguinte forma:
 * http://192.168.4.1:8000/con
 *
 */
String getWifiConnection(char *buffer) {
   String x = "";
   if (strstr(buffer, "/con")) {
#ifndef ENVIRONMENT_PRODUCTION
      Serial.println(F("con.."));
#endif
//      return connectStation(false);
      return "conectou";
   }
   else if (strstr(buffer, "/wifi")) {
#ifndef ENVIRONMENT_PRODUCTION
      Serial.println(F("wifi.."));
#endif
      ssidName = getArgumentValue(buffer, F("sid"));
      ssidPass = getArgumentValue(buffer, F("psw"));
      
      if (!ssidName.equals("") && !ssidPass.equals("")) {
//         str=F("AT+CIFSR");             //get station MacAddress
//         resp = sendData(str, 5000, true, "OK");
         resp = wifi.getLocalIP();
         int m = resp.indexOf("STAMAC,\"");
         String MACStation = "";
         if ( m >= 0 ) {
            MACStation = resp.substring(m+8, m+25);
         }
         
         x += F("Anote Mac:");
         x += MACStation;
         x += F("<a href=\"http://192.168.4.1:");
         x += APSERVER_PORT;
         x += F("/con\">Connect</a>"); 
      }
      else
         x += F("informe sid e psw...");
   }
   else
      x += F("informe sid e psw...");
#ifndef ENVIRONMENT_PRODUCTION
      Serial.println( x );
#endif
   return (x);
}

String processMsg(char *buffer){
   if (modeAP)  {
     return ( getWifiConnection(buffer) );
   }
   else {
     return ( robuinoCore.getClientMsg(buffer) );
   }
}

uint8_t statusConnection = 3;
long tictacMillis = 0;
#define TicTac 10555               //ms to check tacBoom
void tictacCheck(){
   if ( (millis() - tictacMillis) > TicTac) {
      for (int i=0; i<statusConnection; i++) {
         pinMode(STATUS_LED, OUTPUT);
         digitalWrite(STATUS_LED, HIGH);
         delay(150);
         digitalWrite(STATUS_LED, LOW);
         delay(150);
      }
      tictacMillis = millis();
   }
}

void loop(void)
{
#define BUFFER_SIZE 128
    uint8_t buffer[BUFFER_SIZE] = {0};
    uint8_t mux_id;
    uint32_t len = wifi.recv(&mux_id, buffer, sizeof(buffer), 100);
    if (len > 0) {
        Serial.print("Status:[");
        Serial.print(wifi.getIPStatus().c_str());
        Serial.println("]");
        
        Serial.print("Received from :");
        Serial.print(mux_id);
        Serial.print("Len:");Serial.print(len);
        Serial.print("[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
        }
        Serial.print("]\r\n");
//      if (len == BUFFER_SIZE)
//         waitESP8266(2000);  //recebe restante dos caracteres do client http
        
            ret = sendResponseMsg(processMsg((char*)buffer));
//            ret = wifi.send(mux_id, buffer, len);
        if (ret) {
            Serial.print("send back ok\r\n");
        } else {
            Serial.print("send back err\r\n");
        }
        
        if (wifi.releaseTCP(mux_id)) {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" ok");
        } else {
            Serial.print("release tcp");
            Serial.print(mux_id);
            Serial.println(" err");
        }
        
        Serial.print("Status:[");
        Serial.print(wifi.getIPStatus().c_str());
        Serial.println("]");
    }
   tictacCheck();
}

#define MAJUINO_HOST F("majuino.ddns.net")    //"majuino.ddns.net";

#define SHORT_RESPONSE
uint8_t muxId;
bool sendResponseMsg(String bodyMsg){
//   Serial.println(bodyMsg);
//   if (modeAP)
//      return ( sendDataToClient( bodyMsg ) );

#ifdef SHORT_RESPONSE
   sendDataToClient( F("HTTP/1.1 200 OK\r\n") );
   sendDataToClient( F("Content-Type: text/html\r\n") );
   if (modeAP) {
      return sendDataToClient( "\r\n" + bodyMsg + "\r\n");
   }
   sendDataToClient( F("Transfer-Encoding: chunked\r\n\r\n") );
   sendChunkData( F("<html><body>") );
   sendChunkData( bodyMsg );
   sendChunkData( F("</body></html>") );
   return sendDataToClient( F("0\r\n\r\n") );
#else
   sendDataToClient( F("HTTP/1.1 200 OK\r\n") );
//   sendDataToClient( F("Connection: close\r\n") );
   sendDataToClient( F("Content-Type: text/html\r\n") );
   sendDataToClient( F("Transfer-Encoding: chunked\r\n\r\n") );
//   sendChunkData( F("<!DOCTYPE HTML>") );
   sendChunkData( F("<html><body>") );
//   sendDataToClient( F("<head><script>function On(p){") );
//   sendDataToClient( F("document.f1.op.value=4; document.f1.pin.value=p; document.f1.submit();}") );
//   sendDataToClient( F("</script></head>") );
//   sendDataToClient( F("<body>") );

#define SENDFORM    //send form buttons
#ifdef SENDFORM
   sendChunkData( F("<form name='f1' method='get' action='http://") );
   sendChunkData( MAJUINO_HOST );
   sendChunkData( ":" + String(webserverPort) + "'>" );
//   montaButtons();
   montaPinOptions();
   montaOpOptions();
//   sendDataToClient( F("<br><input type='radio' name='op' value='1'>On/Off<br>") );
//   sendDataToClient( F("<input type='radio' name='op' value='7' checked>Gate<br>") );
//   sendDataToClient( F("pin<input type='text' name='pin'/><br>") );
   sendChunkData( F("Delay<input type='text' name='delay' size='5' value='200'/>") );
   sendChunkData( F("<input type='hidden' name='user' value='uRob'/>") );
   sendChunkData( F("<input type='hidden' name='pass' value='pRob'/>") );
   sendChunkData( F("<input type='submit' value='Go'></form>") );
#endif

//   sendDataToClient( F("</form>") );
//   sendDataToClient( F("<h1>") );
   sendChunkData( "<h1>" + bodyMsg + "</h1>");
   sendChunkData( F("</body></html>") );
//   sendChunkData( bodyMsg );
   return sendDataToClient( F("0\r\n\r\n") );
#endif
}

bool sendChunkData(String data){
   int len = data.length();
   char newBuff[len+6];
   String hex = (len > 15 ? String(len,HEX) : '0' + String(len,HEX));
#ifndef ENVIRONMENT_PRODUCTION
   Serial.print(len,HEX);Serial.print(" ");Serial.println(data);
#endif
   newBuff[0] = hex.charAt(0);
   newBuff[1] = hex.charAt(1);
   newBuff[2] = '\r';
   newBuff[3] = '\n';
   for (int i=0; i<data.length(); i++)
      newBuff[i+4] = data.charAt(i);
   newBuff[len+4] = '\r';
   newBuff[len+5] = '\n';
   return (wifi.send(muxId, (uint8_t*)newBuff, len+6));
}

bool sendDataToClient(String data){
#ifndef ENVIRONMENT_PRODUCTION
   Serial.print(data);
#endif
   char newBuff[data.length()];
   for (int i=0; i<data.length(); i++)
      newBuff[i] = data.charAt(i);
   return (wifi.send(muxId, (uint8_t*)newBuff, data.length()));
}


