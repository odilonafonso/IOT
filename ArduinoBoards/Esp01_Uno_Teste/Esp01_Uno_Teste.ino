/*
 * comentarios Odilon 
 * este teste utiliza comunicação com o ESP8266 SEM a biblioteca ESP8266.h
 * a biblioteca ESP8266.h está sendo sub-utilizada,
 * apenas para startar um tcpserver:
 *   setOprToStation()
 *   enableMUX()
 *   setTCPServerTimeout()
 *   getIPStatus()
 * utiliza apenas a SoftwareSerial
 * o firmware deve ser o AT
 * 
 */
#include <SoftwareSerial.h>
//#include <ESP8266.h>

// Programa: Teste do ESP8266, troca de velocidade
// Autor : MNI
// O Módulo eh bastante sensível a interferências, talvez ruidos na rede, ou
// Baixa amperagem, as vezes funciona as vezes nao
// Se utilizar energia externa, o GND deve ser conectado ao Arduino
// Alguns modulos Mesmo ajustando a velocidade, não fica gravado na EEPROM, ou seja, volta ao padrão de fábrica


/*
 * modulos esp-01 adquiridos
 * 18-fe-34-da-b9-87 - ip 192.168.2.155 - porta 90 - eBay
 * AT wersion:0.24.0.0(Jun  5 2015 16:27:06)
 * SDK version:1.1.1
 * 25Q80ASSIG ESP8266EX
 * 
 * 18-fe-34-da-65-1a - ip 192.168.2.166 - porta 91 - eBay
 * AT versioo:0.25.0.0(Jtn  5 2015 06:27:16)
 * SDK version:1.r.2
 * 25Q80ASSIG ESP8266EX
 * 
 * 5c-cf-7f-0b-06-49 - ip 192.168.2.156 - porta 8090 - ESP_0B0649 - ML1
 * AT wersion:
 * SDK version:
 * 25Q80ASSIG ESP8266EX
 *
 * 5c-cf-7f-0b-03-3f - ip 192.168.2.177 - porta 8091 - ESP_0B033F - ML2
 * AT wersion:0.40.0.0(Aug  8 2015 14:45:48)
 * SDK version:13c
 * 25Q80ASSIG ESP8266EX
 * 
 * Modulo NodeMCU Esp12e placa de desenvolvimento
 * 18-FE-34-F3-A0-C8 - ip 192.168.2.188 - ML
 */
//#include <Ethernet.h>
//#include <SPI.h>

//RX pino 2, TX pino 3
#define RX 2
#define TX 3
#define RST 4

#define SSID      "PONESA-3G"
#define PASSWORD  "meiadeseda"

SoftwareSerial esp8266(RX, TX, false);

#define PINTEST 8
#define LED_PIN 9
#define DEBUG true
#define VELOCIDADE 9600
#define VELOCIDADE_SERIAL 9600

/*
 * quebra-galho para setar a porta de teste
 * quando estiver funcionando, vai ler o Mac(ou IPStation) e associar a porta
 */
String IPStation;
int webserverPort;
//#define BOARD 18-fe-34-da-65-1a
//#define HOST_NAME   "192.168.2.155"   //"172.16.5.12"
//#define WEBSERVER_PORT 90

//#define BOARD 18-fe-34-da-65-1a - ip 192.168.2.166 - porta 91 - eBay
//#define WEBSERVER_PORT 91

//#define BOARD 5c-cf-7f-0b-06-49 - ip 192.168.2.156 - porta 8090 - ESP_0B0649 - ML1
//#define WEBSERVER_PORT 8090

//#define BOARD 5c-cf-7f-0b-03-3f - ip 192.168.2.177 - porta 8091 - ESP_0B033F - ML2
//#define HOST_NAME   "192.168.2.177"   //"172.16.5.12"
//#define WEBSERVER_PORT 8091

//EthernetServer server(WEBSERVER_PORT);

int nCommands = 0;

boolean resetESP8266() {
  digitalWrite(RST,LOW);
  delay(100);
  pinMode(RST, OUTPUT);
  digitalWrite(RST, HIGH);
//  delay(3000);
  Serial.println(F("Reset logico ESP... "));
  sendData("AT+RST", 1000, DEBUG);
  return true;
}  

