#define INTERVALO 2000
#define LED1 12
#define LED2 13
#define LED3 14
#define LED4 15

String comando = "x";
long inicio, now;
bool led1_aceso = false;
bool led2_aceso = false;
bool led3_aceso = false;
bool led4_aceso = false;
bool startLeds = true;
bool pegarComando = true;

void setup(){
    Serial.begin(9600);
    Serial.println("Start...");
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(LED4, OUTPUT);
    digitalWrite(LED1,LOW);
    digitalWrite(LED2,LOW);
    digitalWrite(LED3,LOW);
    digitalWrite(LED4,LOW);
    delay(500);
}

#define TIMEOUT_SERIAL 3000
String readSerial(String msg){
   String resp = "";
   Serial.setTimeout(TIMEOUT_SERIAL);
   if (msg != "")
      Serial.print(msg);
   resp = Serial.readStringUntil(10);
   if (resp != ""){
      pegarComando = false;
      Serial.print("\n{");
      Serial.print(resp);
      Serial.println ("}");
   }
   else {
      resp = comando; //mantem valor anterior 
      Serial.println();
   }
   return resp;
}

void loop() {
   if ( pegarComando ) {
      comando = readSerial("comando:");
   }
   if ( comando.indexOf("0") >= 0 ) {
      if (startLeds == true) {
         startLeds = false;
         inicio = millis();
      }
      now = millis();
      if ( (now - inicio) > INTERVALO )
         if (led1_aceso == false) {
            digitalWrite(LED1,HIGH);
            led1_aceso = true;
            Serial.println("acendeu led1");
         }
      if ( (now - inicio) > INTERVALO*2 )
         if (led2_aceso == false) {
            digitalWrite(LED2,HIGH);
            led2_aceso = true;
            Serial.println("acendeu led2");
         }
      if ( (now - inicio) > INTERVALO*3 )
         if (led3_aceso == false) {
            digitalWrite(LED3,HIGH);
            led3_aceso = true;
            Serial.println("acendeu led3");
         }
      if ( (now - inicio) > INTERVALO*4 )
         if (led4_aceso == false) {
            digitalWrite(LED4,HIGH);
            led4_aceso = true;
            Serial.println("acendeu led4");
         }
      if ( (now - inicio) > INTERVALO*5 ) {
         startLeds = true;
         pegarComando = true;
         comando = "x";
         led1_aceso = led2_aceso = led3_aceso = led4_aceso = false;
         digitalWrite(LED1,LOW);
         digitalWrite(LED2,LOW);
         digitalWrite(LED3,LOW);
         digitalWrite(LED4,LOW);
         Serial.println("apagou tudo");
      }
   }
}

