/**
 * @example ESP8266_ATCommands
 * @brief The ESP8266 AT Commands Utility with SoftwareSerial library
 * @author OdilonCenamo<odilon@cenamo.net> 
 * @date 2015.12
 * 
 * @par Copyright:
 * Copyright (c) 2015 Cenamo Software
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


/*
 * Como utilizar:
 * =============
 * 
 * 1.o Ligações.
 * ============
 * O TX do ESP8266 deve ser ligado ao pino definido como RX do Arduino
 * ver #define RX acima
 * O RX do ESP8266 deve ser ligado ao pino definido como TX do Arduino
 * ver #define TX acima
 * (*) se o arduino for de 3.3V essa ligação pode ser direta;
 *     se for de 5V, montar como abaixo 
 *     
 *     GND ---2K---.---1k--- TX (Arduino)
 *                 |
 *                 |
 *                R X (ESP8266)
 *                
 * Os exemplos de ligação que encontrei recomendam usar resistores de 10k e 5k
 * respectivamente no lugar dos de 2k e 1K
 * Como eu não tenho, usei da forma acima e funciona.
 * 
 * Os GND's devem ser interligados
 * O ESP8266 deve ser alimentado com 3.3v amperagem acima de 300ma
 * (provavelmente o Arduino nao será suficiente para fornecer a amperagem necessaria)
 * 
 * O RST de alguns ESP8266 devem ser aterrados no restart fisico e depois colocados em HIGH;
 * Para isso, coloque o RST no pino definido com RST do Arduino
 * ver #define RST acima
 * A loop piscaLed(led, times) serve para isso; eu estou usando para piscar um led, não
 * precisei conectar o RST;
 * Estou deixando o RST flutuando e não estou tendo problemas
 * 
 * 2.o Comandos.
 * ============
 * No setup são emitidos alguns comandos para teste, em seguida o sketch entrará em modo
 * interativo com o usuario.
 * A partir dai você poderá enviar comandos pelo monitor serial.
 * 
 * Os seguintes comandos são válidos:
 * 
 * "test"
 * para executar novamente os comandos enviados quando o sketch entra 
 * 
 * "webport,timeout"
 * executa os comandos para acionar webserver na porta port
 * tenta receber solicitação de cliente http
 * envia mensagem padrão de resposta ("OK!!")
 *  
 * "uart=nnnnn"
 * Muda a velocidade da serial que está acessando o ESP8266 (no sketch)
 * não tem nada a ver com a velocidade que está gravada no firmware do ESP8266
 * Esse comando é para verificar se os comandos de teste estão funcionando
 * Se na entrada do sketch os comandos de teste mostrarem caracteres estranhos,
 * a velocidade pode estar errada (torça para isso, caso contrário é defeito)
 * Use-o até descobrir qual a velocidade com que o ESP8266 está funcionando melhor
 * 
 * Comandos AT;
 * Entre com o comando AT desejado.
 * 
 * "help"
 * Teclando help é mostrada uma lista de comandos AT válidos
 * 
 * Tecle "nnxxx...xxx"
 * Onde:
 * nn é o numero do comando de acordo com a lista de comandos exibida com "help"
 * xx...xx é o complemento do comando, caso o mesmo necessite.
 * Ex. 01 - é o comando AT - não precisa complemento
 *     11 - AT+CWLAP - não precisa complemento
 *     09 - CWJAP_CUR - deve ser complementado,
 *     09? - CWJAP_CUR? - verifica se há uma conexão wifi ativa
 *     09="ssid","senha" - CWJAP="ssid","senha" - tenta conectar-se a uma rede wifi
 *     06=9600,8,1,0,0 - AT+UART_CUR=9600,8,1,0,0 - seta a velocidade atual do ESP8266
 * 
 * IMPORTANTE:
 * ==========
 * Caso você queira mudar a velocidade do ESP8266 numa sessao de testes, 
 * o comando a ser utilizado é o AT+UART_CUR, por exemplo:
 * 
 * AT+UART_CUR=19200,8,1,0,0 (*) consulte o manual para ver os detalhes deste comando
 * 
 * Em seguida, troque o uart do sketch arduino, com o comando:
 * 
 * uart=19200
 * 
 * (***) Para gravar a velocidade no firmware, utilize:
 * 
 * AT+UART_DEF=19200,8,1,0,0
 * 
 * **** ATENÇÃO ****
 * =================
 * Existem alguns comandos que você deve evitar, ou utilizar com muito cuidado.
 * Estão marcados com (*)
 * 
 * AT+GSLP(*)      // Enter deep-sleep mode
 * AT+RESTORE(*)   // Factory Reset
 * AT+SLEEP(*)     // Sleep mode
 * AT+RFPOWER(*)   // Set maximum value of RF TX Power
 * AT+RFVDD(*)     // Set RF TX Power according to VDD33
 */

