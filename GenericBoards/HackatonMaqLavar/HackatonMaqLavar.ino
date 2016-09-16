/*
 * https://brainy-bits.com/tutorials/using-the-hmc5883l/
 * 
 */

#include <Servo.h>
#include <Wire.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

#define REGISTRO_TANQUE 2           //SERVO QUE FECHA REGISTRO DO TANQUE
#define INICIO 0                    //ANGULO INICIAL SERVO DO REGISTRO
#define REGISTRO_FECHADO 115
#define STEP 3
#define DELAY_STEP 19

#define PRIMEIRA_LAVAGEM 5          //LED INDICATIVO DE USO AGUA 1.A LAVAGEM
#define SEGUNDA_LAVAGEM 6           //LED INDICATIVO DE USO AGUA 2.A LAVAGEM
#define TERCEIRA_LAVAGEM 7          //LED INDICATIVO DE USO AGUA 3.A LAVAGEM
#define SOLENOIDE_WATERBOX 10       //SOLENOIDE DA BOMBONA/WATERBOX
#define SOLENOIDE_AGUA_DA_RUA 11    //SOLENOIDE DA AGUA DA RUA
#define BOTAO_USO_AGUA 12           //BOTAO QUE PROGRAMA USO DA ÁGUA

/* The I2C address of the module */
#define HMC5803L_Address 0x1E
 
/* Register address for the X Y and Z data */
#define X 3
#define Y 7
#define Z 5

int xOld, xNew, yOld, yNew, zOld, zNew;

#define varX 3
#define varY 3
#define varZ 3
#define LAVAGEM 10

int xDif, yDif, zDif;

int apertouBotaoUsoAgua;
bool apertado;

long initInterval;
long now;

#define INTERVAL_SIZE 800
#define DELAY_MEDIDAS 50

int xTotVar, yTotVar, zTotVar;

void abreRegistro(){
   myservo.attach(REGISTRO_TANQUE);  // attaches the servo on pin to the servo object
   myservo.write(INICIO);
   delay(3500);
   myservo.detach();
}

void fechaRegistro(){
   myservo.attach(REGISTRO_TANQUE);  // attaches the servo on pin to the servo object
   // goes from INICIO degrees to REGISTRO_FECHADO degrees
   for (pos = INICIO; pos <= REGISTRO_FECHADO; pos += STEP) {
      // in steps of STEP degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(DELAY_STEP);                       // waits 15ms for the servo to reach the position
   }
   delay(3500);
   myservo.detach();
}

void setup() 
{
   Serial.begin(9600);
   Serial.println("Start..");

//   abreRegistro();
//   fechaRegistro();
   
   Wire.begin();
   xOld = yOld = zOld = -1;

   xTotVar = yTotVar = zTotVar = 0;
     
   /* Initialise the module */ 
   Init_HMC5803L();
   initInterval = millis();

   pinMode(PRIMEIRA_LAVAGEM,OUTPUT);
   digitalWrite(PRIMEIRA_LAVAGEM, LOW);
   pinMode(SEGUNDA_LAVAGEM,OUTPUT);
   digitalWrite(SEGUNDA_LAVAGEM, LOW);
   pinMode(TERCEIRA_LAVAGEM,OUTPUT);
   digitalWrite(TERCEIRA_LAVAGEM, LOW);
   pinMode(BOTAO_USO_AGUA, INPUT_PULLUP);
//   digitalWrite(BOTAO_USO_AGUA, LOW);
   apertouBotaoUsoAgua = 0;
}

void verificaBotaoUsoAgua() {
   int botao = digitalRead(BOTAO_USO_AGUA);
   Serial.print("digitalRead(BOTAO_USO_AGUA) ");Serial.println(botao);
   if (botao == HIGH)
      apertado=true;
   else if(apertado==true){
      if (apertouBotaoUsoAgua == 3)
         apertouBotaoUsoAgua = 1;
      else
         apertouBotaoUsoAgua++;
      apertado=false;
   }
   Serial.print("botaoUsoAgua:");Serial.println(apertouBotaoUsoAgua);
   digitalWrite(PRIMEIRA_LAVAGEM, LOW);
   digitalWrite(SEGUNDA_LAVAGEM, LOW);
   digitalWrite(TERCEIRA_LAVAGEM, LOW);
   switch (apertouBotaoUsoAgua) {
      case 1:
         digitalWrite(PRIMEIRA_LAVAGEM, HIGH);
         break;
      case 2:
         digitalWrite(SEGUNDA_LAVAGEM, HIGH);
         break;
      case 3:
         digitalWrite(TERCEIRA_LAVAGEM, HIGH);
         break;
   }
}

bool centrifuga_1 = false;
bool centrifuga_2 = false;
bool centrifuga_3 = false;

