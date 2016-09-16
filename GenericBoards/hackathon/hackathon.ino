
int estado = 0;
int numeroDescartes = 1;
int contadorDescartesFeitos = 0;
int numLavagens;
bool apertado = false;
#define MAX_NUMERO_DESCARTES 3

/*
   constantes para ativar-desativar relay
   se relay ativar com LOW ou com HIGH
   essas constantes deverão ser invertidas
   o relay que estamos usando ativa com LOW
*/
#define RELAY_ON LOW
#define RELAY_OFF HIGH

//constantes utilizadas para os sensores de nivel
#define BOMBONA_TEM_AGUA 500        //qquer medida acima
#define MAQUINA_CHEIA 350           //qquer medida acima

#define LIQUID_FLUX_SENSOR 2        //SENSOR DO FLUXO DE ÁGUA
#define REGISTRO_TANQUE 3           //SERVO QUE FECHA REGISTRO DO TANQUE
#define BOTAO_NUMERO_DESCARTES 4    //BOTAO QUE CONTROLA NUMERO DE DESCARTES (A1)

// leds
#define PRIMEIRA_LAVAGEM 5          //LED INDICATIVO DE USO AGUA 1.A LAVAGEM
#define SEGUNDA_LAVAGEM 6           //LED INDICATIVO DE USO AGUA 2.A LAVAGEM
#define TERCEIRA_LAVAGEM 7          //LED INDICATIVO DE USO AGUA 3.A LAVAGEM
#define LED_MEDIDOR_NIVEL 8         //LED INDICATIVO MEDIDOR NIVEL BOMBONA
#define LIGA_DESLIGA 9              //botao de liga-desliga

//relays
#define SOLENOIDE_SABESP 10         //SOLENOIDE DA SABESP (OU BOMBA)
#define BOMBA_WATERBOX 11           //SOLENOIDE DA BOMBONA/WATERBOX (OU BOMBA)
#define BOMBA_SAIDA_MAQUINA 12      //BOMBA SAIDA DE AGUA DA MAQUINA
#define MAQUINA_DE_LAVAR 13         //LIGA MAQUINA DE LAVAR (VIRA-VIRA) 

//sensores de nivel
#define MEDIDOR_NIVEL_BOMBONA A0    //medidor nivel bombona - analogo 0
#define NIVEL_AGUA_MAQUINA A1       //SENSOR DO NIVEL DA MAQUINA
#define POWER_SENSORES A2           //pino para enviar power aos sensores

// estados
#define DESLIGADO 0
#define INICIADO 1
#define ENCHENDO_MAQUINA 2
#define MAQUINA_BATENDO 3           // maquina batendo
#define DESCARTE 4                  //descarte da água da máquina
#define MOLHO 5
#define FIM_CICLO 6

extern float flowRate;
extern int servoPosition;

/*
 * Funçoes para ligar/desligar o power enviado aos sensores
 */
void LigaSensores(){
   pinMode(POWER_SENSORES, OUTPUT);
   digitalWrite(POWER_SENSORES, HIGH);
}
void DesligaSensores(){
   pinMode(POWER_SENSORES, OUTPUT);
   digitalWrite(POWER_SENSORES, LOW);
}


/*
   Funções auxiliares reservatório de água re-aproveitada
*/
bool BombaWaterboxLigada() {
  int nivel = digitalRead(BOMBA_WATERBOX);
  return (nivel == RELAY_ON);
}

void LigaBombaWaterbox() {
  pinMode(BOMBA_WATERBOX, OUTPUT);
  digitalWrite(BOMBA_WATERBOX, RELAY_ON);
}

void DesligaBombaWaterbox() {
  pinMode(BOMBA_WATERBOX, OUTPUT);
  digitalWrite(BOMBA_WATERBOX, RELAY_OFF);
}

/*
 * esta porcaria de sensor está funcionando ao contrario;
 * quando está seco, le 1024 molhado le 31
 * por isso a conta abaixo
 */
int nivelBombona;       //global para facilitar
void NivelBombona(){
   LigaSensores();
   delay(150);
   nivelBombona = 1024 - analogRead(MEDIDOR_NIVEL_BOMBONA);
   DesligaSensores();
}

