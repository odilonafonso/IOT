/* Blink without Delay

 Turns on and off a light emitting diode (LED) connected to a digital
 pin, without using the delay() function.  This means that other code
 can run at the same time without being interrupted by the LED code.

 The circuit:
 * LED attached from pin 13 to ground.
 * Note: on most Arduinos, there is already an LED on the board
 that's attached to pin 13, so no hardware is needed for this example.
 
 created 2005
 by David A. Mellis
 modified 8 Feb 2010
 by Paul Stoffregen
 modified 11 Nov 2013
 by Scott Fitzgerald
 
 
 This example code is in the public domain.
 
 http://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
 */

// constants won't change. Used here to set a pin number :
const int ledPin =  10;      // the number of the LED pin

// Variables will change :
int ledState = LOW;             // ledState used to set the LED

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change :
const long interval = 2000;           // interval at which to blink (milliseconds)

void softwareReset() {
/*
 * modo de fazer reset programaticamente:
 * ligar o pino RST do ESP8266 com resistor 2K no GND, 1K no pino RST do ProMini
 * iniciar com LOW por algum tempo (DELAY_RST)
 * em seguida deixar com HIGH
 * 
 * esse delay deve ser testado melhor; pode ser que funcione com menos tempo
 */
#define RST 10
#define DELAY_RST 1000
   pinMode(RST, OUTPUT);
   Serial.print("softRST");
   for (int i=0; i<5; i++) {
      Serial.print(".");
      digitalWrite(RST, LOW);
      Serial.println("ON");
      delay(DELAY_RST);
      Serial.println("Light ON");
      digitalWrite(RST, HIGH);
      delay(DELAY_RST);
  }
  Serial.println("");
}

void setup() {
   Serial.begin(9600);
   // set the digital pin as output:
   pinMode(ledPin, OUTPUT);
   softwareReset();
}

void loop()
{
//  softwareReset();
//   blinkWithoutDelay();
   delay(5000);
}

void blinkWithoutDelay() {

  // here is where you'd put code that needs to be running all the time.

  // check to see if it's time to blink the LED; that is, if the
  // difference between the current time and last time you blinked
  // the LED is bigger than the interval at which you want to
  // blink the LED.
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, !digitalRead(ledPin));
  }
}

