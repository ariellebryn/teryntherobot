#include <elapsedMillis.h>
#include <RFduinoBLE.h>
#define outPin 6

elapsedMillis now;

long startPulseDuration = 100; // 100ms
long bitDuration = 50;
int numBits = 4;

int sendingNum = -1;
long sendingStartTime;

void setup() {
  Serial.begin(9600);
  pinMode(outPin, OUTPUT);
  RFduinoBLE.advertisementData = "CouDow"; // shouldnt be more than 10 characters long
  RFduinoBLE.deviceName = "ourduino";  //  name of your RFduino. Will appear when other BLE enabled devices search for it
  RFduinoBLE.begin(); // begin
  // test:
  sendingNum = 6;
  sendingStartTime = now;
}

void loop() {
  bool on = 0;
  if (sendingNum != -1) {
    long t = now - sendingStartTime;
    if (t < startPulseDuration) {
      on = 1;
    } else if (t < startPulseDuration + bitDuration * numBits) {
      int theBit = (t - startPulseDuration) / bitDuration;
      on = (sendingNum & (1 << theBit)) ? 1 : 0;
    } else {
      sendingNum = -1;
    }
  }
  digitalWrite(outPin, on);
}

void RFduinoBLE_onReceive(char *data, int len)
{
  // display the first recieved byte
  sendingNum = data[0];
  sendingStartTime = now;
}