bool TemAguaWaterbox() {
   int nivel;
   NivelBombona();
   //   Serial.print("Medidor ANALOGICO:"); Serial.println(moisture);
   if (nivelBombona > BOMBONA_TEM_AGUA) {
      nivel = HIGH;
   } else   {
      nivel = LOW;
   }
   //   nivel = digitalRead(MEDIDOR_NIVEL);
   //   Serial.print("Medidor_nivel:"); Serial.println(nivel);
   digitalWrite(LED_MEDIDOR_NIVEL, nivel);
   return (nivel == HIGH);
}


/*
   Funções auxiliares Solenoide Sabesp
*/
bool SolenoideSabespAberto() {
  int nivel = digitalRead(SOLENOIDE_SABESP);
  return (nivel == RELAY_ON);
}

void AbreSolenoideSabesp() {
  pinMode(SOLENOIDE_SABESP, OUTPUT);
  digitalWrite(SOLENOIDE_SABESP, RELAY_ON);
}

void FechaSolenoideSabesp() {
  pinMode(SOLENOIDE_SABESP, OUTPUT);
  digitalWrite(SOLENOIDE_SABESP, RELAY_OFF);
}


/*
   Funções auxiliares ciclo de descarte de água da máquina de lavar
*/
void FechaSaidaMaquinalavar() {
   pinMode(BOMBA_SAIDA_MAQUINA, OUTPUT);
   digitalWrite(BOMBA_SAIDA_MAQUINA, RELAY_OFF);
}

void AbreSaidaMaquinalavar() {
   pinMode(BOMBA_SAIDA_MAQUINA, OUTPUT);
   digitalWrite(BOMBA_SAIDA_MAQUINA, RELAY_ON);
}

//verificar direitinho como o sensor está detectando
int nivelMaquina;
bool MaquinaEncheu() {
   LigaSensores();
   delay(120);
   nivelMaquina = analogRead(NIVEL_AGUA_MAQUINA);
   DesligaSensores();
   return (nivelMaquina >= MAQUINA_CHEIA);
}

bool SaidaMaquinaLavarAberta() {
   pinMode(BOMBA_SAIDA_MAQUINA, INPUT);
   return ( digitalRead(BOMBA_SAIDA_MAQUINA) == RELAY_ON ? true : false );
}


/*
   Funções relacionadas ao ciclo de lavagem de roupa
*/
#define TIME_MAQ_LAVAR 9000      //tempo que a maquina vai bater - millis
long startLavagem;
void LigaMaquinaLavar() {
  pinMode(MAQUINA_DE_LAVAR, OUTPUT);
  digitalWrite(MAQUINA_DE_LAVAR, RELAY_ON);
  startLavagem = millis();
}

void DesligaMaquinaLavar() {
  pinMode(MAQUINA_DE_LAVAR, OUTPUT);
  digitalWrite(MAQUINA_DE_LAVAR, RELAY_OFF);
}

bool TerminouLavagem() {
  long timeLavagem = millis() - startLavagem;
  if (timeLavagem > TIME_MAQ_LAVAR)
    return true;
  else
    return false;
}

bool MaquinaLigada() {
  pinMode(MAQUINA_DE_LAVAR, INPUT);
  return ( digitalRead(MAQUINA_DE_LAVAR) == RELAY_ON ? true : false );
}


/*
   Funções relacionadas ao tempo de molho
*/
#define TIMER_MOLHO 5000
long initTimerMolho;
void IniciaMolho() {
  initTimerMolho = millis();
}
bool TerminouMolho() {
  if ( (millis() - initTimerMolho) >= TIMER_MOLHO )
    return true;
  else
    return false;
}

void EncerraMolho() {

}


/*
   Funções relacionadas ao fluxo de saida de água
*/
#define FLUX_DETECTOR 1                 //nivel acima do qual deduz-se que há passagem de água
#define TIMER_FLUXO_INTERROMPIDO 2000   //tempo para confirmar que fluxo de água terminou
long iniciaFluxo;
#define INICIO_FLUXO 5000
bool TemFluxoSaida() {

   /*
     obtem leitura de fluxo de água
   */
   checkLFS();

   //   Serial.print("fluxo:");Serial.println("flowRate");
   return (flowRate > FLUX_DETECTOR);
}

long inicioFluxoInterrompido;
bool fluxoAguaInterrompido;      //contador de tempo de fluxo interrompido

void IniciaDescarte() {
   iniciaFluxo = millis();
   fluxoAguaInterrompido = false;
}

void EncerraDescarte() {
}

