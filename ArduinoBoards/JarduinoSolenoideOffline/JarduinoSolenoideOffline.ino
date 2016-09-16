/*
 * Arquitetura utilizada:
 * Arduino ProMini ATMega328
 * USB To RS232 TTL PL2303HX Auto Converter Module Converter Adapter For arduino
 * Fonte de alimentação mini 5V
 * Sensor de umidade do solo
 * Relay trigger High
 * 
 */
#define BOOM 1000000    //1sec - 1000000 microseconds
#define LED1 5
#define LED2 4
int task1Id, task2Id;

#include <TimerMultiTask.h>

/*
 * create a TimerMultiTask object 
 */
TimerMultiTask timerMultiTask;

#define LED_DRY 8
#define LED_WET 9
#define SOIL_ANALOG_PIN 0     //analog pin off the SoilMoisture sensor
#define DIGITAL_PUMP_PIN 11   //digital pin off the pump (or solenoid) to on water

String sensorString;
void checkSoilMoistureSensor(){
   #define SOIL_DRY 100
   #define SOIL_WET 700

   int intSensor, pumpStatus;
   digitalWrite(PIN_POWER,LOW);
   //read analog pin with the SoilMoisture sensor
   intSensor = analogRead(SOIL_ANALOG_PIN);
   digitalWrite(PIN_POWER,HIGH);
   pumpStatus = digitalRead(DIGITAL_PUMP_PIN); //really necessary ?
   sensorString = F("S:");
   sensorString += String(intSensor);
   sensorString += (pumpStatus ? F(" On") : F(" Off"));
   Serial.println(sensorString);
   //power off sensor leds
   digitalWrite(LED_DRY, LOW);
   digitalWrite(LED_WET, LOW);
   // check if soil dry, very dry
   if (intSensor <= SOIL_DRY) {
      //solo seco; liga led de aviso
      digitalWrite(LED_DRY, HIGH);
      //e liga bombinha..
      digitalWrite(DIGITAL_PUMP_PIN, HIGH);
   }
   //soil wet or very wet
   else {
      //check if very wet
      if (intSensor > SOIL_WET)
         //very wet soil; power on led
         digitalWrite(LED_WET, HIGH);
         //e power off water pump or solenoid;
         digitalWrite(DIGITAL_PUMP_PIN, LOW);
   }
}

#define SERIAL_BAUD 9600
void setup() {
   Serial.begin(SERIAL_BAUD);

   // set SoilMoisture LEDS On and pump Off
   pinMode(LED_DRY, OUTPUT);
   digitalWrite(LED_DRY, HIGH);   
   pinMode(LED_WET, OUTPUT);
   digitalWrite(LED_WET, HIGH);   
   pinMode(DIGITAL_PUMP_PIN, OUTPUT);
   digitalWrite(DIGITAL_PUMP_PIN, LOW);   
   /*
    * set a boom of 1000000 microseconds (1 sec) Arduino boards
    * or 1000 milliseconds ESP8266 boards
    */
   timerMultiTask.setBOOM(BOOM);
  /*
   * add a task that will be acioned each 3 boom intervals - 3 secs
   * the user callback function is userCall1
   */
   task1Id = timerMultiTask.add (3, checkSoilMoistureSensor);
}

void loop(void) {
   delay(10);
}


