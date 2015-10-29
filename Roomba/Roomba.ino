/* 
This is a test sketch for the Adafruit assembled Motor Shield for Arduino v2
It won't work with v1.x motor shields! Only for the v2's with built in PWM
control

For use with the Adafruit Motor Shield v2 
---->  http://www.adafruit.com/products/1438
*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

Adafruit_DCMotor *motorA = AFMS.getMotor(1);
Adafruit_DCMotor *motorB = AFMS.getMotor(2);

int trigPin = 4;
int echoPin = 2;
long duration; long distance;

long lastThree[3];

int basespeed = 110;

// RFDUINO Pins
int roboStop = 8;
int roboGo = 9;
int roboLeft = 10;
int roboRight = 11;

// Command State
int command = 0;

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps

  AFMS.begin();  // create with the default frequency 1.6KHz
  
  // Set the speed to start, from 0 (off) to 255 (max speed)
  motorA->setSpeed(144);
  motorA->run(FORWARD);
  motorB->setSpeed(144);
  motorB->run(FORWARD);
  
  // turn on motor
  motorA->run(RELEASE);
  motorB->run(RELEASE);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(roboStop, INPUT);
  pinMode(roboGo, INPUT);
  pinMode(roboLeft, INPUT);
  pinMode(roboRight, INPUT);
}

void changeNums(long newReading) {
  lastThree[0] = lastThree[1];
  lastThree[1] = lastThree[2];
  lastThree[2] = newReading;
}

long average() {
  long sum = lastThree[0] + lastThree[1] + lastThree[2];
  return sum/3;
}

bool checkCommandPins() {
  int commandStop = digitalRead(roboStop);
  int commandGo = digitalRead(roboGo);
  int commandLeft = digitalRead(roboLeft);
  int commandRight = digitalRead(roboRight);
  Serial.println("PIN INPUTS:");
  Serial.println(commandStop);
  Serial.println(commandGo);
  Serial.println(commandLeft);
  Serial.println(commandRight);
  
//  if (commandStop == 1) {
//    Serial.println("STOPPING");
//    motorA->setSpeed(0);
//    motorB->setSpeed(0);
//    motorA->run(FORWARD);
//    motorB->run(FORWARD);
//  } else if (commandGo == 1) {
//    Serial.println("GOING");
//    motorA->setSpeed(basespeed);
//    motorB->setSpeed(basespeed);
//    motorA->run(FORWARD);
//    motorB->run(FORWARD);
//  } else if (commandLeft == 1) {
//    Serial.println("LEFT");
//    motorA->setSpeed(basespeed);
//    motorB->setSpeed(basespeed);
//    motorA->run(FORWARD);
//    motorB->run(BACKWARD);
//  } else if (commandRight) {
//    Serial.println("RIGHT");
//    motorA->setSpeed(basespeed);
//    motorB->setSpeed(basespeed);
//    motorA->run(BACKWARD);
//    motorB->run(FORWARD);
//  } else {
//    Serial.println("NOPE");
//    return false;
//  }

  return true;
}

void loop() {  
  uint8_t i;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2)/29.1;
  int speed; int duration;
  
  changeNums(distance);
  long avg = average();
//  Serial.print("New dist: ");
//  Serial.print(distance);
//  Serial.println(" cm");
//  Serial.print(avg);
//  Serial.println(" cm");

  if (!checkCommandPins()) {
    if (avg == 0) {
//      Serial.println("Out of range");
      speed = random(0,basespeed);
      Serial.println(speed);
      motorA->setSpeed(speed);
      motorB->setSpeed(speed);
      motorA->run(FORWARD);
      motorB->run(FORWARD);
    } else if (avg <= 15) {
//      Serial.println("Too close! Get away!");
      motorA->setSpeed(0);
      motorB->setSpeed(0);
      motorA->run(BACKWARD);
      motorB->run(BACKWARD);
      delay(400);
  
      duration = random(400,1000);
      motorA->setSpeed(basespeed);
      motorB->setSpeed(basespeed);
      motorA->run(FORWARD);
      motorB->run(BACKWARD);
  
      delay(duration);
      motorA->setSpeed(0);
      motorB->setSpeed(0);
      motorA->run(FORWARD);
      motorB->run(FORWARD);
      
    } else if (avg <= 30) {
//      Serial.println("Slow down. Grab the wall.");
      motorA->setSpeed(basespeed/2);
      motorB->setSpeed(basespeed/2);
      motorA->run(FORWARD);
      motorB->run(FORWARD);
    } else {
//      Serial.println("I'm good.");
      motorA->setSpeed(basespeed);
      motorB->setSpeed(basespeed);
      motorA->run(FORWARD);
      motorB->run(FORWARD);
    }
  }
  

  delay(10);  
}