void piscaLed(int led, int n=7){
  pinMode(led, OUTPUT);
  for(uint8_t i=0; i<n; i++) {
    digitalWrite(led, HIGH); delay(100);
    digitalWrite(led, LOW);  delay(100);
  }
}

String readSerial(String msg=""){
  Serial.setTimeout(3000);
  if (msg != "")
    Serial.print(msg);
  String resp = Serial.readStringUntil(10);
  if (resp != ""){
    Serial.print("{");
    Serial.print(resp);
    Serial.println ("}");
  }
  return resp;
}

void setup()
{
  String resp, msg;
  long velo[] = { 9600,14400,28800,19200,38400,57600,76800,76800+4800,76800+2*4800,76800+3*4800,
96000,96000+4800,96000+2*4800,96000+3*4800,115200,115200+4800,115200+2*4800 };
//  long velo[] = { 96000,96000+4800,96000+2*4800,96000+3*4800,115200,115200+4800 };
//  115200+2*4800,115200+3*4800,115200+4*4800,115200+5*4800 };
  long velocidade = VELOCIDADE;

  Serial.begin(VELOCIDADE_SERIAL);
  //inicia esp8266 com a velocidade que está gravada no módulo;
  esp8266.begin(velocidade);

  // Flash RST low(reset) left HIGH on power-up
  resetESP8266();
//  piscaLed(RST, 1);
  // Flash LED on power-up
  piscaLed(PINTEST);
  piscaLed(LED_PIN);

  resp = readSerial("troca velocidade ? (s/n)");
  if (resp.indexOf("s") == 0 || resp.indexOf("s") == 0){
    int len = sizeof(velo)/sizeof(long);
    for (int i=0; i<len; i++) {
      velocidade = velo[i];

      //fecha serial anteriormente aberta com a velocidade anterior
//      Serial.println(esp8266.stopListening()==true?"stopListening OK!":"stopListening Fail :(");
      
//      sendData("AT+RST", 3000, DEBUG);
      sendData("AT", 3000, DEBUG);
      sendData("AT+CWMODE_CUR?", 5000, DEBUG);
      sendData("AT+GMR", 5000, DEBUG);
      Serial.print(F("Testando velocidade: "));
      Serial.println(velocidade);
      esp8266.end();
      esp8266.begin(velocidade); 
      msg = "AT+UART_CUR=";
      msg += velocidade;
      msg += ",8,1,0,0"; 
      sendData(msg, 3000, DEBUG);
//
      Serial.println(F("Tecle ok quando estiver vendo a resposta..."));
      delay(1000);
      String s = readSerial();
      Serial.println(s);
      if (s.indexOf("ok") >= 0) {
        Serial.println("achou...");
        break;
      }
    }
  }

  Serial.println(" ");
  Serial.println("-----------------------");
//  sendData("AT+RST", 3000, DEBUG);
  sendData("AT", 2000, DEBUG);
  Serial.print(F("Baud rate para "));
  Serial.println(velocidade);
  msg = "AT+UART_CUR=";
  msg += velocidade;
  msg += ",8,1,0,0"; 
  /* os comandos abaixo estragam o módulo...??? bem estranho, mas tô seguindo...
   *  depois tem de regravar firmware
  String msg = "AT+CIOBAUD=";
  msg += velocidade;
  */
  sendData(msg, 3000, DEBUG);

  Serial.print("Versao em ");
  Serial.println(velocidade);
  esp8266.begin(velocidade); 
  sendData("AT+GMR", 2000, DEBUG);
  
  sendData(F("AT+CWMODE_DEF?"), 3000, DEBUG);

  Serial.println(F("Altera modo para client/ap..."));
  msg = "AT+CWMODE_CUR=3";
  sendData(msg, 3000, DEBUG);

  sendData(F("AT+CWMODE_CUR?"), 3000, DEBUG);

  sendData(F("AT+CWDHCP?"), 3000, DEBUG);
  
  msg = "AT+CWLAP";
  Serial.println(F("Listando redes disponiveis..."));
  sendData(msg, 7000, DEBUG);

  Serial.println(F("conectando com wifi.."));
  msg = "AT+CWJAP_CUR=\"";
  msg += SSID;
  msg += "\",\"";
  msg += PASSWORD;
  msg += "\""; 
  sendData(msg, 7000, DEBUG);

  Serial.println(F("Status wifi.."));
  msg = "AT+CIPSTA?"; 
  sendData(msg, 3000, DEBUG);

  msg = "AT+CWJAP_CUR?";
  Serial.println(F("rede conectada..."));
  sendData(msg, 3000, DEBUG);
/*
  msg = "AT+CIPSTAMAC_CUR?";
  Serial.println(F("Mac address..."));
  sendData(msg, 3000, DEBUG);
*/
/*
  msg = "AT+CIPSTA_CUR?";
  Serial.println("IP address...");
  sendData(msg, 3000, DEBUG);
*/
  msg = "AT+CIFSR";
  Serial.println(F("Get local IP address..."));
  msg = sendData(msg, 3000, DEBUG);
  int i = msg.indexOf("+CIFSR:STAIP,\"");
  if ( i >= 0 ) {
    for (i+=14 ; msg.charAt(i) != '"'; i++) {
      IPStation += msg.charAt(i);
    }
  }
  if (IPStation.equals("192.168.2.155"))
    webserverPort = 90;
  if (IPStation.equals("192.168.2.166"))
    webserverPort = 91;
  if (IPStation.equals("192.168.2.156"))
    webserverPort = 8090;
  if (IPStation.equals("192.168.2.177"))
    webserverPort = 8091;
  Serial.print("IPStation:"); Serial.print(IPStation);
  Serial.print(", port:"); Serial.println(webserverPort);

/*
  msg = "AT+CIPSTART=\"TCP\",\"";
  msg += HOST_NAME;
  msg += "\",";
  msg += WEBSERVER_PORT;
  Serial.println(F("Establish TCP connection..."));
  sendData(msg, 5000, DEBUG);
*/
  msg = "AT+CIPSTATUS";
  Serial.println(F("Check network connection status..."));
  sendData(msg, 5000, DEBUG );

  startServer();
}


