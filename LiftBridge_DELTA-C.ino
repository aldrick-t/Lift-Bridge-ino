/*
  Lift Bridge Control System
  DELTA-C (GAMMA-2)
  Built for Arduino MEGA 2560
  JUN 2022

  Created by:
  Aldrick Tadeo
  12JUNE2022

  Free use code.
  
  Repository: https://github.com/aldrick-t/Lift-Bridge.git
*/

#include <timer.h>
#include <Servo.h>
#include <Chrono.h>
#include <IRremote.h>

#define closeSpeed 100
#define openSpeed 200

const int motorA_speed = 2;
const int motorB_speed = 3;

const int gatePin = 4;

const int remotePin = 7;

const int motorA_open = 22;
const int motorA_close = 23;
const int motorB_open = 24;
const int motorB_close = 25;

const int stopLight1 = 26;
const int stopLight2 = 27;
const int roadYELLOW = 28;
const int roadGREEN = 29;

const int boatRED = 30;
const int boatYELLOW = 31;
const int boatGREEN = 32; 

const int irSens1 = 34;
const int irSens2 = 35;
const int irSens_river1 = 36;
const int irSens_river2 = 37;

const int limSwitchA_down = 42;
const int limSwitchA_up = 39; 
const int limSwitchB_down = 44; 
const int limSwitchB_up = 41; 


int irsens1_status;
int irsens2_status;

int leafA_downState;
int leafA_upState;
int leafB_downState;
int leafB_upState;

int liftStatus;

int stopLight1_state = HIGH;
int stopLight2_state = LOW;

int remoteIn;

Chrono blinkTime;
Chrono carTime;
Chrono closedTime;
Chrono lightTime;

Servo boomGate;

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
  pinMode(roadYELLOW, OUTPUT);
  pinMode(roadGREEN, OUTPUT);

  pinMode(boatRED, OUTPUT);
  pinMode(boatYELLOW, OUTPUT);
  pinMode(boatGREEN, OUTPUT);

  pinMode(irSens1, INPUT);
  pinMode(irSens2, INPUT);
  pinMode(irSens_river1, INPUT);
  pinMode(irSens_river2, INPUT);
  
  pinMode(limSwitchA_down, INPUT);
  pinMode(limSwitchA_up, INPUT);
  pinMode(limSwitchB_down, INPUT);
  pinMode(limSwitchB_up, INPUT);

  pinMode(remotePin, INPUT);

  boomGate.attach(gatePin);

  IrReceiver.begin(remotePin, ENABLE_LED_FEEDBACK);

  boomGate.write(0);
}

void bridgeOpen(){
  analogWrite(motorA_speed, openSpeed);
  digitalWrite(motorA_open, HIGH);
  digitalWrite(motorA_close, LOW);

  analogWrite(motorB_speed, openSpeed);
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
  analogWrite(motorA_speed, closeSpeed);
  digitalWrite(motorA_close, HIGH);
  digitalWrite(motorA_open, LOW);

  analogWrite(motorB_speed, closeSpeed);
  digitalWrite(motorB_close, HIGH);
  digitalWrite(motorB_open, LOW);  
}

void stopLight_blink(){
  digitalWrite(roadYELLOW, LOW);
  digitalWrite(roadGREEN, LOW);

  if(blinkTime.hasPassed(300)){
    blinkTime.restart();
    stopLight1_state = !stopLight1_state;
    digitalWrite(stopLight1, stopLight1_state);
    stopLight2_state = !stopLight2_state;
    digitalWrite(stopLight2, stopLight2_state);
  } 
}

void stopLight_hold(){
  digitalWrite(stopLight1, HIGH);
  digitalWrite(stopLight2, HIGH);

  digitalWrite(roadYELLOW, LOW);
  digitalWrite(roadGREEN, LOW);
}

void stopLight_off(){
  digitalWrite(stopLight1, LOW);
  digitalWrite(stopLight2, LOW);
}

void roadTraffic_yellow(){
  digitalWrite(stopLight1, LOW);
  digitalWrite(stopLight2, LOW);
  digitalWrite(roadYELLOW, HIGH);
  digitalWrite(roadGREEN, LOW);
}

