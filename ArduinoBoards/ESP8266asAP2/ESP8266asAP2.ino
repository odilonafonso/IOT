
#include <SoftwareSerial.h>

/* Setup esp8266 as an access point and a server.
   Allows connecting browser to 192.168.4.1:8000
   Allows connecting another esp8266 and sending roundtrip time.
   define ACCESSPT to setup an access pt. on boot.
 */

#define ACCESSPT
#define RX 2
#define TX 3

const int ledPin=9; // lights led on arduino if none attached to pin 13
int accesspt=0;

// connect RX=2,TX=3 on arduino to TX,RX on esp8266
SoftwareSerial espSerial(RX, TX, false);

String msg;

void setup()  
{
  espSerial.begin(9600);
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  Serial.println(F("setup completed..."));
}
#define GETBUF 160


char cmd[64],b[GETBUF];
int n=0,id;

// input typed by user
void process_input(int i) {
  cmd[i]=0;
  if( *cmd == 'r' )
    espSerial.println(F("AT+GMR"));
  else if( strncmp(cmd,"acc",3) == 0 )
    access_point();
  else
    espSerial.println(cmd); // send to esp8266
}

#ifdef ACCESSPT
// setup access pt. when booted.
void boot_and_setup_accesspt() {
  if( ! accesspt ) {  // start up access pt.
    do {
      espSerial.println(F("AT"));  // wait until at returns ok
    }
    while( ! waitcmd("OK",1000) )
      ;
    access_point();  
    accesspt=1;
  }  
}
#endif

void loop() 
{  
  int i=0;

  if (espSerial.available()) {// if input from the esp8266    
    while (espSerial.available()) {
      char c=espSerial.read();  // read from esp module
 //     Serial.write(c);  // echo char to console
      if( c == '+' )  // detect +IPD
        n=0;    
      b[n++]=c;
      b[n]=0;
      if( n == GETBUF-1 )  // start over
        n=0;

      char *str;
      // Round trip timing, time sent from another esp connected to an arduino
      if( strstr(b,"+IPD") && (str=strchr(b,'$')) ) {  
        char *p=strchr(b,':')+1; // pt to begin of payload
        int len;
        if( p ) { // numeric payload
          len=strlen(p);
          id=atoi(b+5); // get id        
          *(str+1)=0;  // leave $ at end, 0 delimit incase xtra chars
          // prepare to send back
          sprintf(cmd,"%d,%d",id,len);
          espSerial.print(F("AT+CIPSEND="));         
          espSerial.println(cmd); // id and length
          if( waitcmd( ">", 50 ) ) { // wait up to 50mS for '>'
            espSerial.print(p);  // send
            blink(1,20);  // quick blink
          }
        }
        *b=*cmd=0;  // reset buffers and counter
        n=0;
      }  // Browser connection 192.168.4.1:8000
      else if((str=strstr(b,"+IPD")) && strstr(b,"HTTP") ) {   
        // +IPD,id,bytes_rcvd:msg content..    
        id=atoi(str+5);   // id or channel
        int chrcnt=atoi(strchr(str+5,',')+1); // total bytes recv'd
        char *msg=strchr(b,':')+1;  // start of recvd msg
        int chrs2get=chrcnt-strlen(msg);  // bytes to recv
        // must flush recv'd data before calling send page
        unsigned long t=millis();
        // read buffered bytes in < 200 ms, that way if byte count is
        // wrong we won't get stuck in this loop.
        while(chrs2get || millis() - t < 200 ) 
          if( espSerial.available() ) {
            espSerial.read();
            chrs2get--;
          }
          
        // send page to browser
        snd_page(id);
        // reset buffers and counter
        *b=*cmd=0;  
        n=0;
      } 
    }   // while input from esp available
  }

  while (Serial.available()) { // input from host
    cmd[i]=Serial.read();  // buffer cmd
    i++;
    delay(5);
  }
  if( i ) 
    process_input(i);

#ifdef ACCESSPT
  if( ! accesspt )
    boot_and_setup_accesspt();
#endif

}
// wait for string up to timeout ms.
// called after sending cmd to esp
int waitcmd(char *s, unsigned short timeout) {
  unsigned long tm=millis();
  while( 1 ) {
    if( millis() - tm >= timeout ) {
      Serial.print(F("TimedOutWaiting4 "));
      Serial.println(s);
      return 0;
    }
    if (espSerial.find(s)) {
      Serial.println(s);
      return 1;
    }
  }
}

