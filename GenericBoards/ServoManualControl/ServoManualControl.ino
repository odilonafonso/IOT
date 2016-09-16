/* Manual servo control - PWM Sweeping
   Copyright 2012 by Zipwhip. 
http://blog.zipwhip.com/2012/03/28/manual-control-of-a-servo-on-the-arduino-for-the-zipwhip-textspresso-machine

   Modified July 2013 by LinuxCircle.com team
http://www.linuxcircle.com/2013/07/04/servo-manual-control-without-arduino-servo-library-on-raspberry-pi-and-gertboard/

A versao modificada funciona (teste1), a original(teste2) não

// You are free to use and modify this code in your own software.
// Please give us credit to by mentioning LinuxCircle.com 
// in the header of your published code.
*/

#define SERVO_PIN 2  // Any pin on the Arduino or Gertboard will work.

void setup()
{
   pinMode(SERVO_PIN, OUTPUT);
   Serial.begin(9600);
}

int lenMicroSecondsOfPeriod = 21 * 1000; // 25 milliseconds (ms)
//int lenMicroSecondsOfPulse = 1 * 1000; // 1 ms is 0 degrees
int first = 0.7 * 1000; //0.5ms is 0 degrees in HS-422 servo
int end = 1.9 * 1000;
int increment = 0.010 * 1000;

void loop()
{
//    teste1();
//    teste2();
    teste3();
}

void teste1() {
    int current = 0;
    Serial.println("ta indo...");
    for(current = first; current <end; current+=increment){
         // Servos work by sending a 25 ms pulse.  
         // 0.7 ms at the start of the pulse will turn the servo to the 0 degree position
         // 2.2 ms at the start of the pulse will turn the servo to the 90 degree position 
         // 3.7 ms at the start of the pulse will turn the servo to the 180 degree position 
         // Turn voltage high to start the period and pulse
         digitalWrite(SERVO_PIN, HIGH);

         // Delay for the length of the pulse
         delayMicroseconds(current);

         // Turn the voltage low for the remainder of the pulse
         digitalWrite(SERVO_PIN, LOW);

         // Delay this loop for the remainder of the period so we don't
         // send the next signal too soon or too late
         delayMicroseconds(lenMicroSecondsOfPeriod - current); 
    }
    Serial.println("vai voltar...");
    delay(1000);
    for(current = end; current >first; current-=increment){
         // Servos work by sending a 20 ms pulse.
         // 0.7 ms at the start of the pulse will turn the servo to the 0 degree position
         // 2.2 ms at the start of the pulse will turn the servo to the 90 degree position
         // 3.7 ms at the start of the pulse will turn the servo to the 180 degree position
         // Turn voltage high to start the period and pulse
         digitalWrite(SERVO_PIN, HIGH);

         // Delay for the length of the pulse
         delayMicroseconds(current);

         // Turn the voltage low for the remainder of the pulse
         digitalWrite(SERVO_PIN, LOW);

         // Delay this loop for the remainder of the period so we don't
         // send the next signal too soon or too late
         delayMicroseconds(lenMicroSecondsOfPeriod - current);
    }
    Serial.println("terminou...");
   delay(1000);
}

bool vaiVolta = true;
int fim = 2.5 * 1000; // 1.0 ms is 0 degrees
int inicio = 0.7 * 1000;
void teste2() {
// Manual servo control
// Copyright 2012 by Zipwhip.
// You are free to use and modify this code in your own software.
int lenMicroSecondsOfPeriod = 25 * 1000; // 20 milliseconds (ms)
// Servos work by sending a 20 ms pulse.
// 1.0 ms at the start of the pulse will turn the servo to the 0 degree position
// 1.5 ms at the start of the pulse will turn the servo to the 90 degree position
// 2.0 ms at the start of the pulse will turn the servo to the 180 degree position

   if (vaiVolta)
      vai();
   else
      volta();
   vaiVolta = !vaiVolta;

}
//Serial.println("teste2...");
void vai(){
// Turn voltage high to start the period and pulse
   digitalWrite(SERVO_PIN, HIGH);

// Delay for the length of the pulse
   delayMicroseconds(fim);

// Turn the voltage low for the remainder of the pulse
   digitalWrite(SERVO_PIN, LOW);

// Delay this loop for the remainder of the period so we don't
// send the next signal too soon or too late
   delayMicroseconds(lenMicroSecondsOfPeriod - fim);
   delay(25);
   Serial.println("foi...");
//   delay(1000);
}

void volta(){

// Turn voltage high to start the period and pulse
   digitalWrite(SERVO_PIN, HIGH);
// Delay for the length of the pulse
   delayMicroseconds(inicio);
// Turn the voltage low for the remainder of the pulse
   digitalWrite(SERVO_PIN, LOW);
// Delay this loop for the remainder of the period so we don't
// send the next signal too soon or too late
   delayMicroseconds(lenMicroSecondsOfPeriod - inicio);
   delay(25);

   Serial.println("voltou...");
//   delay(2000);
 
}

void servoPulse (int servo, int angle)
{
   int pwm = (angle*11) + 500;      // Convert angle to microseconds
   digitalWrite(servo, HIGH);
   delayMicroseconds(pwm);
   digitalWrite(servo, LOW);
   delay(20);                   // Refresh cycle of servo
}

void teste3()
{
   int servo = 2;
   int inicio = 0;
   int fim = 150;
   int step = 5;
   int angle;
   for (angle = inicio; angle <= fim; angle += step)  {
      servoPulse(servo, angle);
   }
   delay(500);
   for (angle = fim; angle >= inicio; angle -= step)  {
      servoPulse(servo, angle);
   }
   delay(1000);
}


