#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

#define REGISTRO_TANQUE 3           //SERVO QUE FECHA REGISTRO DO TANQUE
#define REGISTRO_ABERTO 0           //ANGULO INICIAL SERVO DO REGISTRO
#define REGISTRO_FECHADO 115
#define STEP 3
#define DELAY_STEP 19

int servoPosition;
bool registroAberto = false;

void AbreRegistro(){
   myservo.attach(REGISTRO_TANQUE);  // attaches the servo on pin to the servo object
   myservo.write(REGISTRO_ABERTO);
   registroAberto = true;
   delay(1500);
   myservo.detach();
}

void FechaRegistro(){
   myservo.attach(REGISTRO_TANQUE);  // attaches the servo on pin to the servo object
   // goes from REGISTRO_ABERTO degrees to REGISTRO_FECHADO degrees
   for (pos = REGISTRO_ABERTO; pos <= REGISTRO_FECHADO; pos += STEP) {
      // in steps of STEP degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(DELAY_STEP);                       // waits 15ms for the servo to reach the position
   }
   registroAberto = false;
   delay(1500);
   myservo.detach();
}

bool RegistroAberto(){
   myservo.attach(REGISTRO_TANQUE);  // attaches the servo on pin to the servo object
   servoPosition = myservo.read();
//   return (servoPosition==REGISTRO_ABERTO);
   myservo.detach();
   return registroAberto;
}

void setup_servo(){
   AbreRegistro();
}