void startServer(){
#define SERVER_TIMEOUT 11
   String str;
//   ESP8266 wifi(esp8266);

   sendData("AT+GMR", 2000, DEBUG);

   str=F("AT+CWMODE_DEF=1");            //esp8266.setOprToStation()
   sendDataTarget(str, 5000, true, "OK");
   /*
    if (wifi.setOprToStation()) {
        Serial.print("to station ok\r\n");
    } else {
        Serial.print("to station err\r\n");
    }
   */

   str=F("AT+CIPMUX=1");            //enableMux()
   sendDataTarget(str, 3000, true, "OK");
   /*
    if (wifi.enableMUX()) {
        Serial.println(F("enable multiple ok"));
    } else {
        Serial.print(F("enable multiple err"));
    }
   */

    Serial.println(F("Start server..."));
    str = "AT+CIPSERVER=1,";
    str += webserverPort;
    sendData(str, 3000, DEBUG);

   str=F("AT+CIPSTO=");         //setTCPServerTimeout(int timeout)
   str += SERVER_TIMEOUT;
   sendDataTarget(str, 3000, true, "OK");
   /*
    if (wifi.setTCPServerTimeout(10)) { 
        Serial.println(F("set tcp server timeout 10 seconds"));
    } else {
        Serial.println(F("set tcp server timeout err"));
    }
   */

//String  getIPStatus (void) : Get the current status of connection(UDP and TCP). 
//    Serial.println (wifi.getIPStatus());
   str=F("AT+CIFSR");             //getLocalIP()
   sendDataTarget(F("AT+CIFSR"), 5000, true, "OK");
//String  getLocalIP (void) : Get the IP address of ESP8266. 
//    Serial.println (wifi.getLocalIP());
}