/*
 * Espressif Systems ESP8266 AT Instruction Set
 * Retirado de: 4A-ESP8266__AT Instruction Set__EN_v1.4
 * http://bbs.espressif.com/viewtopic.php?f=51&t=1022
 * 
 * Tem um monte de comandos comentados;
 * Isso foi feito propositalmente para economizar memória 
 * Os comandos comentados comentados são os que menos devem ser utilizados,
 * em parte por serem de uso restrito, ou por serem comandos @deprecated
 * Basta descomenta-los para que possam ser normalmente mostrados e/ou utilizados
 * Outra forma de economizar memoria seria retirar "AT+" da tabela e incluir 
 * na hora de mostrar no help e quando for montar o comando.
 * 
 */

/* 
 * Ver sample Timer_Interrupt-sample
 * avr-libc library includes
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#define STATUS_LED 13

#include <SoftwareSerial.h>
#include <ESP8266.h>
#include <string.h>

//RX pino 2, TX pino 3
#define RX 2
#define TX 3
#define RST 4
#define DEBUG true
#define TIMEOUT_SERIAL 21000
#define TIMEOUT_ESP8266 5000
#define UART_ESP8266 9600
#define UART_SERIAL 9600


char* atCommands[] = {
"*Basic AT",    //Basic AT Command Set
"AT",           // Test AT startup
"ATE",          // AT commands echo or not
"RST",       // Restart module
"GMR",       // View version info
//"GSLP(*)",   // Enter deep-sleep mode
//"RESTORE(*)",// Factory Reset
//"UART",      // UART configuration, [@deprecated]
"UART_CUR",  // UART current configuration
"UART_DEF",  // UART default configuration, save to flash
//"SLEEP(*)",  // Sleep mode
//"RFPOWER(*)",// Set maximum value of RF TX Power
//"RFVDD(*)",  // Set RF TX Power according to VDD33
"*WiFi Functions",//WiFi Functions Overview
//"CWMODE",    // Wi-Fi mode(sta/AP/sta+AP), [@deprecated]
"CWMODE_CUR",// Wi-Fi mode(sta/AP/sta+AP) Won’t save to Flash
"CWMODE_DEF",// Wi-Fi default mode(sta/AP/sta+AP) Save to Flash
//"CWJAP",     // Connect to AP, [@deprecated]
"CWJAP_CUR", // Connect to AP, won’t save to Flash
"CWJAP_DEF", // Connect to AP, save to Flash
"CWLAP",     // Lists available APs
"CWQAP",     // Disconnect from AP
//"CWSAP",     // Set configuration of ESP8266 soft-AP[@deprecated]
"CWSAP_CUR", // Set configuration of ESP8266 soft-AP Won’t save to Flash.
"CWSAP_DEF", // Set configuration of ESP8266 soft-AP Save to Flash.
"CWLIF",     // Get station’s IP which is connected to ESP8266 soft-AP
//"CWDHCP",    // Enable/Disable DHCP, [@deprecated]
"CWDHCP_CUR",// Enable/Disable DHCP, won’t save to Flash
"CWDHCP_DEF",// Enable/Disable DHCP, save to Flash
"CWDHCPS_CUR",// Set IP range of DHCP server, won’t save to Flash
"CWDHCPS_DEF",// Set IP range of DHCP server, save to Flash
"CWAUTOCONN",// Connect to AP automatically when power on
"*TCP/IP",       // TCP/IP Related AT Commands
//"CIPSTAMAC", // Set MAC address of ESP8266 station [@deprecated]
"CIPSTAMAC_CUR",// Set MAC address of ESP8266 station Won’t save to Flash.
"CIPSTAMAC_DEF",// Set MAC address of ESP8266 station Save to Flash.
//"CIPAPMAC",  // Set MAC address of ESP8266 soft-AP [@deprecated]
"CIPAPMAC_CUR",// Set MAC address of ESP8266 soft-AP Won’t save to Flash.
"CIPAPMAC_DEF",// Set MAC address of ESP8266 soft-AP Save to Flash.
//"CIPSTA",    // Set IP address of ESP8266 station, [@deprecated]
"CIPSTA_CUR",// Set IP address of ESP8266 station Won’t save to Flash.
"CIPSTA_DEF",// Set IP address of ESP8266 station Save to Flash.
//"CIPAP",     // Set IP address of ESP8266 soft-AP, [@deprecated]
"CIPAP_CUR", // Set IP address of ESP8266 soft-AP Won’t save to Flash.
"CIPAP_DEF", // Set IP address of ESP8266 soft-AP Save to Flash.
"CIPSTATUS", // Get connection status
"CIPSTART",  // Establish TCP connection or register UDP port
"CIPSEND",   // Send data
"CIPSENDEX", // Send data, if <length> or “\0” is met, data will be sent
"CIPSENDBUF",// Write data into TCP-send-buffer
"CIPBUFRESET",// Reset segment ID count
"CIPBUFSTATUS",// Check status of TCP-send-buffer
"CIPCHECKSEQ",// Check if a specific segment is sent or not
"CIPCLOSE",  // Close TCP/UDP connection
"CIFSR",     // Get local IP address
"CIPMUX",    // Set multiple connections mode
"CIPSERVER", // Configure as server
"CIPMODE",   // Set transmission mode
"SAVETRANSLINK",// Save transparent transmission link to Flash
"CIPSTO",    // Set timeout when ESP8266 runs as TCP server
"CIUPDATE",  // Upgrade firmware through network
"PING",      // Function PING
"CIPDINFO",   // Show remote IP and remote port with “+IPD”
"*"
};

//indices dos 3 grupos de comandos para o help
int i1,i2,i3;

SoftwareSerial esp8266(RX, TX, false);
ESP8266 wifi8266(esp8266, UART_ESP8266);

int uartESP8266 = UART_ESP8266;

/*
 * pisca o led no pino led n vezes e depois deixa desligado;
 * Pode ser usado também como reset físico;
 * Se seu ESP8266 exige isto, troque a ordem dos comandos 
 * digitalWrite abaixo - primeiro LOW e depois HIGH
 * teoricamente se este pino estiver ligado ao RST do ESP8266
 * funcionará como um reset fisico, dispensando o AT+RST
 * no caso do ESP8266 não estiver respondendo a comandos AT
 */
