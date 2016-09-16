//#include <DigitalToggle.h>

int pin2=2;   //Define pino do led a piscar
unsigned long ledLastDelay=0; //variavel auxiliar para guardar ultimo "toggle"

void setup() {
  pinMode(pin2, OUTPUT);  // indica que o pino e uma saida
  digitalWrite(pin2, LOW); //começa desligado
}

void loop()  {
  troca(pin2);
}

void troca(int led){
  digitalWrite(led, !digitalRead(led));
//  digitalToggleDelay(led, 1500, &ledLastDelay);  // inverte o estado do pino a cada passagem pela linha
}

