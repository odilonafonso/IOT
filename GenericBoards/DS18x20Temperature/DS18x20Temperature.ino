// Programa : Sensor de temperatura DS18B20
// Autor : FILIPEFLOP

#include <OneWire.h>
#include <DallasTemperature.h>
//#include <LiquidCrystal.h>

// Porta do pino de sinal do DS18B20
#define ONE_WIRE_BUS 10
#define MAX_TEMP_SENSOR 10
#define LED_LOW 4
#define LED_HIGH 5
#define TEMP_LOW 64
#define TEMP_HIGH 68

// Armazena temperaturas minima e maxima
//float tempMin = 999;
//float tempMax = 0;

// Define uma instancia do oneWire para comunicacao com o sensor
OneWire oneWire(ONE_WIRE_BUS);
int maxTempSensor;
DallasTemperature sensors(&oneWire);
DeviceAddress sensor[MAX_TEMP_SENSOR];

// Inicializa o LCD
//LiquidCrystal lcd(12, 11, 7, 6, 5, 4);

void sensorConnect() {
  sensors.begin();
  // Localiza e mostra enderecos dos sensores
  maxTempSensor = sensors.getDeviceCount();
  Serial.print("Found ");
  Serial.print(maxTempSensor, DEC);
  Serial.println(" DS18B20 sensors.");
  for (int i = 0; i < maxTempSensor; i++)
    showSensorAddress(i);
}

void showSensorAddress (int i) {
if (sensors.getAddress(sensor[i], i)) {
  Serial.print("Sensor " + String(i+1) + " at:");
  showAddress (sensor[i]);
  Serial.println(".");
}
else
  Serial.println("Sensor " + String(i) + " not found..");
}

void showAddress(DeviceAddress deviceAddress){
  for (uint8_t i = 0; i < 8; i++)  {
    // Adiciona zeros se necessário
    if (deviceAddress[i] < 16)
      Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void setup(void)
{
  Serial.begin(9600);
   pinMode (LED_LOW, OUTPUT);
   pinMode (LED_HIGH, OUTPUT);
   digitalWrite(LED_LOW, LOW);
   digitalWrite(LED_HIGH, LOW);
   sensorConnect();
//  lcd.begin(16, 2);
}

void loop()
{
  Serial.println (operationSensorInput());

  delay(3000);
}

String operationSensorInput(){
  String sensorMsg = "No sensor...";
  float tempC;

  if (maxTempSensor > 0) {
    // Le a informacao dos sensores
    for (int i = 0; i < maxTempSensor; i++) {
      sensors.requestTemperatures();
      float tempC = sensors.getTempC(sensor[i]);
      sensorMsg = "TempSensor" + String(i+1) + ":" + String(tempC) + "oC;";
      if (tempC < TEMP_LOW) {
         digitalWrite(LED_LOW, HIGH);
         digitalWrite(LED_HIGH, LOW);
      }
      else if (tempC > TEMP_HIGH) {
         digitalWrite(LED_LOW, LOW);
         digitalWrite(LED_HIGH, HIGH);
      }
      else {
         digitalWrite(LED_LOW, LOW);
         digitalWrite(LED_HIGH, LOW);
      }
    }
  }
  return sensorMsg;
}
  
/*
 * codigo antigo - se quiser implementar TemMin e TempMax revise o codigo abaixo
  // Le a informacao do sensor
  sensors.requestTemperatures();
  float tempC = sensors.getTempC(sensor1);
  // Atualiza temperaturas minima e maxima
  if (tempC < tempMin)
  {
    tempMin = tempC;
  }
  if (tempC > tempMax)
  {
    tempMax = tempC;
  }
  // Mostra dados no serial monitor
  Serial.print("Sensor1 Temp C: ");
  Serial.print(tempC);
  Serial.print(" Min : ");
  Serial.print(tempMin);
  Serial.print(" Max : ");
  Serial.println(tempMax);
  
  tempC = sensors.getTempC(sensor2);
  // Atualiza temperaturas minima e maxima
  if (tempC < tempMin)
  {
    tempMin = tempC;
  }
  if (tempC > tempMax)
  {
    tempMax = tempC;
  }
  // Mostra dados no serial monitor
  Serial.print("Sensor2 Temp C: ");
  Serial.print(tempC);
  Serial.print(" Min : ");
  Serial.print(tempMin);
  Serial.print(" Max : ");
  Serial.println(tempMax);
*/  
/*
  // Mostra dados no LCD  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp.:       ");
  //Simbolo grau
  lcd.write(223);
  lcd.print("C");
  lcd.setCursor(7,0);
  lcd.print(tempC);
  lcd.setCursor(0,1);
  lcd.print("L: ");
  lcd.setCursor(3,1);
  lcd.print(tempMin,1);
  lcd.setCursor(8,1);
  lcd.print("H: ");
  lcd.setCursor(11,1);
  lcd.print(tempMax,1);
*/