void piscaLed(int led, int n){
  pinMode(led, OUTPUT);
  for(uint8_t i=0; i<n; i++) {
    digitalWrite(led, LOW);  delay(100);
    digitalWrite(led, HIGH); delay(100);
  }
}

void displayHelp(){
//  int i,j,k;
   char line[70];
   char* format1 = "%02u AT+%-17.17s";
   char* format0 = "%02u %-20.20s";

   //i1 aponta para o lote de comandos da esquerda
   i1=0;

   //i2 ao final dessa loop irá apontar para o lote de comandos do meio
   for (i2=i1+1; atCommands[i2][0] != '*'; i2++) ; //segundo lote de comandos

   //i3 ao final dessa loop irá apontar para o lote de comandos da direita
   for (i3=i2+1; atCommands[i3][0] != '*'; i3++) ; //terceiro lote de comandos

   Serial.println(F("AT Commands"));
   do {
      //limpa linha que receberá os tres comandos
      for (int x=0; x<69; x++) {
         line[x] = '\0';
      }

      //primeira linha (cabecalho)
      if (i1 == 0) {
         //imprime cabecalho
         sprintf(line, "%-23.23s%-23.23s%-23.23s", atCommands[i1++],atCommands[i2++],atCommands[i3++]);
      }

      //linhas de comando
      else {
         //ainda tem comandos à esquerda ?
         if (atCommands[i1][0] != '*' || i1==0) {
            if (i1 < 3)
               sprintf(line, format0, i1-1, atCommands[i1++]);
            else
               sprintf(line, format1, i1-1, atCommands[i1++]);
         }

         //ainda tem comandos no meio ?
         if (atCommands[i2][0] != '*' || i1==1)
            sprintf(&line[23], format1, i2-2, atCommands[i2++]);

         //ainda tem comandos à direita ?
         if (atCommands[i3][0] != '*' || i1==1)
            sprintf(&line[46], format1, i3-3, atCommands[i3++]);

         //substitue nulo por espaço em branco
         for (int x=0; x<69; x++)
            if (line[x] == '\0')
               line[x] = ' ';
      }
      Serial.println(line);
   }
   while (
      atCommands[i1][0] != '*'
      ||
      atCommands[i2][0] != '*'
      ||
      atCommands[i3][0] != '*'
   ) ;
   Serial.println("*");
}

