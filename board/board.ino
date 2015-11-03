#include <Wire.h>
#include <elapsedMillis.h>

#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

Adafruit_DCMotor *motorA = AFMS.getMotor(1);
Adafruit_DCMotor *motorB = AFMS.getMotor(2);
int baseSpeed = 140;

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

int RED = 0;
int GREEN = 1;
int BLUE = 2;
int YELLOW = 3;

int numLeds = 4;
int leds[] = {9, 10, 11, 5}; // LED pins
int cycles[] = {0, 0, 0, 0}; // The initial level of the light
int times[] = {1, 1, 1, 1}; 
bool flags[] = {false, false, false, false};
int increments[] = {0, 0, 0, 0};
int modders[] = {64, 64, 64, 64};

int timer = 0;
int timerMax = 500;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(leds[RED],   OUTPUT);
  pinMode(leds[GREEN], OUTPUT);   
  pinMode(leds[BLUE],  OUTPUT); 
  pinMode(leds[YELLOW],  OUTPUT);
  setOcean();
  AFMS.begin();  // create with the default frequency 1.6KHz
}

void loop() {
  // put your main code here, to run repeatedly:
  readInput();
  analogWrite(leds[RED], cycles[RED]);
  analogWrite(leds[GREEN], cycles[GREEN]);
  analogWrite(leds[BLUE], cycles[BLUE]);
  analogWrite(leds[YELLOW], cycles[YELLOW]);

  if (timer >= timerMax) {
    timer = 0;
  } else {
    timer++;
  }

  for (int i = 0; i < numLeds; i++) {
    if (flags[i] || !(timer % times[i])) {
      flags[i] = true;
      cycles[i] = (cycles[i] + increments[i]) % modders[i];
      if (!cycles[i]) {
        flags[i] = false;
      }
    }
  }

  delay(10);
}

void resetLights() {
  for (int i = 0; i < numLeds; i++) {
    cycles[i] = 0;
  }

  for (int i = 0; i < numLeds; i++) {
    times[i] = 1;
  }

  for (int i = 0; i < numLeds; i++) {
    increments[i] = 0;
  }

  for (int i = 0; i < numLeds; i++) {
    modders[i] = 64;
  }
}

void setJungle() {
  resetLights();
  cycles[BLUE] = 5;
  cycles[GREEN] = 255;
  times[YELLOW] = 350; 
  increments[GREEN] = 1;
  increments[YELLOW] = 3;
  modders[GREEN] = 256;
}

void setOcean() {
  resetLights();
  cycles[BLUE] = 15;
  cycles[GREEN] = 15;
  times[RED] = 350;
  times[YELLOW] = 220; 
  increments[YELLOW] = 3;
  increments[RED] = 3;
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



