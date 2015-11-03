#include <Wire.h>
#include <elapsedMillis.h>

#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

Adafruit_DCMotor *motorA = AFMS.getMotor(1);
Adafruit_DCMotor *motorB = AFMS.getMotor(2);
int baseSpeed = 200;

#define inPin 3

int COMMAND_FORWARD = 1;
int COMMAND_TURN_LEFT = 2;
int COMMAND_TURN_RIGHT = 3;
int COMMAND_STOP = 4;

elapsedMillis now;

long startPulseDuration = 100; // 100ms
long bitDuration = 50;
int numBits = 4;

bool wasOn = 0;
long pulseStartTime = 0;
long transmissionStartTime = 0;
int currentBit;
long currentBitOnCount;
long currentBitOffCount;
int bits;

int ledR = 9;
int ledG = 10;
int ledB = 11;

int rCycle = 128;
int gCycle = 255;
int bCycle = 128;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("ok");

  pinMode(ledR,   OUTPUT);
  pinMode(ledG, OUTPUT);   
  pinMode(ledB,  OUTPUT); 


  AFMS.begin();  // create with the default frequency 1.6KHz
}

void loop() {
  // put your main code here, to run repeatedly:
  readInput();
  analogWrite(ledR, rCycle);
  analogWrite(ledG, gCycle);
  analogWrite(ledB, bCycle);

  rCycle = (rCycle + 2) % 256;
  gCycle = (gCycle + 3) % 256;
  bCycle = (bCycle + 1) % 256;

  delay(10);
}


void readInput() {
    bool on = analogRead(inPin) > 400;
  if (on && !wasOn) {
    pulseStartTime = now;
  }
  wasOn = on;
  // Serial.println( analogRead(inPin));
  if (on && now - pulseStartTime > startPulseDuration * 0.9 && transmissionStartTime != pulseStartTime) {
    // start a new transmission:
    transmissionStartTime = pulseStartTime;
    bits = 0;
    currentBit = -1;
  }
  if (transmissionStartTime) {
    long t = now - transmissionStartTime;
    int theBit = (t - startPulseDuration) / bitDuration;
    if (theBit >= 0) {
      if (theBit >= numBits) {
        // transmission done:
        transmissionStartTime = 0;
        Serial.print("Received number: ");
        Serial.println(bits);
        receivedNumber(bits);
      } else {
        if (theBit != currentBit) {
          currentBit = theBit;
          currentBitOnCount = 0;
          currentBitOffCount = 0;
        }
        if (on) {
          currentBitOnCount++;
        } else {
          currentBitOffCount++;
        }
        bool bitIsOn = currentBitOnCount > currentBitOffCount;
        // unset this bit:
        bits = bits & ~(1 << theBit);
        if (currentBitOnCount > currentBitOffCount) {
          bits = bits | (1 << theBit);
        }
      }
    }
  }
}

void receivedNumber(int n) {
  if (n == COMMAND_FORWARD) {
    goForward();
  } else if (n == COMMAND_TURN_LEFT) {
    turnLeft();
  } else if (n == COMMAND_TURN_RIGHT) {
    turnRight();
  } else if (n == COMMAND_STOP) {
    stopMoving();
  }
}

void goForward() {
  // Set the speed to start, from 0 (off) to 255 (max speed)
  motorA->setSpeed(baseSpeed);
  motorA->run(FORWARD);
  motorB->setSpeed(baseSpeed);
  motorB->run(FORWARD);
}

void turnLeft() {
  // Set the speed to start, from 0 (off) to 255 (max speed)
  motorA->setSpeed(baseSpeed);
  motorA->run(FORWARD);
  motorB->setSpeed(baseSpeed);
  motorB->run(BACKWARD);
}

void turnRight() {
  // Set the speed to start, from 0 (off) to 255 (max speed)
  motorA->setSpeed(baseSpeed);
  motorA->run(BACKWARD);
  motorB->setSpeed(baseSpeed);
  motorB->run(FORWARD);
}

void stopMoving() {
  motorA->setSpeed(0);
  motorB->setSpeed(0);
  motorA->run(RELEASE);
  motorB->run(RELEASE);
}



