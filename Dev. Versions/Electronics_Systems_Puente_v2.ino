/*
  Lift Bridge Control System
  V2 - FULL AUTO BETA
  Built for Arduino MEGA 2560
  APR - MAY 2022
*/

#include <timer.h>
#include <NewPing.h>
#include <Servo.h>
#include <Chrono.h>
#include <LiquidCrystal.h>

#include <TaskScheduler.h>
#include <TaskSchedulerDeclarations.h>
#include <TaskSchedulerSleepMethods.h>

//#include <arduino-timer.h>
//#include <Timer.h>
//#include <LightChrono.h>
//#include <Stepper.h>

#define sonar1_trig 36
#define sonar1_echo 37
#define sonar2_trig 38
#define sonar2_echo 39
#define maxDistance 7 //at 15 for testing, set to 50 for tripwire, 80-120 for parallel distance measurement.

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
bool riverTraffic;
bool boomStatus;
int liftStatus;
bool bridgeClear;

int stopLight1_state = HIGH;
int stopLight2_state = LOW;

unsigned long arduinoTime;
unsigned long blinkTime_prev;
unsigned long blinkTime_current;
const long blinkTime_interval = 300;

Chrono blinkTime;
Chrono carTime;
Chrono riverTime;
Chrono closedTime;
Chrono lightTime;

Servo boomGate;

NewPing tripwire1(sonar1_trig, sonar1_echo, maxDistance);
NewPing tripwire2(sonar2_trig, sonar2_echo, maxDistance);

void sonicPing_callback();
void switchesIn_callback();
//void stopLights_callback();

Task sonicPing(50, TASK_FOREVER, &sonicPing_callback);
Task switchesIn(TASK_IMMEDIATE, TASK_FOREVER, &switchesIn_callback);
//Task stopLights(300, 10, &stopLights_callback);

Scheduler inputs;

//Include continuous function exec. with TaskScheduler lib., for sensors and inputs.
//Utilize bool/return (view docs) for status conditions.
//

void sonicPing_callback(){
  sonar1_dist = tripwire1.ping_cm();
  sonar2_dist = tripwire2.ping_cm();
  Serial.println("SEARCHING!");
}

void switchesIn_callback(){
  leafA_upState = digitalRead(limSwitchA_up);
  leafA_downState = digitalRead(limSwitchA_down);

  leafB_upState = digitalRead(limSwitchB_up);
  leafB_downState = digitalRead(limSwitchB_down);

  irsens1_status = digitalRead(irSens1);
  irsens2_status = digitalRead(irSens2);
}

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

  inputs.init();
  inputs.addTask(sonicPing);
  inputs.addTask(switchesIn);

  delay(4000);

  sonicPing.enable();
  switchesIn.enable();
}

void sonarPing(){
  delay(50);
  sonar1_dist = tripwire1.ping_cm();
  sonar2_dist = tripwire2.ping_cm();
}

void bridgeOpen(){
  analogWrite(motorA_speed, 100);
  digitalWrite(motorA_open, HIGH);
  digitalWrite(motorA_close, LOW);

  analogWrite(motorB_speed, 100);
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
  analogWrite(motorA_speed, 70);
  digitalWrite(motorA_close, HIGH);
  digitalWrite(motorA_open, LOW);

  analogWrite(motorB_speed, 70);
  digitalWrite(motorB_close, HIGH);
  digitalWrite(motorB_open, LOW);  
}

void stopLight_blink(){
  if(blinkTime.hasPassed(300)){
    blinkTime.restart();
    stopLight1_state = !stopLight1_state;
    digitalWrite(stopLight1, stopLight1_state);
    stopLight2_state = !stopLight2_state;
    digitalWrite(stopLight2, stopLight2_state);

  }


/*
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
*/  
}

void stopLight_hold(){
  digitalWrite(stopLight1, HIGH);
  digitalWrite(stopLight2, HIGH);

}

void trafficLight_river(){ //Create function for river lights.

}

void inputRead(){
  leafA_downState = digitalRead(limSwitchA_down);
  leafB_downState = digitalRead(limSwitchB_down);

  leafA_upState = digitalRead(limSwitchA_up);
  leafB_upState = digitalRead(limSwitchB_up);

  irsens1_status = digitalRead(irSens1);
  irsens2_status = digitalRead(irSens2);
}

//Change to single linear logic tree to avoid retro-switching.
void loop(){
  inputs.execute(); //Starts scheduler!
  //sonarPing();
  carTime.restart();
  
  arduinoTime = millis();
  blinkTime_current = millis();

  if(sonar1_dist > 0){
    riverStatus = 1;
  }

  //include toggles to current status change tree to avoid status switching when limSwitch changes state.
  if(sonar1_dist > 0){//change condition to status from sensor reading tree (above).
    //include condition toggle.
    lightTime.restart();
    while(lightTime.elapsed() < 3000){
      stopLight_blink();
      //Serial.println("STOP LIGHT");//Possible error while stuck in loop.
    }
    lightTime.stop();
    boomGate.write(90);
    //Serial.println("gate closed");
    stopLight_hold();
    //carTime.restart();
    if(irsens1_status == 1 && carTime.hasPassed(7000)){//Possible error when reading carTime.
      //add while loop to avoid block skipping.
      //if(carTime.hasPassed(4000)){
        carTime.stop();
        do{
          bridgeOpen();
          //Serial.println("opening Bridge");
          if(leafA_upState == 1){
            bridgeIdle();
            break;
          }
          //delay(50);//delay to wait for switch detach.
        } while(leafA_upState == 0);

        while(leafA_upState == 1){
          bridgeIdle();
          if(sonar1_dist == false){
            break;
          }

        if(sonar1_dist == false){
          do{
            bridgeClose();
            //Serial.println("closing Bridge");
            if(leafA_downState == 1){
              bridgeIdle();
              break;
            }    
            //delay(50);//delay to wait for switch detach.
          } while(leafA_downState == 0);

          if(leafA_downState == 1){
            bridgeIdle();
            boomGate.write(0); //Use true/false toggle with inverse (!).
            Serial.println("gate open");
            digitalWrite(stopLight1, LOW);
            digitalWrite(stopLight2, LOW);
          }
        }  
        }
      //}
    }      
    else if(irsens1_status == 0){
      carTime.restart();
      stopLight_blink();
    }
  }
//Logic Tree changed up to here.

  Serial.println(sonar1_dist);
  //Serial.println(leafA_downState);
  //Serial.println(riverStatus);
  //Serial.println(bridgeClear_time);
  //Serial.println(liftStatus);
  //Serial.println(irsens1_status);
  //Serial.println(carTime.elapsed());
  //Serial.println(lightTime.elapsed());

}//LOOP END

