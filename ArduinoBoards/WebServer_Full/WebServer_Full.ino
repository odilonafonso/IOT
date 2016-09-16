/*
  Web Server
 
 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 modified 13 Agos 2013
 by Roberto Cervero
 */

#include <SPI.h>
#include <Ethernet.h>

//CONFIGURACION DE RED Y CONEXION
//Direccion IP ES MUY PROBABLE QUE TENGAS QUE AJUSTARLO A TU RED LOCAL
//Cambia la ip por una dentro de tu rango, tambien debes actualizarla en tu aplicacion android
//Ajusta el usuario y contrasña que registraste en la aplicacion android

// NETWORK CONFIGURATION AND CONNECTION
// IP Address, ADJUST TO YOUR LOCAL NETWORK
//If you change the IP address will have to adjust in android application
//Sets the user name and password registered in android application


byte mac[] = {0xDE, 0xFD, 0xAE, 0xAF, 0xFE, 0xED };
//IPAddress ip(192, 168, 1, 150);
const String Mail ="YOUR_MAIL@hotmail.com";
const String Key ="123456";

/***********************************************************************************************************************/
/***********************************************************************************************************************/
/***********************************************************************************************************************/
/***********************************************************************************************************************/
//FIN DE ZONAS DE CONFIGURACIONES
//END SETTINGS ZONE
/***********************************************************************************************************************/
/***********************************************************************************************************************/
/***********************************************************************************************************************/
/***********************************************************************************************************************/

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(81);
EthernetClient client;
unsigned long lastDebounceTime = 0;
byte EspRfrIp = 1;

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  // start the Ethernet connection and the server:
//  Ethernet.begin(mac, ip);
  Ethernet.begin(mac);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  lastDebounceTime= millis();
}


void loop() {
  IncomingConnections();
  GestRfIp();
}
void IncomingConnections(){

  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
                    
          client.println("<head><meta content='text/html; charset=utf-8' http-equiv='Content-Type' />");
          client.println("<link href='http://excontrol.es/CSS/ARDUINO-WEB.css' rel='stylesheet' type='text/css' /></head>");
          client.println("<body><div id='CmdLogo' ><img id='Logo' alt='' src='http://excontrol.es/Imagenes/LogoArduinoWebServer.png' /></div>");
          client.println("<div id='Cmd1' ><a href='http://excontrol.es/Arduino-WebServer/' target='_blank'>GO TO PROJECT WEBSITE</a></div>");
          client.println("<div id='Cmd2' ><a href='http://excontrol.es/Domotica-Arduino/' target='_blank' >ARDUINO HOME AUTOMATION</a></div></body></html>");   

          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }

}
/***********************************************************************************************************************/
/***********************************************************************************************************************/
/***********************************************************************************************************************/
/***********************************************************************************************************************/
//FIN DE ZONAS DE CONFIGURACIONES
//END SETTINGS ZONE
/***********************************************************************************************************************/
/***********************************************************************************************************************/
/***********************************************************************************************************************/

void GestRfIp(){
  unsigned long MillisNow = millis();
  if (MillisNow<lastDebounceTime){lastDebounceTime= millis();}
  if ((MillisNow - lastDebounceTime) >= 60000) {lastDebounceTime= millis(); if (EspRfrIp<1){connectAndRfr();EspRfrIp=20;}else{EspRfrIp--;}}
}

void connectAndRfr(){

  if (client.connect("www.excontrol.es", 80)) {
    Serial.println("connected");
    client.print("GET http://excontrol.es/Users/IpSet.aspx?Mail=");
    client.print(Mail  + "&Key=" + Key);
    client.println(" HTTP/1.0");
    client.println();
    RfIp();
  }
}

void RfIp(){
  int Reintento;
  Reintento=0;

  while(true){    
    if (client.available()) {
    client.stop();
    client.flush();
    return;
    }
    else{
      Reintento++;
      IncomingConnections();
      delay(10);
      if (Reintento >= 100 ){
        client.stop();
        client.flush();
        return;
      }
    }
  }
}
