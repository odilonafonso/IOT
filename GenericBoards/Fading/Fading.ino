/*
 Fading

 This example shows how to fade an LED using the analogWrite() function.

 The circuit:
 * LED attached from digital pin 9 to ground.

 Created 1 Nov 2008
 By David A. Mellis
 modified 30 Aug 2011
 By Tom Igoe

 http://www.arduino.cc/en/Tutorial/Fading

 This example code is in the public domain.

 */


int ledPin = 9;    // LED connected to digital pin 9
#define MIN 0
#define MAX 255
#define STEP 3
#define DELAY 25

void setup() {
  // nothing happens in setup
}

void loop() {
  // fade in from min to max in increments of 5 points:
  delay(DELAY);
  for (int fadeValue = MIN; fadeValue <= MAX; fadeValue += STEP) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(DELAY);
  }

  // fade out from max to min in increments of 5 points:
  for (int fadeValue = MAX ; fadeValue >= MIN; fadeValue -= STEP) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(DELAY);
  }
  delay(DELAY);
}