//Nada faz
void loop() {
  String command, message;
  digitalWrite(PINTEST, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);              // wait for a second
  digitalWrite(PINTEST, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);
  if ( (message = recvHTTP(300, true)) != "") {
    Serial.print("messageHttp:"); Serial.println(message);
    replyHTTP("recebido!");
    closeTCP(0);
  }
  while ( true ) {
    command = "";
    if (nCommands == 20){
      nCommands = 0;
      Serial.println("*");
    }
    if (nCommands == 0)
      message = "waiting for command";
    else
      message = ".";
    if ( (command = readSerial(message)) == String(10) || command == "") {
      ++nCommands;
      break;
    }
    sendData(command, 3000, DEBUG);
    nCommands = 0;
  }
  
}

void replyHTTP(String message){
  static int cnt=0, id=0; 
  int total_len=0;
  char msg[100];
  char *hdr[] ={ 
    "HTTP/1.1 200 OK\r\n",
    "Content-Type: text/html\r\n",
    "Connection: close\r\n", 
    NULL,  // content length string
    NULL,  // content string
    NULL   // terminating null
  };

  digitalWrite(LED_PIN, 1);  // turn on led

  cnt++;
  int i;
  for (i=0; i<message.length() && i<100; i++)
    msg[i] += message.charAt(i);
  msg[i] = '\0';
  sprintf(msg,"<p>Counter Value = %d</p>\r\n",cnt);
  hdr[4] = msg;  // counter value
  total_len=strlen(msg);
  hdr[3] = msg+total_len+1;  // content length
  sprintf(hdr[3],"Content-Length: %d\r\n\r\n",total_len);
  Serial.println("message:"); Serial.println(message);
  Serial.println(String(hdr[3]));  
  total_len=0;
  for(int i=0; hdr[i] != NULL; i++)  // get total length of data to send
    total_len+=strlen(hdr[i]);

  // set channel and length of transfer
  sprintf(msg,"AT+CIPSEND=%d,%d",id,total_len); 
  sendData(msg, 3000, DEBUG);

  // wait for '>' and send data
  if( waitcmd( ">", 50 ) ) {
    for(int i=0; hdr[i]!=NULL; i++)
      esp8266.print(hdr[i]);

    waitcmd("Unlink", 150);  // after sending wait for unlink
  }

  digitalWrite(LED_PIN, 0);  // turn OFF led
}

/*
 * funçao bem interessante;
 * wait for string up to timeout ms.
 * called after sending cmd to esp
 */
int waitcmd(char *s, unsigned short timeout) {
  unsigned long tm=millis();
  while( 1 ) {
    if( millis() - tm >= timeout ) {
      Serial.print(F("TimedOutWaiting4 "));
      Serial.println(s);
      return 0;
    }
    if (esp8266.find(s)) {
      Serial.println(s);
      return 1;
    }
  }
}

void closeTCP(int mux_id) {
    String str = F("AT+CIPCLOSE=");     //releaseTCP(uint8_t mux_id)
    str += mux_id;
    sendData(str, 2000, true);
}
String sendDataTarget(String command, const int timeout, boolean debug, String target){
   return sendData(command, timeout, debug);
}
String sendData(String command, const int timeout, boolean debug)
{
  if (debug) {
    Serial.print ("Enviando: {");
    Serial.print (command);
    Serial.println ("}");
  }
  esp8266.println(command);
  return recvData(timeout, debug);
}

String recvData(const int timeout, boolean debug) {
  // Envio dos comandos AT para o modulo
  String response = "";
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      // The esp has data so display its output to the serial window
      char c = esp8266.read(); // read the next character.
      response += c;
    }
  }
  if (debug)
  {
    Serial.print("response:");
    Serial.print("{");
    Serial.print(response);
    Serial.println ("}");
  }
  return response;
}

String recvHTTP(const int timeout, boolean debug) {
#define MAX_HTTP_LENGTH 128
  String httpResp = "";
  long int time = millis();
  int httpLen = 0;
  bool crlf = false;
  Serial.println("recvHttp");
  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      // The esp has data so display its output to the serial window
      char c = esp8266.read(); // read the next character.
      if (crlf == false)
        if (c == '\r' || c == '\n') {
          crlf = true;
          break;
        }
      if (++httpLen >= MAX_HTTP_LENGTH)
        break;
      httpResp += c;
    }
  }
  if (debug && httpResp != "")
  {
    Serial.print("http:");
    Serial.print("{");
    Serial.print(httpResp);
    Serial.println ("}");
  }
  return httpResp;
}


