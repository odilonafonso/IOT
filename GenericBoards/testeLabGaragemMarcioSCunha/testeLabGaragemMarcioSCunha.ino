#define pino1 4                   //coloque o pino que quiser
#define pino2 5                   //coloque o pino que quiser
#define determinadoTempo 10000    //em millisegundos...
int resultadoPino1;
long momentoPino1Pressionado;
long tempoDesdePino1Pressionado;
int saida;

int estado;
#define INICIO             0
#define PRESSIONOU_PINO1   1
#define PRESSIONOU_PINO2   2
#define TEMPO_ESGOTADO     3 

void setup(){
   Serial.begin(9600);
   Serial.println("inicio...");
   pinMode(pino1, OUTPUT);
   pinMode(pino2, OUTPUT);
   digitalWrite(pino1, LOW);
   digitalWrite(pino2, LOW);
   pinMode(pino1, INPUT);
   pinMode(pino2, INPUT);
   estado = INICIO;
   saida = -1;
}

bool pressionouPino1(){
   return (digitalRead(pino1));
}

bool pressionouPino2(){
   return (digitalRead(pino2));
}

long displayEstado = 5000;
long controlaEstado = -displayEstado;
void loop(){
   if (millis() - controlaEstado > displayEstado) {
      controlaEstado = millis();
      Serial.print("Estado:");Serial.println(estado);
   }
   switch(estado) {
      case INICIO:
         if (pressionouPino1()) {
            momentoPino1Pressionado = millis();
            estado = PRESSIONOU_PINO1;
         }
         break;
      case PRESSIONOU_PINO1:
         tempoDesdePino1Pressionado = millis() - momentoPino1Pressionado;
         if (tempoDesdePino1Pressionado >= determinadoTempo) {
            saida = 0;
            estado = TEMPO_ESGOTADO;
         }
         else {
            if (pressionouPino2()) {
               saida = 1;
               estado = PRESSIONOU_PINO2;
            }
         }
         break;
      case PRESSIONOU_PINO2:
         Serial.println("pressionou Pino1 e Pino2 dentro do tempo determinado...");
         Serial.println("E agora Sr. MarcioSCunha  ??");
         Serial.println("favor definir o que fazer...");
         delay(displayEstado);
         break;
      case TEMPO_ESGOTADO:
         Serial.println("pressionou Pino1 e não pressionou Pino2 dentro do tempo determinado...");
         Serial.println("E agora Sr. MarcioSCunha  ??");
         Serial.println("favor definir o que fazer...");
         delay(displayEstado);
         break;
      default:
         Serial.println("Não deveria ter chegado aqui...");
   }
}