void roadTraffic_green(){
  digitalWrite(stopLight1, LOW);
  digitalWrite(stopLight2, LOW);
  digitalWrite(roadYELLOW, LOW);
  digitalWrite(roadGREEN, HIGH);
}

void trafficReset(){
  if(irsens1_status == 1 || irsens2_status == 1){
    carTime.restart();
  }
}

void inputRead(){
  leafA_downState = digitalRead(limSwitchA_down);
  leafB_downState = digitalRead(limSwitchB_down);

  leafA_upState = digitalRead(limSwitchA_up);
  leafB_upState = digitalRead(limSwitchB_up);

  irsens1_status = digitalRead(irSens1);
  irsens2_status = digitalRead(irSens2);
}

void remoteRecieve(){
  remoteIn = (IrReceiver.decodedIRData.decodedRawData);
  if (IrReceiver.decode()) {
      Serial.println(remoteIn);
      IrReceiver.resume();
  }
}

void riverTraffic_red(){
  digitalWrite(boatRED, HIGH);
  digitalWrite(boatYELLOW, LOW);
  digitalWrite(boatGREEN, LOW);
}

void riverTraffic_yellow(){
  digitalWrite(boatRED, LOW);
  digitalWrite(boatYELLOW, HIGH);
  digitalWrite(boatGREEN, LOW);
}

void riverTraffic_green(){
  digitalWrite(boatRED, LOW);
  digitalWrite(boatYELLOW, LOW);
  digitalWrite(boatGREEN, HIGH);
}

void loop(){
  return_loc:
  inputRead();
  if(leafA_downState == 1 && closedTime.hasPassed(3000)){
    closedTime.stop();
    boomGate.write(0);
    Serial.println("TRAFFIC GATE OPEN");
    stopLight_off();
    roadTraffic_green();

  }
  else if(leafA_downState == 0){
    closedTime.restart();
    boomGate.write(90);
    Serial.println("BRIDGE LEAF AJAR");
    stopLight_hold();
  }

  liftStatus = 9; 

  trafficReset();

  remoteRecieve();
  if(remoteIn == 244){
    carTime.restart();
    lightTime.restart();
    riverTraffic_yellow();
    while(lightTime.elapsed() < 5000){
      stopLight_blink();
      trafficReset();
      Serial.println("STOP LIGHT FLASHING");
      if(lightTime.hasPassed(5000)){
        break;
      }
    }
    while(lightTime.hasPassed(4500)){

      lightTime.stop();

      boomGate.write(90);
      Serial.println("GATE CLOSED");

      stopLight_hold();
      Serial.println("STOP LIGHT HOLD");

      trafficReset();
      inputRead();
      while((irsens1_status == 0 && irsens2_status == 0) && (carTime.hasPassed(10000))){
        inputRead();
        carTime.stop();
        liftStatus = 1;

        Serial.println("BRIDGE ACTIVE");
        while(leafA_upState == 0 && liftStatus == 1){
          inputRead();
          bridgeOpen();
          Serial.println("Opening Bridge...");
          if(leafA_upState == 1){
            Serial.println("UPPER LIMIT REACHED");
            bridgeIdle();
            liftStatus = 0;
            break;
          }
        }

        while(leafA_upState == 1 && liftStatus == 0){
          remoteRecieve();
          inputRead();
          bridgeIdle();
          riverTraffic_green();
          Serial.println("BRIDGE FULLY OPEN");
          if(remoteIn == 245){
            liftStatus = 2;
            break;
          }
        }
        remoteRecieve();
        while(remoteIn == 245 && liftStatus == 2){
          riverTraffic_red();
          do{
            inputRead();
            bridgeClose();
            Serial.println("Closing Bridge...");
            if(leafA_downState == 1){
              bridgeIdle();
              liftStatus = 3;
              break;
            }    
          } while(leafA_downState == 0 && liftStatus == 2);

          if(leafA_downState == 1 && liftStatus == 3){
            bridgeIdle();
            break;
          }
        }
        if(leafA_downState == 1 && liftStatus == 3){
          Serial.println("BRIDGE CLOSED");
          bridgeIdle();
          closedTime.restart();
          remoteIn = 0;
          riverTraffic_red();
          roadTraffic_yellow();
          goto return_loc;
        }
      }
    }      
  }
}

