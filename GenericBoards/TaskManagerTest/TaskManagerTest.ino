typedef struct taskControl {
   long taskInit;       //millis at start of task
   long taskSize;       //millis to go
   byte taskOp;         //op to execute on task end
   byte taskPin;        //Pin 
} TaskControl;

#define TASK_FULL -1
#define TASK_CONTROL_SIZE 3
#define TASK_OP_ON HIGH
#define TASK_OP_OFF LOW

class TaskManager {
   private:
      TaskControl tc[TASK_CONTROL_SIZE];
   public:
      TaskManager();
      int Add(long taskSize, byte taskOp, byte taskPin);
      long Check(byte n);
      void Execute(byte i);
      virtual ~TaskManager();
};

TaskManager::~TaskManager(){
}

TaskManager::TaskManager(){
   for (int i=0; i < TASK_CONTROL_SIZE; i++) {
      this->tc[i].taskInit = 0;
   }
}

int TaskManager::Add(long taskSize, byte taskOp, byte taskPin){
   for (int i=0; i < TASK_CONTROL_SIZE; i++) {
      if (this->tc[i].taskInit == 0) {
         this->tc[i].taskInit = millis();
         this->tc[i].taskSize = taskSize;
         this->tc[i].taskOp = taskOp;
         this->tc[i].taskPin = taskPin;
         return i;
      }
   }
   return TASK_FULL;
}

long TaskManager::Check(byte tcNum){
   long now = millis();
   long taskEnd = this->tc[tcNum].taskInit + this->tc[tcNum].taskSize;
   if (this->tc[tcNum].taskInit == 0)
      return -1;   //invalid tc
   //task end ?
   if ( now > taskEnd ) {
      Serial.print("\ntask end:");Serial.print(tcNum);
      this->Execute(tcNum);           //execute taskOp at end of task time
      this->tc[tcNum].taskInit = 0;   //clear taskControl[i]
      return 0;                       //task end
   }
   return (taskEnd - now);            //return task remaining time
}

void TaskManager::Execute(byte i){
   Serial.print("; execute Op:");Serial.print(this->tc[i].taskOp);
   Serial.print(" pino:");Serial.println(this->tc[i].taskPin);
   digitalWrite(this->tc[i].taskPin, this->tc[i].taskOp);
}

TaskManager tm;
#define PIN_TEST_1 5
#define PIN_TEST_2 4
#define TASK_SIZE_1 5000
#define TASK_SIZE_2 7000
int tm1, tm2;
int taskOp1 = TASK_OP_ON;
int taskOp2 = TASK_OP_OFF;
void setup() {
   Serial.begin(9600);
   pinMode(PIN_TEST_1, OUTPUT);
   pinMode(PIN_TEST_2, OUTPUT);
   
   //inicializa pino task1
   digitalWrite(PIN_TEST_1, !taskOp1);
   tm1 = tm.Add(TASK_SIZE_1, taskOp1, PIN_TEST_1);
   Serial.print("tm1:");Serial.println(tm1);

   //inicializa pino task2
   digitalWrite(PIN_TEST_2, !taskOp2);
   tm2 = tm.Add(TASK_SIZE_2, taskOp2, PIN_TEST_2);
   Serial.print("tm2:");Serial.println(tm2);
}

#define DELAY_LOOP 1000
int loopDelay = 0;
void loop() {
   long now = millis();
   if ( now > (loopDelay + DELAY_LOOP) ) {
//      Serial.print("time decurred:");Serial.println(now);
      loopDelay = now;
      long ret;
      ret = tm.Check(tm1);
//      Serial.print("tm1 remaining time:");Serial.println(ret);
      if (ret == 0) {
         //initialize task1 pin, swap state
         Serial.print("tm1 now:");Serial.println(digitalRead(PIN_TEST_1));
         taskOp1 = !(digitalRead(PIN_TEST_1));
         tm1 = tm.Add(TASK_SIZE_1, taskOp1, PIN_TEST_1);
      }
      ret = tm.Check(tm2);
//      Serial.print("tm2 remaining time:");Serial.println(ret);
      if (ret == 0) {
         //initialize task2 pin, swap state
         Serial.print("tm2 now:");Serial.println(digitalRead(PIN_TEST_2));
         taskOp2 = !(digitalRead(PIN_TEST_2));
         tm2 = tm.Add(TASK_SIZE_2, taskOp2, PIN_TEST_2);
      }
      Serial.print(".");
   }
}
