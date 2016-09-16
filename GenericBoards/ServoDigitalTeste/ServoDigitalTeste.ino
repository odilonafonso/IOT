/* PotentiometerServo without servo library
 *  http://classes.dma.ucla.edu/Spring11/152BC/wp-content/uploads/2011/04/PotentiometerServo.pdf
 *  
 
 modified 28 Abr 2016
 by Odilon Cenamo

 Definig a arbitrary value for servo position
*/

#define SERVOPIN 9
#define POSITION_TIME 2800    //microseconds
int minPulseTime = 500;
int maxPulseTime = 2400;
int pulseTime;

long lastPulse = 0;  //the time in milliseconds of the last pulse
int refreshTime = 100; //the time needed in between pulses in millisecoonds
//servos have 50Hz rate, 1sec/50 = 20 milliseconds

int analogValue = 0;
int ANALOGPIN = 0;
#define ANALOG false  //if there is a potentiometer on Analog pin 

void setup() {
   pinMode(SERVOPIN, OUTPUT);
   pulseTime = minPulseTime; //set the motor position value to the minimum
   Serial.begin(9600);   
}

#define SERVO 9
#define INICIO 0
#define DELAY_RACAO 1500
#define PORTINHOLA_ABERTA 110
#define STEP 3
#define PROXIMA_RACAO 3000
#define DELAY_STEP 19


void loop() {
   if (ANALOG) {
      // read the analog input
      analogValue = analogRead(ANALOGPIN);
      /* 
       * convert the analog value to a range between minPulse and maxPulse.
       */
      pulseTime = map(analogValue,0,1023,minPulseTime,maxPulseTime);
   }
   else {
      pulseTime = POSITION_TIME;  //force a pre-defined value
   }
   // pulse the servo again if the refresh time (20 ms) have passed:
   if (millis() - lastPulse >= refreshTime) {
      digitalWrite(SERVOPIN, HIGH);       // turn on pulse to the servo
      delayMicroseconds(pulseTime);          // length of the pulse sets the motor position
      digitalWrite(SERVOPIN, LOW);        // stop pulse to the servo
      delayMicroseconds(pulseTime);          // length of the pulse sets the motor position
      lastPulse = millis();                    // save the time of the last pulse}
   }
   Serial.println(analogValue);
   Serial.println(pulseTime);
   delay(1000);
}

