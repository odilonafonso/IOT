

#define PINTEST 7
#define DELAY 3000
void setup() {
  pinMode(PINTEST, OUTPUT);
  for(uint8_t i=0; i<3; i++) {
    digitalWrite(PINTEST, HIGH); delay(100);
    digitalWrite(PINTEST, LOW);  delay(100);
  }
}

void loop() {
  for(uint8_t i=0; i<5; i++) {
    digitalWrite(PINTEST, HIGH); delay(100);
    digitalWrite(PINTEST, LOW);  delay(100);
  }
  digitalWrite(PINTEST, HIGH); delay(3000);
  digitalWrite(PINTEST, LOW); delay(3000);
}
