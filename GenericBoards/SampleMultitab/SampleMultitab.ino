extern int varTeste;
void setup() {
   Serial.begin(9600);
   varTeste=0;
}

void loop() {
   // aciona funcSample na tab secundaria
   varTeste++;
   funcSample();
   delay(1000);   
}

