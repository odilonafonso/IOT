//retorna o nome em ingles do número enviado
String number(int n){
    switch (n) {
        case 0:
            return "zero";
        case 1:
            return "one";
        case 2:
            return "two";
        case 3:
            return "three";
        case 4:
            return "four";
        case 5:
            return "five";
        case 6:
            return "six";
        default:
            return "what?";
    }
}
    
String target, respToClient;
int digitalPin;

void buscaComando(String readString) {
bool enviouAnalogico = false;
   Serial.println("----------------------");
   Serial.println(readString);
   //verifica se cliente enviou comando a algum pino analogico
   for (int i=0; i<5; i++) {
       //A0 = 14, A1 = 15, ..., A4 = 18 (i+14)
       digitalPin = i + 14;
       respToClient = "a" + number(i);
       //pino analogivo high ? "aNhigh"
       target = "a" + String(i) + "high";
       Serial.print(target); Serial.print(" ");
       if (readString.indexOf(target) >= 0) {
           digitalWrite(digitalPin, HIGH);
           respToClient += "on";
           enviouAnalogico = true;
           break;
       }
       //pino analogivo low ? "aNlow"
       target = "a" + String(i) + "low";
       Serial.print(target); Serial.print(" ");
       if (readString.indexOf(target) >= 0) {
           digitalWrite(digitalPin, LOW);
           respToClient += "off";
           enviouAnalogico = true;
           break;
       }
   }
   Serial.println(" ");
        
   bool enviouDigital = false;
   //verifica se cliente enviou comando a algum pino digital
   if (enviouAnalogico == false) //so testa digital se não tiver encontrado comando analogico;
       for (int i=2; i<7; i++) {
           digitalPin = i;
           respToClient = "d" + number(i);
           //pino digital high ? "dNhigh"
           target = "d" + String(i) + "high";
           Serial.print(target); Serial.print(" ");
           if (readString.indexOf(target) >= 0) {
               digitalWrite(digitalPin, HIGH);
               respToClient += "on";
               enviouDigital = true;
               break;
           }
           //pino digital low ? "dNlow"
           target = "d" + String(i) + "low";
           Serial.print(target); Serial.print(" ");
           if (readString.indexOf(target) >= 0) {
               digitalWrite(digitalPin, LOW);
               respToClient += "off";
               enviouDigital = true;
               break;
           }
       }
       if (!enviouDigital && !enviouAnalogico)
          respToClient = "comando incorreto";
       Serial.println(" ");
}

void teste(String x) {
   buscaComando(x);
   Serial.print("respToClient:"); Serial.println(respToClient); Serial.println();
   delay(15000);
}

void setup() {
   Serial.begin(9600);
   teste("pimba na gorduchinha");
}

void loop() {
   teste("blablaa4high");
   teste("pumbad5highlalal");
   teste("d2low");
}

