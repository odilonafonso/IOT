/* 
Innovativetom.com
Flower Pot Soil Mosture Sensor
A0 - Soil Mosture Sensor
D3:D7 - LEDS 1,2,3,4,5
LED1 - Green
LED2 - Green
LED3 - Green
LED4 - YELLOW
LED5 - RED
Connect the Soil Mosture Sensor to anolog input pin 0, 
and your 5 led to digital out 2-6
*/
int led1 = 4;
int led2 = 5;
int led3 = 6;
int led4 = 7;
int led5 = 8;

int AnalogMoistureSensor = 0;
int DigitalMoistureSensor = 14;

#define COMPLETELY_DRY 1024
#define VERY_DRY 800
#define DRY 600
#define WET 410
#define VERY_WET 300
#define SOAKED 240

#define DELAY_READ 2000
  
void setup() {
   // Serial Begin so we can see the data from the mosture sensor in our serial input window. 
   Serial.begin(9600);
   // setting the led pins to outputs
   pinMode(led1, OUTPUT);
   pinMode(led2, OUTPUT);
   pinMode(led3, OUTPUT);
   pinMode(led4, OUTPUT);
   pinMode(led5, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
/*
 * Digital read
 */
   // read the input on digital pin:
   int sensorValue = digitalRead(DigitalMoistureSensor);
   // print out the value you read:
   Serial.print("Digital read sensor: ");Serial.print(sensorValue);

/*
 * Analog read
 */
   // read the input on analog pin 0:
   sensorValue = analogRead(AnalogMoistureSensor);
   // print out the value you read:
   Serial.print("\t Analog read sensor: ");Serial.print(sensorValue);

    String stringSoilMoisture = "SS:";

    int drySoil = COMPLETELY_DRY - SOAKED;
    Serial.print("\ndrySoil ");Serial.print(drySoil);
    int actualSoil = sensorValue - SOAKED;
    Serial.print("\tactualSoil ");Serial.print(actualSoil);
    float indexSoil = 100*(float)(drySoil - actualSoil) / drySoil;
    stringSoilMoisture += String(indexSoil,2);
    stringSoilMoisture += '%';

   Serial.print("\t ");Serial.println(stringSoilMoisture);
 
   if (sensorValue >= COMPLETELY_DRY)
   {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH);
      digitalWrite(led4, HIGH);
      digitalWrite(led5, HIGH);
   }
   else if (sensorValue >= VERY_DRY  && sensorValue < COMPLETELY_DRY)
   {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH);
      digitalWrite(led4, HIGH);
      digitalWrite(led5, LOW);
   }  
   else if (sensorValue >= DRY  && sensorValue < VERY_DRY)
   {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH);
      digitalWrite(led4, LOW);
      digitalWrite(led5, LOW);
   }    
   else if (sensorValue >= WET && sensorValue < DRY)
   {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, LOW);
      digitalWrite(led4, LOW);
      digitalWrite(led5, LOW);
   }
   else if (sensorValue >= VERY_WET && sensorValue < WET)
   {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
      digitalWrite(led4, LOW);
      digitalWrite(led5, LOW);
   }
   else if (sensorValue >= SOAKED && sensorValue < VERY_WET)
   {
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
      digitalWrite(led4, LOW);
      digitalWrite(led5, LOW);
   }
   delay(DELAY_READ);        // delay 1 second between reads
}