bool TerminouDescarte() {
   //está saindo água ?
   if ( !((millis() - iniciaFluxo) > INICIO_FLUXO) )
      return false;
      
   if ( TemFluxoSaida() ) {
      /*
       independente do estado atual do fluxoAguaInterromnpido,
       reseta para que reinicie o contador de tempo
      */
      fluxoAguaInterrompido = false;
      return false;
   }
   //acabou de parar de sair água ?
   if (!fluxoAguaInterrompido) {
      fluxoAguaInterrompido = true;
      //inicia (ou reinicia) contador de tempo de fluxo interrompido
      inicioFluxoInterrompido = millis();
      return false;
   }
   //verifica se está sem fluxo por tempo suficiente para determinar fim de fluxo de descarte
   if ( (millis() - inicioFluxoInterrompido) >= TIMER_FLUXO_INTERROMPIDO ) {
      return true;
   }
}


/*
   Inicia estado e verifica todos relays e posicionamento do registro
*/
void ReiniciaFluxo() {
   estado = DESLIGADO;
   numeroDescartes = 1;
   contadorDescartesFeitos = 0;
   
   if (MaquinaLigada()) {
    DesligaMaquinaLavar();
   }
   if (BombaWaterboxLigada()) {
    DesligaBombaWaterbox();
   }
   if (SolenoideSabespAberto()) {
    FechaSolenoideSabesp();
   }
   if (SaidaMaquinaLavarAberta()) {
    FechaSaidaMaquinalavar();
   }
   if (!RegistroAberto()) {
    AbreRegistro();
   }
}

void initB(int n) {
  pinMode(n, OUTPUT);
  digitalWrite(n, LOW);
}


void TestaSensores(){
   for (int i=0; i<10; i++){
      NivelBombona();
      MaquinaEncheu();
      Serial.print("nivelMaquina:");Serial.print(nivelMaquina);
      Serial.print(" nivelBombona:");Serial.println(nivelBombona);
      delay (1000);
   }
}


void setup() {
   Serial.begin(9600);
   Serial.println("Start..");

   initB(LIGA_DESLIGA);
   initB(BOTAO_NUMERO_DESCARTES);
   initB(LED_MEDIDOR_NIVEL);
   initB(PRIMEIRA_LAVAGEM);
   initB(SEGUNDA_LAVAGEM);
   initB(TERCEIRA_LAVAGEM);

   TestaSensores();
   
   /*
     para verificar se os relays estão funcionando;
     em produção comentar.
   */
//   LigaBombaWaterbox();
//   AbreSolenoideSabesp();
//   AbreSaidaMaquinalavar();
//   LigaMaquinaLavar();

   //inicializa servo controlador registro tanque (água descartada)
   setup_servo();

   //inicializa sensor de fluxo de água da saida da máquina
   setupLFS();

   //reinicia relays fechados e registro aberto
   ReiniciaFluxo();
}


/*
   checa botão que determina o numero de descartes e liga/desliga leds
*/
void checkNumeroDescartes() {
   int botao = digitalRead(BOTAO_NUMERO_DESCARTES);
   if (botao == LOW)
      apertado = true;
   else if (apertado == true) {
      if (numeroDescartes == MAX_NUMERO_DESCARTES)
         numeroDescartes = 1;
      else
         numeroDescartes++;
      apertado = false;
   }
   /*
     num Lavagens por default será definido como numeroDescartes + 1;
     para mudar isso é necessário novo botão
   */
   numLavagens = numeroDescartes + 1;
   digitalWrite(PRIMEIRA_LAVAGEM, LOW);
   digitalWrite(SEGUNDA_LAVAGEM, LOW);
   digitalWrite(TERCEIRA_LAVAGEM, LOW);
   switch (numeroDescartes) {
      case 1:
         digitalWrite(PRIMEIRA_LAVAGEM, HIGH);
         break;
      case 2:
         digitalWrite(PRIMEIRA_LAVAGEM, HIGH);
         digitalWrite(SEGUNDA_LAVAGEM, HIGH);
         break;
      case 3:
         digitalWrite(PRIMEIRA_LAVAGEM, HIGH);
         digitalWrite(SEGUNDA_LAVAGEM, HIGH);
         digitalWrite(TERCEIRA_LAVAGEM, HIGH);
         break;
   }
}


