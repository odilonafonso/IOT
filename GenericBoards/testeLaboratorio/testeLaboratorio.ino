void setup() {
/*
a primeira linha do que vc recebe no seu webserver é mais ou menos isso:

GET /?hliga=32&LA=Trocar+Horario HTTP/1.1

Supondo que voce tenha armazenado em uma variavel String 
*/
  // put your setup code here, to run once:
   String msgClient = "GET /?hliga=32&LA=Trocar+Horario HTTP/1.1";
   int horaLiga;
   String strLA;
   int index;

   //pega primeira variavel armazena em horaLiga
   if ( (index=msgClient.indexOf("hliga=")) >= 0) {
      int iFinal;
      //busca '&'
      for (iFinal=index+6; msgClient.charAt(iFinal) != '&'; iFinal++)
         ;
      horaLiga = (msgClient.substring(index+6, iFinal)).toInt();
   }

   //pega segunda variavel armazena em strLA
   if ( (index=msgClient.indexOf("LA=")) >= 0) {
      int iFinal;
      //busca ' '
      for (iFinal=index+3; msgClient.charAt(iFinal) != ' '; iFinal++)
         ;
      strLA = msgClient.substring(index+3, iFinal);
   }
   Serial.begin(9600);
   Serial.print("horaLiga="); Serial.println(horaLiga);
   Serial.print("LA="); Serial.println(strLA);

}

void loop() {
  // put your main code here, to run repeatedly:

}

