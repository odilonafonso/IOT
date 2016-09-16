#define PIN 2
void setup() {
pinMode(PIN, OUTPUT);
}
void loop() {
digitalWrite(PIN, !digitalRead(PIN));
delay(1000);
}

