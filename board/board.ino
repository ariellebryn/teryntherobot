#include <elapsedMillis.h>

#define inPin 3

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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("ok");
  //pinMode(A4, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
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

