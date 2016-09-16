/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

#define INICIO 0
#define ABERTURA 250
#define DELAY 50 //150
#define DELAY_CHACOALHO 100 //200
#define ANGULO_CHACOALHO 90

#define DELAY_RACAO 1500
#define PORTINHOLA_ABERTA 90

void setup() {
//   Serial.begin(9600);
   myservo.attach(9);  // attaches the servo on pin 9 to the servo object
//   delay(15);                           // waits for the servo to get there
//   Serial.print("inicio:"); Serial.println(myservo.read());

//   testeGenerico();

//   serveRacao();
//   Serial.print("fim:"); Serial.println(myservo.read());
   
   myservo.detach();
}

void abrePortinhola() {
//   vai(INICIO);
//   myservo.write(PORTINHOLA_ABERTA);
   for (pos = 0; pos <= PORTINHOLA_ABERTA; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
   }
//   Serial.print("abriu:"); Serial.println(myservo.read());
}

void fechaPortinhola() {
   vai(INICIO);
//   Serial.print("fechou:"); Serial.println(myservo.read());
}

void serveRacao() {
   abrePortinhola();
   delay(DELAY_RACAO);
   fechaPortinhola();
//   vai(INICIO);
}

void testeGenerico() {
   vai(ABERTURA);
   delay(500);
//   vaivolta();
//   chacoalha();
   volta(INICIO);
   delay(500);
}

void vaivolta() {
  for (int i=0; i<3; i++) {
     myservo.write(0);
     delay(500);
     myservo.write(180);
     delay(500);
  }
}

#define TROCA_FUNCOES 5000
long timeStart;
long now;
int nFuncao = 1;

void lavagem() {
   myservo.attach(9);  // attaches the servo on pin 9 to the servo object
   testeGenerico();
   myservo.detach();
}

void centrifugacao() {
   myservo.attach(9);  // attaches the servo on pin 9 to the servo object
   chacoalha();
   myservo.detach();
}

void loop() {
   now = millis();
   if ( (now - timeStart) > TROCA_FUNCOES ) {
      if (nFuncao == 1)
         nFuncao = 2;
      else
         nFuncao = 1;
      timeStart = millis();
   }
   if (nFuncao == 1) 
      lavagem();
   else
      centrifugacao();
//   serveRacao();
   delay(100);
}

void vai(int p) {
   pos = p;
   myservo.write(pos);
//   delay(15);                           // waits for the servo to get there
}

void volta(int p) {
   pos = p;
   myservo.write(pos);
   delay(DELAY);
}

void chacoalha() {
   myservo.write(pos);
   delay(DELAY);
   for (int i=0; i<9; i++) {
      myservo.write(pos+ANGULO_CHACOALHO);
      delay(DELAY_CHACOALHO);
      myservo.write(pos-ANGULO_CHACOALHO);
      delay(DELAY_CHACOALHO);
   }
}

void vaivolta2(int destino) {
  for (pos = 0; pos <= destino; pos += 10) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(150);                       // waits 15ms for the servo to reach the position
  }
  for (pos = destino; pos >= 0; pos -= 10) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(150);                       // waits 15ms for the servo to reach the position
  }
}


