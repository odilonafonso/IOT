#include <TimerOne.h>
#include <TicTacBoom.h>
#include <DigitalToggle.h>

#define BOOM 250000 //250000 microsegundos cada ciclo (1/4seg)
#define LED1 6
#define LED2 7

void setup() {
//  Serial.begin(9600);
  pinMode(LED1, OUTPUT); digitalWrite(LED1,LOW); //inicia apagado
  pinMode(LED2, OUTPUT); digitalWrite(LED2,LOW); //inicia apagado

  //cria TicTacBoom com chamada a userCall1 a cada 2 ciclos de 1/4seg
  TicTacBoom ticBoom = TicTacBoom(BOOM, 2, userCall1);
  /* 
   * tambem poderia ser: 
   * //construtor apenas da classe TicTacBoom
   * TicTacBoom ticBoom = TicTacBoom(BOOM);
   * //adiciona rotina userCall1 a cada 2 ciclos 
   * ticBoom.add (2, userCall1);
   */
  ticBoom.add (3, userCall2); //aciona userCall2 a cada 3 ciclos
}

void userCall1(){
//  Serial.println("userCall1");
  digitalToggle(LED1);
}

void userCall2(){
//  Serial.println("userCall2");
  digitalToggle(LED2);
}

void loop() {
}

