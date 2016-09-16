#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8);

const byte rxAddr[6] = "00001";
void setup() {
  // put your setup code here, to run once:
  radio.begin();
  radio.setRetries(15, 15);
  radio.openWritingPipe(rxAddr);
  
  radio.stopListening();
}

void loop() {
  // put your main code here, to run repeatedly:
  const char text[] = "Hello World";
  radio.write(&text, sizeof(text));
  
  delay(2000);
}
