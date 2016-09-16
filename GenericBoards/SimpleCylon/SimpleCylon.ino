/*
  Simple Cylon
  Cylon Eye sweep using 5 LEDs
  http://www.hobbytronics.co.uk/tutorials-code/arduino-tutorials/arduino-tutorial4-cylon
  In this Arduino UNO tutorial, we are expanding on our simple LED flash tutorial. We will use 5
  LEDs to create a simple Cylon roving eye effect. To do this we will make life easier for
  ourselves and use direct port manipulation.
  That is we will write directly to one of the Arduino UNO ports rather than writing to
  individual pins. This allows us to set the values for each of the LEDs in one operation.

  The Arduino UNO has 3 ports

    B (digital pin 8 to 13)
    C (analog input pins)
    D (digital pins 0 to 7)

  Each port is controlled by three registers, which are also defined variables in the arduino
  language. The DDR register, determines whether the pin is an INPUT or OUTPUT.
  The PORT register controls whether the pin is HIGH or LOW, and the PIN register reads the
  state of INPUT pins set to input with pinMode()

  We will use port B for our Cylon eye sketch. First we need to set all the port B pins as
  digital outputs. Port B has only six pins allocated to it.
  The bits in the port B register (DDRB) control whether each of the pins in PORTB are
  configured as inputs or outputs, Setting a pin to 1 makes it an OUTPUT whilst setting it to 0
  makes it an INPUT. Port pins are numbered from 0 to 7 but not all ports have 8 pins.
  So, for example:
 
  DDRB = B00111110; // sets Arduino port B pins 1 to 5 as outputs, pin 0 as input

  NOTE:If you have used any of Microchips microcontrollers they use the opposite strategy,
  where 0 is OUTPUT and 1 is INPUT
 */
 
bool OnOff=true;
bool upDown=true;  // start off going UP 
char cylon=-1;  // determines which LED is on 0 to 4
#define DELAY 500

void setup() {                
  // initialize the digital pins as outputs.
  DDRB = B00011111;  // sets Arduino port B pins 0 to 4 as outputs     
}

void loop() {
  if(upDown){
    cylon++;
    if (OnOff)
      PORTB |= (1 << cylon);
    else
      PORTB = 1 << cylon;
    if(cylon>=4) upDown=false;      // Reached max LED, next time we need to go down
  }
  else {
    cylon--;
    if (OnOff)
      PORTB &= B11111111&(~(1 << (cylon+1)));
    else
      PORTB = 1 << cylon;
    if(cylon<=0) upDown=true;      // Reached min LED, next time we need to go up
  }
//  PORTB = 1 << cylon;
  delay(DELAY);              // wait for a second
}

