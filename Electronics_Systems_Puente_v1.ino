
//Built for Arduino MEGA 2560
#include <timer.h>
#include <NewPing.h>
#include <Servo.h>
#include <Chrono.h>
//#include <arduino-timer.h>
//#include <Timer.h>
//#include <LightChrono.h>

#define sonar1_trig 36
#define sonar1_echo 37
#define sonar2_trig 38
#define sonar2_echo 39
#define maxDistance 15 //at 15 for testing, set to 50 for tripwire, 80-120 for parallel distance measurement.

const int motorA_speed = 2;
const int motorB_speed = 3;

const int gatePin = 4;

const int motorA_open = 22;
const int motorA_close = 23;
const int motorB_open = 24;
const int motorB_close = 25;

const int stopLight1 = 26;
const int stopLight2 = 27;

const int boatRED = 28;
const int boatYELLOW = 29;
const int boatGREEN = 30; 

const int irSens1 = 32;
const int irSens2 = 33;

const int limSwitchA_down = 40;
const int limSwitchA_up = 41; 
const int limSwitchB_down = 42; 
const int limSwitchB_up = 45; 

int sonar1_dist;
long sonar1_time;

int sonar2_dist;
long sonar2_time;

int irsens1_status;
int irsens2_status;

int leafA_downState;
int leafA_upState;
int leafB_downState;
int leafB_upState;

int riverStatus;
bool boomStatus;
int liftStatus;
bool bridgeClear;

int stopLight1_state;
int stopLight2_state;

unsigned long arduinoTime;
unsigned long blinkTime_prev;
unsigned long blinkTime_current;
const long blinkTime_interval = 300;

Chrono blinkTime;
Chrono carTime;
Chrono riverTime;

Servo boomGate;

NewPing tripwire1(sonar1_trig, sonar1_echo, maxDistance);
NewPing tripwire2(sonar2_trig, sonar2_echo, maxDistance);

void setup(){
  Serial.begin(9600);
  pinMode(motorA_speed, OUTPUT);
  pinMode(motorB_speed, OUTPUT);

  pinMode(motorA_open, OUTPUT);
  pinMode(motorA_close, OUTPUT);
  pinMode(motorB_open, OUTPUT);
  pinMode(motorB_close, OUTPUT);

  pinMode(stopLight1, OUTPUT);
  pinMode(stopLight2, OUTPUT);

  pinMode(boatRED, OUTPUT);
  pinMode(boatYELLOW, OUTPUT);
  pinMode(boatGREEN, OUTPUT);

  pinMode(irSens1, INPUT);
  pinMode(irSens2, INPUT);
  
  pinMode(limSwitchA_down, INPUT);
  pinMode(limSwitchA_up, INPUT);
  pinMode(limSwitchB_down, INPUT);
  pinMode(limSwitchB_up, INPUT);

  boomGate.attach(gatePin);
}

void sonarControl(){
  delay(50);
  sonar1_dist = tripwire1.ping_cm();
  sonar2_dist = tripwire2.ping_cm();
}

void bridgeOpen(){
  analogWrite(motorA_speed, 120);
  digitalWrite(motorA_open, HIGH);
  digitalWrite(motorA_close, LOW);

  analogWrite(motorB_speed, 120);
  digitalWrite(motorB_open, HIGH);
  digitalWrite(motorB_close, LOW);
}

void bridgeIdle(){
  digitalWrite(motorA_open, LOW);
  digitalWrite(motorA_close, LOW);

  digitalWrite(motorB_open, LOW);
  digitalWrite(motorB_close, LOW);
}

void bridgeClose(){
  analogWrite(motorA_speed, 80);
  digitalWrite(motorA_close, HIGH);
  digitalWrite(motorA_open, LOW);

  analogWrite(motorB_speed, 80);
  digitalWrite(motorB_close, HIGH);
  digitalWrite(motorB_open, LOW);  
}

