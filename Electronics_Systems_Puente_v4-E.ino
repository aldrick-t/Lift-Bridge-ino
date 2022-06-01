/*
  Lift Bridge Control System
  V4.E - FULL MANUAL (SIMPLIFIED)
  Built for Arduino MEGA 2560
  APR - MAY 2022
*/

#include <timer.h>
#include <Servo.h>
#include <Chrono.h>
#include <LiquidCrystal.h>

#include <IRremote.h>

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

const int remotePin = 52;

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

  pinMode(boatRED, OUTPUT);
  pinMode(boatYELLOW, OUTPUT);
  pinMode(boatGREEN, OUTPUT);

  pinMode(irSens1, INPUT);
  pinMode(irSens2, INPUT);
  
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
}

void stopLight_hold(){
  digitalWrite(stopLight1, HIGH);
  digitalWrite(stopLight2, HIGH);
}

void stopLight_off(){
  digitalWrite(stopLight1, LOW);
  digitalWrite(stopLight2, LOW);
}

void trafficLight_river(){

}

void trafficReset(){
  if(irsens1_status == 0){
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

void loop(){
  return_loc:
  Serial.println(closedTime.elapsed());
  if(closedTime.hasPassed(4000)){
    closedTime.stop();
    boomGate.write(0);
    Serial.println("gate open");
    stopLight_off();
    digitalWrite(boatYELLOW, LOW);
    digitalWrite(boatGREEN, HIGH);
  }
  //ADD FAILSAFE FOR BOOM GATE & LEAF POSITION.
  liftStatus = 9;

  stopLight_off();
  //Serial.println("PHASE 0 IDLE");
  
  if(leafA_downState == 0){
    stopLight_off();
    bridgeIdle();
    //Serial.println("ROAD TRAFFIC OPEN");
  }

  trafficReset();

  remoteRecieve();
  if(remoteIn == 244){
    carTime.restart();
    lightTime.restart();
    while(lightTime.elapsed() < 5000){
      stopLight_blink();
      trafficReset();
      Serial.println("STOP LIGHT");
      if(lightTime.hasPassed(5000)){
        break;
      }
    }
    while(lightTime.hasPassed(4500)){

      lightTime.stop();

      boomGate.write(90);
      Serial.println("gate closed");

      stopLight_hold();
      Serial.println("LIGHT HOLD");

      trafficReset();
      inputRead();
      while(irsens1_status == 1 && (carTime.hasPassed(10000))){
        inputRead(); //new
        carTime.stop();
        liftStatus = 1;

        Serial.println("BRIDGE SEQ ACTIVE");
        while(leafA_upState == 0 && liftStatus == 1){
          inputRead();
          bridgeOpen();
          Serial.println("opening Bridge");
          if(leafA_upState == 1){
            Serial.println("UP LIMIT");
            bridgeIdle();
            liftStatus = 0;
            break;
          }
        }

        while(leafA_upState == 1 && liftStatus == 0){
          remoteRecieve();
          inputRead();
          bridgeIdle();
          Serial.println("Bridge OPEN!!!");
          if(remoteIn == 245){
            liftStatus = 2;
            break;
          }
        }
        remoteRecieve();
        while(remoteIn == 245 && liftStatus == 2){
          do{
            inputRead();
            bridgeClose();
            Serial.println("closing Bridge");
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
          bridgeIdle();
          closedTime.restart();
          remoteIn = 0;
          digitalWrite(boatYELLOW, HIGH);
          goto return_loc;
        }
      }
    }      
  }
}