//uma forma de pegar comandos da serial
String readSerial(String msg){
   String resp = "";
   Serial.setTimeout(TIMEOUT_SERIAL);
   if (msg != "")
      Serial.print(msg);
   resp = Serial.readStringUntil(10);
   if (resp != ""){
      Serial.print("\n{");
      Serial.print(resp);
      Serial.println ("}");
   }
   return resp;
}

//outra forma de pegar comandos da serial
/*
String getCommand() {
  String command = "";
  int timeout = TIMEOUT_SERIAL;
  long int time = millis();
  Serial.flush();
  Serial.println("Waiting for a command...");
  while ( (time + timeout) > millis() )
    while (Serial.available()) {
      char c = Serial.read();
        command += c;
      if (command.indexOf("\r\n") > 0) {
        command.remove(command.length()-1);
        return command;
      }
    }
  return command;
}
*/

void testCommands(){
   //AT+RST destroe firmware em algumas versões
//  sendData(F("AT+RST"), TIMEOUT_ESP8266+3000);
   sendData(F("AT"), TIMEOUT_ESP8266);
   sendData(F("AT+GMR"), TIMEOUT_ESP8266);
   sendData(F("AT+CWMODE_DEF?"), TIMEOUT_ESP8266);
//  sendData(F("AT+CWMODE_CUR=3"), TIMEOUT_ESP8266);
   sendData(F("AT+CWLAP"), TIMEOUT_ESP8266);
   sendData(F("AT+CWJAP_DEF?"), TIMEOUT_ESP8266);
   sendData(F("AT+CWSAP_DEF?"), TIMEOUT_ESP8266);
   sendData(F("AT+CIFSR"), TIMEOUT_ESP8266);
//  sendData("AT+CIPSTATUS", TIMEOUT_ESP8266);
}

void statusLedSetup() {
   pinMode(STATUS_LED, OUTPUT);
   digitalWrite(STATUS_LED, LOW);

   /* os defines estão em:
    * arduino-.../hardware/tools/avr/avr/include/avr/iom328p.h
    * (ou otro iomXXX.h, vai depender da placa
    */
   // initialize Timer1
   cli();          // disable global interrupts
   TCCR1A = 0;     // set entire TCCR1A register to 0
   TCCR1B = 0;     // same for TCCR1B
  
   // set compare match register to desired timer count:
   // OCR1A cada interrupção se dará com 250ms - 1seg/4
   OCR1A = 15624/4;  //15624 = 1seg;
   // turn on CTC mode:
   TCCR1B |= (1 << WGM12); //00000001 | 00000001 << 3 = 00001000
   // Set CS10 and CS12 bits for 1024 prescaler:
   TCCR1B |= (1 << CS10);  //00001000 | 00000001 << 0 = 00001001
   TCCR1B |= (1 << CS12);  //00001001 | 00000001 << 2 = 00001101
   // enable timer compare interrupt:
   TIMSK1 |= (1 << OCIE1A);
   sei();          // enable global interrupts
}
volatile byte seconds = 0;
volatile byte pause = 0;
#define TIMEOUT_SECONDS 10   //interrupções blink (1/2 high, 1/2 low)
                            //tem que ser um numero par, high/low
