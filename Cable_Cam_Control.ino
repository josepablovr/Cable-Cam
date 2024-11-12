// INSTITUTO TECNOLOGICO DE COSTA RICA
// MADE BY: JOSÉ PABLO VÁSQUEZ ROJAS
// CONTACT: josepablovasro@gmail.com

#include <NewPing.h>

// Define Ultrasonic Sensor Pins and Distances
#define TRIGGER_PIN1  11
#define ECHO_PIN1     8
#define MAX_DISTANCE  200  // Maximum distance for object detection (in cm)
#define MIN_DISTANCE  30   // Minimum safe distance to trigger stop (in cm)
#define TRIGGER_PIN2  10
#define ECHO_PIN2     9
#define SONAR_NUM     2    // Number of sensors.
#define PING_INTERVAL 33   // Interval between sensor pings (29ms is about the minimum to avoid cross-sensor echo).

unsigned long pingTimer[2]; // Array to track ping times for each sensor.
unsigned int cm[2];         // Array to store distance readings from sensors.
uint8_t currentSensor = 0;  // Index to keep track of active sensor.
unsigned int Sensor_Block[2]; // Flags to block further movement based on proximity.

NewPing sonar[2] = {NewPing(TRIGGER_PIN1, ECHO_PIN1, MAX_DISTANCE), NewPing(TRIGGER_PIN2, ECHO_PIN2, MAX_DISTANCE)};

// Motor Control Setup
#define MAXSTEPS_START 50
#define START_DURATION 5000 // Starting duration in milliseconds
int STEPS_START_DURATION = START_DURATION / MAXSTEPS_START; 
int VEL = 0;               // Current motor speed
unsigned long Timer = 0;   // Timer for progressive start
int Step_ST = 1;           // Step increment for start speed

#define MAXSTEPS_STOP 6   
#define STOP_DURATION 600  // Stopping duration in milliseconds
int STEPS_STOP_DURATION = STOP_DURATION / MAXSTEPS_STOP;
unsigned long Timer2 = 0;  // Timer for progressive stop
int Step_SP = 1;           // Step decrement for stop speed

// Operational Control Variables
int dir = 1;               // Direction of movement
bool ON = false;           // Control flag to turn on movement
bool STOP = false;         // Control flag to initiate stop
int Speed_Value = 0;       // Target speed

// H-Bridge Motor Control Pins
const int R1 = 6; 
const int L1 = 5; 
const int R2 = 3;
const int L2 = 4;

void setup() {
  // Set PWM frequency for motor speed control (using the default 976.56 Hz)
  TCCR0B = TCCR0B & B11111000 | B00000011;

  // Set motor control pins as output
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);

  Serial.begin(9600); // Set serial communication at 9600 baud rate
  
  // Initialize sensor ping timers
  pingTimer[0] = millis() + 75;
  pingTimer[1] = pingTimer[0] + PING_INTERVAL;
  
  // Initialize sensor blocking flags
  Sensor_Block[0] = 0;
  Sensor_Block[1] = 0;
}

void loop() {
  // Serial data handling for control commands
  char buf[12];
  char Valor[3];
  int byteReceived;

  if (Serial.available() > 0) {
    byteReceived = Serial.readBytesUntil('\n', buf, 10);   // Read incoming command
    buf[byteReceived] = '\0';   // Null-terminate buffer

    if (buf[0] == 'A') {  // Start movement command
      Serial.print("ON");
      ON = true;
      STOP = false;
      VEL = 0;
    }
    
    if (buf[0] == 'B') {  // Stop movement command
      VEL = 0;
      ON = false;
      STOP = true;
    }

    if (buf[0] == 'S') {  // Set speed command
      Valor[0] = buf[2];
      Valor[1] = buf[3];
      Valor[2] = buf[4];
      Speed_Value = atoi(Valor);
      Step_ST = Speed_Value / MAXSTEPS_START;
    }

    if (buf[0] == 'C') {  // Change direction command
      dir = !dir;
    }
  }

  // Check Ultrasonic Sensors
  if (ON == true) {
    for (uint8_t i = 0; i < SONAR_NUM; i++) {
      if (millis() >= pingTimer[i]) {
        pingTimer[i] += PING_INTERVAL * SONAR_NUM;  
        if (i == 0 && currentSensor == SONAR_NUM - 1) oneSensorCycle();
        sonar[currentSensor].timer_stop();          
        currentSensor = i;                          
        cm[currentSensor] = 200;                    
        sonar[currentSensor].ping_timer(echoCheck); 
      }
    }
  }

  // Progressive Start Logic
  if ((ON == true) && (VEL < Speed_Value)) {
    Start(); 
  }

  // Stop Logic
  if (STOP == true) {
    if (VEL == 0) {
      Aboslute_Stop();
      STOP = false;
      Step_SP = 1;
      VEL = 0;
    } else {
      Stop(); // Progressive stop
    }
  }
}

// Motor Control Functions
int TurnRight(int speed) {
  digitalWrite(R2, HIGH);
  analogWrite(R1, speed);
  digitalWrite(L1, LOW);
  digitalWrite(L2, LOW);
}

int TurnLeft(int speed) {
  digitalWrite(L2, HIGH);
  analogWrite(L1, speed);
  digitalWrite(R1, LOW);
  digitalWrite(R2, LOW);
}

int Run(int Speed) { 
  if(dir == 1) TurnRight(Speed); 
  else TurnLeft(Speed); 
}

int Aboslute_Stop() {
  digitalWrite(L2, LOW);
  digitalWrite(L1, LOW);
  digitalWrite(R1, LOW);
  digitalWrite(R2, LOW);
}

// Progressive Start Routine
int Start() {
   unsigned long currentTime = millis();
   if (currentTime - Timer > STEPS_START_DURATION) {
     if (VEL < Speed_Value) VEL += Step_ST; 
     else VEL = Speed_Value;
     Run(VEL);       
     Timer = currentTime;
   } 
}

// Progressive Stop Routine
int Stop() {
   unsigned long currentTime = millis();
   if (currentTime - Timer2 >= STEPS_STOP_DURATION) {
       Timer2 = currentTime;
       Run(Speed_Value / Step_SP);
       Step_SP++;
   } 
}

// Interrupt for ultrasonic echo return handling
void echoCheck() {
  if (sonar[currentSensor].check_timer())
    cm[currentSensor] = sonar[currentSensor].ping_result / US_ROUNDTRIP_CM;
}

// Routine to handle actions based on sensor distance readings
void oneSensorCycle() {
  for (uint8_t i = 0; i < SONAR_NUM; i++) {
    if ((cm[i] <= MIN_DISTANCE) && (Sensor_Block[i] == 0) && (dir == i)) {
      STOP = true;
      dir = !i;
      ON = false;
      Step_SP = 1;
      VEL = 0;
      Sensor_Block[i] = 1;
    } else if ((cm[i] > MIN_DISTANCE) && (Sensor_Block[i] == 1)) {
      Sensor_Block[i] = 0;
    }
  }
}
