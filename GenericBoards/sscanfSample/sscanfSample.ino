/*
 *  Dimon;
 *  sscanf() não funciona no arduino:
 *  http://forum.arduino.cc/index.php?topic=43247.0
 *  leia até o fim esse link acima
 *  use atof() conforme exemplo abaixo
 *  Boa sorte !!
 */

float ff;
int i;
char str[] = "34 41.23";

void setup() {
   Serial.begin(9600);
   sscanf( str, "%d %f", &i, &ff );
   Serial.print("var int:");Serial.println(i);
   Serial.print("var float (sscanf):");Serial.println(String(ff, 5));
   //sscanf não funciona no arduino !!!
   ff = atof("23.456");
   Serial.print("var float (atof):");Serial.println(String(ff, 5));
}

void loop() {
  // put your main code here, to run repeatedly:

}