/*
 * fluxo lógico
 * Leds:
 * PRIMEIRA_LAVAGEM
 * SEGUNDA_LAVAGEM
 * TERCEIRA_LAVAGEM
 * indicam qual água será recolhida
 */
void loop() {
//   abreRegistro();
//   fechaRegistro();
   verificaBotaoUsoAgua();
   //verifica se botao de pegar água na primeira centrifugação foi acionado 
   centrifuga_1 = centrifuga_2 = centrifuga_3 = false;
   centrifuga_1 = (digitalRead(PRIMEIRA_LAVAGEM) == HIGH ? true : false);
   centrifuga_2 = (digitalRead(SEGUNDA_LAVAGEM) == HIGH ? true : false);
   centrifuga_3 = (digitalRead(TERCEIRA_LAVAGEM) == HIGH ? true : false);
//   checaMaquina();
   delay(100);
}

void checaMaquina() {
   now = millis();

   if ( (now - initInterval) > INTERVAL_SIZE ) {
      int totvar = xTotVar + yTotVar + zTotVar;
      if (totvar > 0) {
         Serial.print("TotVar=");Serial.print(totvar);
         if (totvar <= LAVAGEM) {
            digitalWrite(SOLENOIDE_WATERBOX, LOW);
            Serial.println("Lavagem");
         }
         else {
//            totCentrifuga++;
            if (centrifuga_1 == true)
               digitalWrite(SOLENOIDE_WATERBOX, HIGH);
            Serial.print("Centrifugacao");Serial.println(apertouBotaoUsoAgua);
         }
      }
//      Serial.print("xTotVar=");Serial.print(xTotVar);
//      Serial.print(" yTotVar=");Serial.print(yTotVar);
//      Serial.print(" zTotVar=");Serial.println(zTotVar);
      xTotVar = yTotVar = zTotVar = 0;
      initInterval = now;
   }
   /* Read each sensor axis data and output to the serial port */
   xNew = HMC5803L_Read(X);
   yNew = HMC5803L_Read(Y);
   zNew = HMC5803L_Read(Z);

   if (xOld == -1) {
      xOld = xNew;
      yOld = yNew;
      zOld = zNew;
      xTotVar = yTotVar = zTotVar = 0;
   }

   if (xNew > xOld)
      xDif = xNew - xOld;
   else
      xDif = xOld - xNew;
   if (xDif > varX) {
      xTotVar++;
//      Serial.print("xOld:");Serial.print(xOld);
//      Serial.print(" xNew:");Serial.print(xNew);
//      Serial.print(" varX:");Serial.println(xDif);
   }

   if (yNew > yOld)
      yDif = yNew - yOld;
   else
      yDif = yOld - yNew;
   if (yDif > varY) {
      yTotVar++;
//      Serial.print("yOld:");Serial.print(yOld);
//      Serial.print(" yNew:");Serial.print(yNew);
//      Serial.print(" varY:");Serial.println(yDif);
   }

   if (zNew > zOld)
      zDif = zNew - zOld;
   else
      zDif = zOld - zNew;
   if (zDif > varZ) {
      zTotVar++;
//      Serial.print("zOld:");Serial.print(zOld);
//      Serial.print(" zNew:");Serial.print(zNew);
//      Serial.print(" varZ:");Serial.println(zDif);
   }

      xOld = xNew;
      yOld = yNew;
      zOld = zNew;
/*
   Serial.print(xNew);   Serial.print(" ");
   Serial.print(yNew);   Serial.print(" ");
   Serial.print(zNew);
*/  

  /* Wait a little before reading again */
  delay(DELAY_MEDIDAS);
}
 
 
/* This function will initialise the module and only needs to be run once
   after the module is first powered up or reset */
void Init_HMC5803L(void)
{
  /* Set the module to 8x averaging and 15Hz measurement rate */
  Wire.beginTransmission(HMC5803L_Address);
  Wire.write(0x00);
  Wire.write(0x70);
          
  /* Set a gain of 5 */
  Wire.write(0x01);
  Wire.write(0xA0);
  Wire.endTransmission();
}
 
 
/* This function will read once from one of the 3 axis data registers
and return the 16 bit signed result. */
int HMC5803L_Read(byte Axis)
{
  int Result;
  
  /* Initiate a single measurement */
  Wire.beginTransmission(HMC5803L_Address);
  Wire.write(0x02);
  Wire.write(0x01);
  Wire.endTransmission();
  delay(6);
  
  /* Move modules the resiger pointer to one of the axis data registers */
  Wire.beginTransmission(HMC5803L_Address);
  Wire.write(Axis);
  Wire.endTransmission();
   
  /* Read the data from registers (there are two 8 bit registers for each axis) */  
  Wire.requestFrom(HMC5803L_Address, 2);
  Result = Wire.read() << 8;
  Result |= Wire.read();
 
  return Result;
}
