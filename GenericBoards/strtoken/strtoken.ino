#define ARRAY_SIZE 20
char *input = "#13##15##12##20##22##20##19##25##100##15##";
int intArray[ARRAY_SIZE];
int *pointer;

int split(int *array, int arraySize, char *str, const char *s)
{
   char *token;
   
   /* get the first token */
   token = strtok(str, s);
   
   /* walk through other tokens */
   int i = 0;
   while( token != NULL ) 
   {
      Serial.println( atoi(token) );
      array[i] = atoi(token);
      if (i++ == arraySize){
        i--;
        break;
      }
      token = strtok(NULL, s);
   }
   return (i);
}
void setup(){
  Serial.begin(9600);
  int n = split (intArray, ARRAY_SIZE, input, "#");
  for (int i=0; i<n; ){
    int tot = intArray[i] + intArray[i+1];
    Serial.print(intArray[i++]); Serial.print("+");
    Serial.print(intArray[i++]); Serial.print("=");
    Serial.println(tot);
  }
}
void loop(){
}

