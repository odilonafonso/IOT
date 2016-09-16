#include <TimerOne.h>
volatile byte seconds=0;
#define TIMEOUT_SECONDS 10
#define LEDPIN 13

void setup() 
{
  // Initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards
  pinMode(LEDPIN, OUTPUT);    
  
//  Timer1.initialize(100000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
  Timer1.initialize(1000000); //1 sec
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here
}
 
void loop()
{
  // Main code loop
  // TODO: Put your regular (non-ISR) logic here
}

#define delayBlink 100

/// --------------------------
/// Custom ISR Timer Routine
/// --------------------------
void timerIsr()
{
    // Toggle LED
//    digitalWrite( 13, digitalRead( 13 ) ^ 1 );

    if (seconds == TIMEOUT_SECONDS){
        seconds = 0;
        long x;
//        digitalWrite(LEDPIN, !digitalRead(LEDPIN));
        for (int i=0; i<11; i++){
          digitalWrite(LEDPIN, HIGH);
          x=millis();
          while (millis() - x < delayBlink)          
            ;
          digitalWrite(LEDPIN, LOW);
          x=millis();
          while (millis() - x < delayBlink)          
            ;
        }
//        readMySensor();
    }

}
