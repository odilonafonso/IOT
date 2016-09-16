
char str[]= "robuino/action/jarduino/odilon";
char *p;

String getStringDelimitedBy (char **p, char c){
   String s;
   while (**p != c && **p != '\0') {
//      Serial.print(**p);
      s += *(*p)++;
//      (*p)++;
   }
//   Serial.println(s);
   return s;
}

void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600);
   p = str;
   do {
      Serial.println(getStringDelimitedBy(&p,'/'));
   } while (*p++ != '\0');
   
   p = str;
   Serial.println(getStringDelimitedBy(&p,'/'));
   p++;
   Serial.println(getStringDelimitedBy(&p,'/'));
   p++;
   Serial.println(getStringDelimitedBy(&p,'/'));
   p++;
   Serial.println(getStringDelimitedBy(&p,'/'));
   
}

void loop() {
  // put your main code here, to run repeatedly:

}
