/*
  *
  * Udemy.com
  * Building an Arduino DC Voltmeter
  *
  */
 
float vPow = 5;   //3.3 for ESP8266
float r1 = 4520; //100000;
float r2 = 480;  //10000;



#define ANALOGPIN 5
 
void setup() {
   Serial.begin(9600);
   
   // Send ANSI terminal codes
//   Serial.print("\x1B");
//   Serial.print("[2J");
//   Serial.print("\x1B");
//   Serial.println("[H");
   // End ANSI terminal codes
   
   Serial.println("--------------------");
   Serial.println("DC VOLTMETER");
   Serial.print("Maximum Voltage: ");
   Serial.print((int)(vPow / (r2 / (r1 + r2))));
   Serial.println("V");
   Serial.println("--------------------");
   Serial.println("");
   
   delay(2000);
}
 
void loop() {
   float analogPin = analogRead(ANALOGPIN);

   float v1 = (analogPin * vPow) / 1024.0;

   float v2 = v1 / (r2 / (r1 + r2));

   float a1 = analogPin * 12.67;
   
   // Send ANSI terminal codes
//   Serial.print("\x1B");
//   Serial.print("[1A");
   // End ANSI terminal codes

   Serial.print("analogRead "); Serial.print(analogPin);
   Serial.print("     amp:"); Serial.print(a1);
   Serial.print("     v1:"); Serial.print(v1);
   Serial.print(" v2:"); Serial.println(v2);
   delay(1000);
}
 
