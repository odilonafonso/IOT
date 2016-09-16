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

#define SERVO 9
#define INICIO 0
#define DELAY_RACAO 1500
#define PORTINHOLA_ABERTA 110
#define STEP 3
#define PROXIMA_RACAO 3000
#define DELAY_STEP 19

#define PIN_ABRE 7
#define PIN_FECHA 8
#define PIN_AUTOMATICO 6

void setup() {
//  myservo.attach(SERVO);  // attaches the servo on pin 9 to the servo object
   pinMode(PIN_AUTOMATICO, INPUT);
   pinMode(PIN_ABRE, OUTPUT);
   pinMode(PIN_FECHA, OUTPUT);
   Serial.begin(9600);
   Serial.println("Starting...");
}

void loop() {
   int actionAuto, actionAbre, actionFecha;
   if ( digitalRead(PIN_AUTOMATICO) > 0 ) {
      Serial.println("automatico ON..");
      automatico();
   }
   else if ( digitalRead(PIN_ABRE) > 0 ) {
      Serial.println("abre ON..");
      abre();
   }
   else if ( digitalRead(PIN_FECHA) > 0 ) {
      Serial.println("fecha ON..");
      fecha();
   }
//   delay (1000);
}

void automatico() {
   abre();   
   delay(DELAY_RACAO);            
   fecha();
   delay(PROXIMA_RACAO);
}

void abre() {
   myservo.attach(SERVO);  // attaches the servo on pin 9 to the servo object
   myservo.write(INICIO);
   delay(150);
   // goes from INICIO degrees to PORTINHOLA_ABERTA degrees
   for (pos = INICIO; pos <= PORTINHOLA_ABERTA; pos += STEP) {
      // in steps of STEP degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(DELAY_STEP);                       // waits 15ms for the servo to reach the position
   }
   //delay para portinhola chegar à posição desejada
   delay(50);
   myservo.detach();
}

void fecha() {
   myservo.attach(SERVO);  // attaches the servo on pin 9 to the servo object
   // goes from PORTINHOLA_ABERTA degrees to INICIO degrees
   for (pos = PORTINHOLA_ABERTA-STEP; pos >= INICIO; pos -= STEP) {
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(DELAY_STEP);                       // waits 15ms for the servo to reach the position
   }
   delay(50);
   //delay para portinhola chegar à posição desejada
   myservo.detach();
}

