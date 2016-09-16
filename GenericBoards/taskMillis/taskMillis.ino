long taskSize = 5000; //5millis ou 5secs

long taskInit;

void setup () {
    Serial.begin(115200);
    taskInit = millis();
    Serial.print("init...");
    Serial.println(taskInit);
}

bool taskCheck(long taskSize, long* taskInit) {
   if ( (millis() - *taskInit) > taskSize ) {
      *taskInit = millis();
      return true;
   }
   return false;    
}

void loop() {
   long now = millis();
   // passou-se o tempo esperado (taskSize)
   if ( taskCheck(taskSize, &taskInit) ) {
     //código a executar
     Serial.print("task end...");
     Serial.println(now);
   }
}

