#include <ESP8266WiFi.h>

#define LED 2
void test2(int);
bool test3(int, int);
void test1(){
  pinMode(LED, OUTPUT);
  test2(LED);
  test3(LED, LOW);
}
void test2(int led){
  digitalWrite(led, HIGH);
  delay(500);
}
bool test3(int led, int what){
  digitalWrite(led, what);
  delay(500);
  return true;
}
void setup() {
  // put your setup code here, to run once:
  test1();
}

void loop() {
  // put your main code here, to run repeatedly:
  test2(LED);
  test3(LED, LOW);
}

