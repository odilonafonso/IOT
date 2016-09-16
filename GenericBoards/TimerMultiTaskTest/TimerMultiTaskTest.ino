#include <TimerMultiTask.h>

#ifdef ARDUINO_ARCH_ESP8266
#define BOOM 1000                 //1sec - 1000 milliseconds
#define LED1 4
#define LED2 5
#else
#include <TimerOne.h>
#define BOOM 1000000    //1sec - 1000000 microseconds
#define LED1 5
#define LED2 4
#endif
int task1Id, task2Id;


/*
 * create a TimerMultiTask object 
 */
//TimerMultiTask timerMultiTask; // = TimerMultiTask(1000000);

void digitalToggle(int digitalPin){
   digitalWrite( digitalPin, !(digitalRead(digitalPin)) );
}

void setup() {
   Serial.begin(9600);
   setupOTA();
   initMQTT();
   pinMode(LED1, OUTPUT);
   pinMode(LED2, OUTPUT);

   /*
    * set a boom of 1000000 microseconds (1 sec) Arduino boards
    * or 1000 milliseconds ESP8266 boards
    */
//   timerMultiTask.setBOOM(BOOM);
   TimerMultiTask timerMultiTask = TimerMultiTask((long unsigned int)BOOM);
   
  /*
   * add a task that will be acioned each 5 boom intervals - 5 secs
   * the user callback function is userCall1
   */
   task1Id = timerMultiTask.add (5, userCall1 );
   Serial.print("task1Id:");Serial.println(task1Id);
  
  
  /*
   * add a task that will be acioned each 3 boom intervals - 3 secs
   * the user callback function is userCall2
   */
   task2Id = timerMultiTask.add (3, userCall2 );
   Serial.print("task2Id:");Serial.println(task2Id);
}

void userCall1(){
  Serial.print("task1Id:");Serial.print(task1Id);Serial.println("userCall1");
  digitalToggle(LED1);
}

void userCall2(){
  Serial.println("userCall2");
  digitalToggle(LED2);
}

void loop() {
   loopMQTT();
   loopOTA();
  // put your main code here, to run repeatedly:
  Serial.println("aqui entra o código independente das tasks...");
  Serial.println("observe que as tasks são executadas mesmo com delay de 10000 !!");
  delay(10000);
  //disable task1
//  timerMultiTask.disable (task1Id);
}

