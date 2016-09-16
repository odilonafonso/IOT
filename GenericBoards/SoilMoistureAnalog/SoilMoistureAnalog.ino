    const int VAL_PROBE = 0; // Analog pin 0
    const int MOISTURE_LEVEL = 250; // the value after the LED goes ON

    #define LEDSTATE 13
    #define SOIL_MOISTURE_POWER_PIN 16
     
    void setup() {
        Serial.begin(9600);
        pinMode(16, OUTPUT);
    }
     
    void LedState(int state) {
        digitalWrite(LEDSTATE, state);
    }
     
    void loop() {
      digitalWrite(SOIL_MOISTURE_POWER_PIN, HIGH);
        int analogMoisture = analogRead(VAL_PROBE);
      int digitalMoisture = digitalRead(VAL_PROBE);
     
        Serial.print("AnalogRead:");Serial.println(analogMoisture);
        Serial.print("DigitalRead:");Serial.println(digitalMoisture);
     
      digitalWrite(SOIL_MOISTURE_POWER_PIN, LOW);

        if(analogMoisture > MOISTURE_LEVEL) {
            LedState(HIGH);
        } else   {
            LedState(LOW);
        }
        delay(3000);
    }