void liftControl(){
  if(liftStatus == 1 && leafA_upState != 1){
    bridgeOpen();
    Serial.println("opening Bridge - ACTIVE");
  }
  else if(leafA_upState == 1){
    bridgeIdle();
    liftStatus = 0;
  }
  
  if(liftStatus = 2 && leafA_downState != 1){
    bridgeClose();
    Serial.println("closing Bridge - ACTIVE");    
  }
  else if(leafA_downState == 1){
    bridgeIdle();
    liftStatus = 0;
  }
}

void stopLight_blink(){ //Replace current millis() comparator system with Chrono lib. functions.
  if(blinkTime_current - blinkTime_prev >= blinkTime_interval){
    blinkTime_prev = blinkTime_current;

    if(stopLight1_state == LOW && stopLight2_state == HIGH){
      stopLight1_state = HIGH;
      stopLight2_state = LOW;
    }
    else{
      stopLight1_state = LOW;
      stopLight2_state = HIGH;
    }

    digitalWrite(stopLight1, stopLight1_state);
    digitalWrite(stopLight2, stopLight2_state);
  }
}

void trafficLight_river(){ //Create function for river lights.

}

void loop(){
  sonarControl();

  arduinoTime = millis();
  blinkTime_current = millis();
  leafA_downState = digitalRead(limSwitchA_down);
  leafB_downState = digitalRead(limSwitchB_down);
  leafA_upState = digitalRead(limSwitchA_up);
  leafB_upState = digitalRead(limSwitchB_up);
  irsens1_status = digitalRead(irSens1);
  irsens2_status = digitalRead(irSens2);

  //include toggles to current status change tree to avoid status switching when limSwitch changes state.
  if(sonar1_dist > 0){
    riverStatus = 1;
  }
  else if(sonar1_dist == 0){
    riverStatus = 0;
  }
  
  if(sonar1_dist > 0 && leafA_upState == 0){
    liftStatus = 1;
  }
  else if(sonar1_dist == 0 && leafA_upState == 1){
    liftStatus = 2;
  }
  else if(leafA_downState == 1 || leafB_downState == 1){
    liftStatus = 0;
  }


  if(riverStatus == 1 || leafA_downState == 0){
    boomGate.write(90); //Use true/false toggle with inverse (!).
    boomStatus = true;
    Serial.println("gate closed");
    stopLight_blink();
  }
  else if(leafA_downState == 1){
    //delay(1000); //change to timer system if possible.
    boomGate.write(0); //Use true/false toggle with inverse (!).
    boomStatus = false;
    Serial.println("gate open");
    digitalWrite(stopLight1, LOW);
    digitalWrite(stopLight2, LOW);
    //riverStatus == 0; //eliminates fail detection!! - warning.
  }

  if(boomStatus == true){
    if(irsens1_status == 1){
      carTime.restart();
      if(carTime.hasPassed(4000)){
        bridgeClear = true;
      }
    }
    else if(irsens1_status == 0){
      carTime.restart();
    }

    while(bridgeClear == true){//changed from if to while for continuous cycle.
      sonarControl();
      if(liftStatus == 1 && leafA_upState == 0){
          bridgeOpen();
          Serial.println("opening Bridge");
      }
      else if(liftStatus == 0 || leafA_upState == 1){
        bridgeIdle();
        liftStatus = 0;

        if(liftStatus = 2 && leafA_downState == 0){
          bridgeClose();
          Serial.println("closing Bridge");    
        }
        else if(liftStatus == 0 || leafA_downState == 1){
          bridgeIdle();
          liftStatus = 0;
        }
      }
        
    }
    if(leafA_downState == 1 && leafB_downState == 1){//Changed condition to AND from original OR.
      carTime.restart();
      bridgeIdle();
    }

  }

  Serial.println(sonar1_dist);
  //Serial.println(leafA_downState);
  //Serial.println(riverStatus);
  //Serial.println(bridgeClear_time);
  Serial.println(liftStatus);
  //Serial.println(irsens1_status);
}