#define TIMEOUT_PAUSE 50    //50 interrupções sem blink pause

ISR(TIMER1_COMPA_vect)
{
   if (seconds >= TIMEOUT_SECONDS) {
      pause++;
      if (pause == TIMEOUT_PAUSE) {
         seconds = 0;
         pause = 0;
      }
   }
   else {
      digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
      seconds++;
   }
}

void setup() {
   //ativa interrupt
   statusLedSetup();
  
   //ativa o RST
   piscaLed(RST, 2);

   Serial.begin(UART_SERIAL); 

   esp8266.begin(uartESP8266);

   testCommands();

   displayHelp();
}

#define NCOMMANDSMAX 50
int nCommands=0;
void loop() {
   String command;
   String msg;
   if (nCommands == 0 || nCommands >= NCOMMANDSMAX) {
      if (nCommands >= NCOMMANDSMAX)
         Serial.println('*');
      nCommands = 0;
      msg = "cmd:";
   }
   else
      msg = ".";
   nCommands++;
   command = readSerial(msg);
   if (command == "")
      return;
//  Serial.println(command);
   String lowerCommand = command;
   lowerCommand.toLowerCase();
   if (isdigit(command.charAt(0)) and isdigit(command.charAt(1))) {
      montaCommand(command);
   }
   else if ( (lowerCommand.indexOf("help")) >= 0 ) {
      displayHelp();
   }
   else if (lowerCommand.indexOf("test") >= 0){
      testCommands();
   }
   else if (lowerCommand.indexOf("uart=") >= 0){
      uartRestart(command);
   }
   else if (lowerCommand.indexOf("web") >= 0){
      getWebRequest(command.substring(3));
   }
   else {
      sendData(command, TIMEOUT_ESP8266);
   }
   nCommands = 0;
//  Serial.println("voltou");
}