/*
   Função para acompanhar andamento através de display
*/
long unsigned oldTimeToDisplay = millis();
#define TIMER_TO_DISPLAY 2000
void DisplayEstado() {
   if ( (millis() - oldTimeToDisplay) > TIMER_TO_DISPLAY ) {
      oldTimeToDisplay = millis();
      switch (estado) {
         case (DESLIGADO):
            Serial.print("Off;");
            break;
         case (INICIADO):
            Serial.print("Start;");
            break;
         case (ENCHENDO_MAQUINA):
            Serial.print("NivMaq:"); Serial.print(nivelMaquina);
            Serial.print(";");
            break;
         case (MAQUINA_BATENDO):
            Serial.print("Batendo;");
            break;
         case (MOLHO):
            Serial.print("Molho;");
            break;
         case (DESCARTE):
            Serial.print("Descarte "); Serial.print(contadorDescartesFeitos);
            Serial.print(";");
            break;
         case (FIM_CICLO):
            Serial.print("End Ciclo;");
            break;
         default:
            Serial.println("Estado não previsto;");
            break;
      }
      Serial.print(" IN:");Serial.print(SolenoideSabespAberto() ? "SBSP" : "WB");
      Serial.print(" NivMQ:");Serial.print(nivelMaquina);
      Serial.print(" Flx:"); Serial.print(String(flowRate));
      Serial.print(" Reg::"); Serial.print(RegistroAberto() ? "ON" : "OFF");
      NivelBombona();
      Serial.print(" NivWB:"); Serial.print(nivelBombona);
      Serial.print(" Desc:"); Serial.print(numeroDescartes);
      Serial.print("/:"); Serial.print(contadorDescartesFeitos);
      Serial.print(" Lav:"); Serial.print(numLavagens);
      Serial.println();
   }
}


void loop() {
   DisplayEstado();

   /*
     função retirada temporariamente; só tem sentido no contexto
     de checagem se está havendo fluxo de saida de água
   
     checkLFS();
   
   */

   //  PRINCIPAL
   int botao;
   switch (estado) {
      case (DESLIGADO):
         //mudança de numero de descartes só quando estado = DESLIGADO
         checkNumeroDescartes();
         botao = digitalRead(LIGA_DESLIGA);
         if ( botao == HIGH )
           estado = INICIADO;
         break;

      case (INICIADO):
         if ( TemAguaWaterbox() ) {
            LigaBombaWaterbox();
            if ( SolenoideSabespAberto() )
               FechaSolenoideSabesp();
         }
         else {
            AbreSolenoideSabesp();
         }
         estado = ENCHENDO_MAQUINA;
         break;

      case (ENCHENDO_MAQUINA):
         if ( MaquinaEncheu() ) {
            delay(2000);
            LigaMaquinaLavar();
            estado = MAQUINA_BATENDO;
//           if ( BombaWaterboxLigada() )
             DesligaBombaWaterbox();
//           if ( SolenoideSabespAberto() )
             FechaSolenoideSabesp();
         }
         else if ( !TemAguaWaterbox() ) {
            if ( BombaWaterboxLigada() )
               DesligaBombaWaterbox();
           delay(3000);
            if ( !SolenoideSabespAberto() )
               AbreSolenoideSabesp();
         }
         break;

      case (MAQUINA_BATENDO) :
         /*
            controle de MAQUINA_BATENDO é por timer
         */
         if ( TerminouLavagem() ) {
           DesligaMaquinaLavar();
           /*
              antes de iniciar o molho verifica se proximo descarte será feito com registro aberto ou fechado
           */
           if (contadorDescartesFeitos >= numeroDescartes && RegistroAberto()) {
             FechaRegistro();
           }
           IniciaMolho();
           estado = MOLHO;
         }
         break;

      case (MOLHO):
         if (TerminouMolho()) {
            EncerraMolho();
            AbreSaidaMaquinalavar();
            delay(3000);
            IniciaDescarte();
            estado = DESCARTE;
         }
         break;

      case (DESCARTE):
         if ( TerminouDescarte() ) {
            FechaSaidaMaquinalavar();
            contadorDescartesFeitos++;
            Serial.print("terminou descarte contador = ");
            Serial.print(contadorDescartesFeitos);
            Serial.print(" numero de lavagens: ");
            Serial.println(numLavagens);
            delay(2000);
            if (contadorDescartesFeitos >= numLavagens)
               estado = FIM_CICLO;
            else
               estado = INICIADO;
         }
         break;

      case (FIM_CICLO):
         //reinicia relays fechados e registro aberto
         ReiniciaFluxo();
         break;

      default:
         break;
   } // fim switch
}