void blink(int blinks, int led_on ) {

  while( blinks-- ) {
    digitalWrite(ledPin, 1);  // turn on led
    delay(led_on);  // led on time
    digitalWrite(ledPin, 0);
  }

}

// setup as access pt. as server
void access_point() {
  blink(100,2);

  msg = F("AT+GMR");
  sendData (msg, 2000, true);
/*
  Serial.println(F("SETUP ACCESS POINT"));
  Serial.println(F("AT+RST"));
  espSerial.println(F("AT+RST"));
//  waitcmd("thinker.com",5000);
  waitcmd("OK",3000);
*/

  Serial.println(F("AT+CWMODE=2"));
  espSerial.println(F("AT+CWMODE=2"));
  waitcmd("OK",1000);

  Serial.println(F("AT+CIPMUX=1"));
  espSerial.println(F("AT+CIPMUX=1"));
  waitcmd("OK",1000);

  msg = F("AT+CWSAP_CUR=\"esp826605\",\"12345678\",1,0,2,1");
  sendData (msg, 2000, true);
//  Serial.println(F("AT+CWSAP=\"esp826605\",\"12345678\",1,0"));
//  espSerial.println(F("AT+CWSAP=\"esp826605\",\"12345678\",1,0"));
//  waitcmd("OK",1000);

  msg = F("AT+CIPSERVER=0");
  sendData (msg, 2000, true);
//  waitcmd("OK",2000);

  msg = F("AT+CIPSERVER=1,8000");
  sendData (msg, 2000, true);
//  Serial.println(espSerial.println(F("AT+CIPSERVER=1,8000")));
//  waitcmd("OK",2000);

  msg = F("AT+CIFSR");
  sendData (msg, 2000, true);
//  Serial.println(F("AT+CIFSR"));
//  Serial.println(espSerial.println(F("AT+CIFSR")));
//  waitcmd("OK",3000);

  blink(3,100);
}
String sendData(String command, const int timeout, boolean debug)
{
  if (debug) {
    Serial.print ("Enviando: {");
    Serial.print (command);
    Serial.println ("}");
  }
  espSerial.println(command);
  return recvData(timeout, debug);
}
String recvData(const int timeout, boolean debug) {
  // Envio dos comandos AT para o modulo
  String response = "";
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (espSerial.available())
    {
      // The esp has data so display its output to the serial window
      char c = espSerial.read(); // read the next character.
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

void snd_page(int id) {
  static int cnt=0; 
  int total_len=0;  
  char *hdr[] ={ 
    "HTTP/1.1 200 OK\r\n",
    "Content-Type: text/html\r\n",
    "Connection: close\r\n", 
    NULL,  // content length string
    NULL,  // content string
    NULL   // terminating null
  };

  digitalWrite(ledPin, 1);  // turn on led

  cnt++;
  sprintf(b,"<p>Counter Value = %d</p>\r\n",cnt);
  hdr[4]=b;  // counter value
  total_len=strlen(b);
  hdr[3]=b+total_len+1;  // content length
  sprintf(hdr[3],"Content-Length: %d\r\n\r\n",total_len);  
  
  total_len=0;
  for(int i=0; hdr[i]!=NULL; i++)  // get total length of data to send
    total_len+=strlen(hdr[i]);
 
   // set channel and length of transfer
  sprintf(cmd,"AT+CIPSEND=%d,%d",id,total_len); 
  espSerial.println(cmd);
  
    // wait for '>' and send data
  if( waitcmd( ">", 50 ) ) {
    for(int i=0; hdr[i]!=NULL; i++)
      espSerial.print(hdr[i]);

    waitcmd("Unlink",150);  // after sending wait for unlink
  }
  
  digitalWrite(ledPin, 0);  // turn OFF led
}