void getWebRequest(String str){
   /*
   * String str na forma "port,timeout", timeout in sec
   */
   int webPort = 80;         //default port
   int webTimeout = 10;      //default timeout in seconds
   if (!str.equals("")) {    //something informed
      int i=str.indexOf(",");
      if (i < 0) {
      /* 
       * no "," found -> "port" informed
       */
         webPort = str.toInt();
      }
      else {
      /* 
       * "," found -> "port,timeout" or ",timeout" informed
       */
         if (i == 0) {
        /* 
         * ",timeout" informed
         */
            webTimeout = str.substring(1).toInt();
         }
         else {
        /* 
         * "port,timeout" informed
         */
            webPort = str.substring(0,i).toInt();
            webTimeout= str.substring(i+1).toInt();
         }
      }
   }
   String requestMsg;
   requestMsg = F("Run server port:"); requestMsg += webPort;
   requestMsg += F(", timeout:"); requestMsg += webTimeout;
   Serial.println(requestMsg);
   //set multiple 
   Serial.print(F("mux "));
   Serial.println(wifi8266.enableMUX()?"ok":"err");
   //start tcp server in port webPort
   Serial.print(F("start tcp server "));
   Serial.println(wifi8266.startTCPServer(webPort)?"ok":"err");
   //set server timeout secTimeout
   Serial.print(F("tcp server timeout "));
   Serial.println(wifi8266.setTCPServerTimeout(webTimeout)?"ok":"err");
   requestMsg = "";

   /*
    * get request from http client
    */
   long unsigned int millisTimeout = webTimeout*1000;
   long unsigned int timeOut = millis();
   bool inputChar = false;
   long unsigned int timeWithoutInput = millis();
   while ( (timeOut + millisTimeout) > millis()) {
      #define BUFFER_SIZE 192
      uint8_t buffer[BUFFER_SIZE] = {0};
      uint8_t mux_id;
      uint32_t len = wifi8266.recv(&mux_id, buffer, sizeof(buffer), 100);
      if (len > 0) {
         Serial.print(F("\nStatus:["));
         Serial.print(wifi8266.getIPStatus().c_str());
         Serial.println("]");
        
         Serial.print(F("Recv from:"));
         Serial.println(mux_id);
         Serial.print("[");
         for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
         }
         Serial.println("]");
  
        //monta replyMsg;
        /*
        String clientMsg = responseMsg("Ok!!");
        Serial.println(clientMsg);
        for (int i=0; (i<clientMsg.length() && i<BUFFER_SIZE); i++)
          buffer[i] = clientMsg.charAt(i);
        buffer[clientMsg.length()] = 0;
        */
        /*
         * envia mensagem de retorno ao cliente Http
         */
         Serial.print(F("send back "));
//        Serial.println( wifi8266.send(mux_id, buffer, strlen((char*)buffer)) ? "ok" : "err" );
         Serial.println( wifi8266.send(mux_id, buffer, len) ? "ok" : "err" );
        
         Serial.print(F("release tcp "));
         Serial.print(mux_id);
         Serial.println( wifi8266.releaseTCP(mux_id) ? "ok" : "err" );
        
         Serial.print(F("Status:["));
         Serial.print(wifi8266.getIPStatus());
         Serial.println("]");
        
         timeOut = millis();
      }
      else if ( (timeWithoutInput + 1000) < millis() ) {
         Serial.print("*");
         timeWithoutInput = millis();
      }
   }
   Serial.println(F("\nTimeout server..."));
}


String responseMsg(String bodyMsg){
   String clientMsg;
   //sending a standard Http response header
   clientMsg = F("HTTP/1.1 200 OK\n");
   clientMsg += F("Content-Type: text/html\n");
   clientMsg += F("\n<!DOCTYPE HTML>\n");
   clientMsg += F("<html>");
   //~ clientMsg += "<body>\n";
   clientMsg += bodyMsg;
   //~ clientMsg += "\n</body>\n";
   clientMsg += F("</html>\n\0");
   return clientMsg;
}

void uartRestart(String command){
   uartESP8266 = (command.substring(5)).toInt();
//  esp8266.end();
   esp8266.begin(uartESP8266);
   testCommands();
}

void montaCommand(String command){
   String cmdString = ""; 
   int index = (command.substring(0,2)).toInt();

  //determina o indice correto com base no inicio de cada grupo de comandos
   if (index >= i2-1)
      index += 2;
   else if (index >= i1)
      index += 1;

  //copia o comando da array
   if (index > 2)
      cmdString += "AT+";
   for (int i=0; i<strlen(atCommands[index]); i++)
      cmdString += atCommands[index][i];
  //se tiver complemento de comando, acrescenta....
   if (command.length() > 2)
      cmdString += command.substring(2);

//  String pergunta = "Send "; pergunta += cmdString; pergunta += " (y,n)";
//  String resp = readSerial(pergunta);
//  if (tolower(resp.charAt(0)) == 'y')
    sendData(cmdString, TIMEOUT_ESP8266);
}

String sendData(String command, const int timeout)
{
   String response = "";
   String ok = "OK";
   String error = "ERR";

   Serial.print (F("==> {"));
   Serial.print (command);
   Serial.println ("}");

   // AT command to ESP8266
   esp8266.println(command);

   long int time = millis();
   bool found = false;
   while ( (time + timeout) > millis() && !found) {
      while (esp8266.available()) {
         response += (char)esp8266.read();
         if (response.indexOf(ok) != -1 || response.indexOf(error) != -1) {
            found = true;
            break;
         }
      }
   }
   //module reply
   Serial.println(response);
   return response;
}

