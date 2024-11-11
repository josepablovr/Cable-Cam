//INSTITUTO TECNOLOGICO DE COSTA RICA
//MADE BY: JOSÉ PABLO VÁSQUEZ ROJAS
//CONTACT: josepablo2106@estudiantec.cr

#include <NewPing.h>

//ULTRASONIC SENSORS
#define TRIGGER_PIN1  11
#define ECHO_PIN1     8
#define MAX_DISTANCE 200
#define MIN_DISTANCE 30
#define TRIGGER_PIN2  10
#define ECHO_PIN2     9
#define SONAR_NUM     2 // Number of sensors.
#define PING_INTERVAL 33 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).
unsigned long pingTimer[2]; // Holds the times when the next ping should happen for each sensor.
unsigned int cm[2];         // Where the ping distances are stored.
uint8_t currentSensor = 0;          // Keeps track of which sensor is active.
unsigned int Sensor_Block[2];

NewPing sonar[2] = {
  NewPing(TRIGGER_PIN1, ECHO_PIN1, MAX_DISTANCE),
  NewPing(TRIGGER_PIN2, ECHO_PIN2, MAX_DISTANCE)
};





//START
#define MAXSTEPS_START 50
#define START_DURATION 5000 //ms
int STEPS_START_DURATION = START_DURATION/MAXSTEPS_START;
int VEL = 0;
unsigned long Timer = 0;
int Step_ST = 1;

//STOP
#define MAXSTEPS_STOP 6   
#define STOP_DURATION 600 //ms
int STEPS_STOP_DURATION = STOP_DURATION/MAXSTEPS_STOP;
unsigned long Timer2 = 0;
int Step_SP = 1;

//RUN VARIABLES
int dir = 1;
bool ON = false;
bool STOP = false;

int Speed_Value = 0;


//H-BRIDGE INPUTS
const int R1 = 6; 
const int L1 = 5; 
const int R2 = 3;
const int L2 = 4;



void setup() {
  //TCCR0B = TCCR0B & B11111000 | B00000010; // for PWM frequency of 7812.50 Hz
  TCCR0B = TCCR0B & B11111000 | B00000011; // for PWM frequency of 976.56 Hz (The DEFAULT)
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps  
  pingTimer[0] = millis() + 75;
  pingTimer[1] = pingTimer[0] + PING_INTERVAL;
  Sensor_Block[0] = 0;
  Sensor_Block[1] = 0;


  
}

void loop() {
  // send data only when you receive data:

  char buf[12];
  char Valor[3];
  int byteReceived;
  if (Serial.available() > 0) {
    // read the incoming byte:
    byteReceived = Serial.readBytesUntil('\n', buf, 10);   // Keep room for null
    buf[byteReceived] = '\0';   // At this point buf[] hold the ASCII for the int that was entered
    
    if (buf[0] == 'A'){  //RUN
      Serial.print("ON");
      ON = true;
      STOP = false;
      VEL = 0;}
      

    if (buf[0] == 'B'){  //STOP      
      VEL = 0;
      ON = false;
      STOP = true;
    }
    
    
    if (buf[0] == 'S'){  //CHANGE SPEED      
      
      Valor[0] = buf[2];
      Valor[1] = buf[3];
      Valor[2] = buf[4];
      Speed_Value = atoi(Valor);
      Step_ST = Speed_Value/MAXSTEPS_START;
      
      
    }
    if (buf[0] == 'C'){ //CHANGE DIRECTION          
      dir = !dir;  
      
    }
          
  }
  
  

  
  //CHECK SENSORS
  if (ON == true){
    for (uint8_t i = 0; i < SONAR_NUM; i++) { // Loop through all the sensors.
    if (millis() >= pingTimer[i]) {         // Is it this sensor's time to ping?
      pingTimer[i] += PING_INTERVAL * SONAR_NUM;  // Set next time this sensor will be pinged.
      if (i == 0 && currentSensor == SONAR_NUM - 1) oneSensorCycle(); // Sensor ping cycle complete, do something with the results.
        sonar[currentSensor].timer_stop();          // Make sure previous timer is canceled before starting a new ping (insurance).
        currentSensor = i;                          // Sensor being accessed.
        cm[currentSensor] = 200;                      // Make distance zero in case there's no ping echo for this sensor.
        sonar[currentSensor].ping_timer(echoCheck); // Do the ping (processing continues, interrupt will call echoCheck to look for echo).
    }
    
  }
    
  }

  //RUN 
  if ((ON == true)&&(VEL < Speed_Value)){
    Start(); 
       
  }

  
  if (STOP == true){
    if (VEL == 0){
      Aboslute_Stop();
      STOP = false;
      Step_SP = 1;
      VEL = 0;
    }
    //PROGRESSIVE STOP
    else{
      Stop();    
      
    }
}
  
}


int TurnRight(int speed){
  digitalWrite(R2, HIGH);
  analogWrite(R1, speed);
  digitalWrite(L1, LOW);
  digitalWrite(L2, LOW);
}
int TurnLeft(int speed){
  digitalWrite(L2, HIGH);
  analogWrite(L1, speed);
  digitalWrite(R1, LOW);
  digitalWrite(R2, LOW);
}


int Run(int Speed) { 
      if(dir == 1){      
        TurnRight(Speed);
      }
      else {
        TurnLeft(Speed); 
      }
  
}
int Aboslute_Stop(){
  digitalWrite(L2, LOW);
  digitalWrite(L1, LOW);
  digitalWrite(R1, LOW);
  digitalWrite(R2, LOW);
}


int Start(){
   unsigned long currentTime = millis();
    if (currentTime-Timer > STEPS_START_DURATION){
       if (VEL < Speed_Value){
        VEL += Step_ST; 
       }
       else{
        VEL = Speed_Value;
       }
       Run(VEL);       
       Timer = currentTime;
       
            
  } 
}

int Stop(){
   unsigned long currentTime = millis();
    if (currentTime-Timer2 >= STEPS_STOP_DURATION){
       Timer2 = currentTime;
       Run(Speed_Value/Step_SP);
       Step_SP = Step_SP + 1;     
  } 
     
}



void echoCheck() { // If ping received, set the sensor distance to array.
  if (sonar[currentSensor].check_timer())
    cm[currentSensor] = sonar[currentSensor].ping_result / US_ROUNDTRIP_CM;
}

void oneSensorCycle() { // Sensor ping cycle complete, do something with the results.
  // The following code would be replaced with your code that does something with the ping results.
  for (uint8_t i = 0; i < SONAR_NUM; i++) {
    if ((cm[i] <= MIN_DISTANCE)&&(Sensor_Block[i] == 0)&&(dir == i)){
      STOP = true;
      dir = !i;
      ON = false;
      Step_SP = 1;
      VEL = 0;
      Sensor_Block[i] = 1;
      
     
    }
    else if ((cm[i] > MIN_DISTANCE)&&(Sensor_Block[i] == 1)){
      Sensor_Block[i] = 0;
    }
  }
  
}
