#include <Servo.h>

Servo servoFlapL;
Servo servoFlapR;
Servo servoAileronL;
Servo servoAileronR;
Servo servoElevator;
Servo servoRudder;
Servo servoTiltL;
Servo servoTiltR;

int flapState = 0;  // 0, 1, 2
int rollState = 0;  // -2, -1, 0, 1, 2
int pitchState = 0; // -2, -1, 0, 1, 2
int yawState = 0;   // -2, -1, 0, 1, 2
bool horizontalMode = false;
bool targetHorizontalMode = false;

unsigned int currentLFlapPWM = 1500;
unsigned int currentRFlapPWM = 1500;
unsigned int targetLFlapPWM = 1500;
unsigned int targetRFlapPWM = 1500;

unsigned int currentLTiltPWM = 1500;
unsigned int currentRTiltPWM = 1500;
unsigned int targetLTiltPWM = 1500;
unsigned int targetRTiltPWM = 1500;

const unsigned int aileronRPWMs[] = {1500, 1500, 1500, 1500, 1500}; // tbd
const unsigned int aileronLPWMs[] = {1700, 1550, 1400, 1275, 1150};
const unsigned int flapRPWMs[] = {950, 1175, 1400};
const unsigned int flapLPWMs[] = {1500, 1500, 1500}; // tbd
const unsigned int elevatorPWMs[] = {1500, 1500, 1500, 1500, 1500}; // tbd
const unsigned int rudderPWMs[] = {1500, 1500, 1500, 1500, 1500}; // tbd
const unsigned int tiltRPWMs[] = {1500, 1500}; // [horizontal, vertical] tbd
const unsigned int tiltLPWMs[] = {1500, 1500}; // tbd
const unsigned int verticalFlightAuthority = 20;

void setup() {
  servoFlapL.attach(0); // pins tbd
  servoFlapR.attach(0);
  servoAileronL.attach(0);
  servoAileronR.attach(0);
  servoElevator.attach(0);
  servoRudder.attach(0);
  servoTiltL.attach(0);
  servoTiltR.attach(0);

  Serial.begin(115200);
}

void loop() {
  if (Serial.available()) {
    if (Serial.readStringUntil('\n') == "MAD") {
      targetHorizontalMode = false;
      if (Serial.read() == 'h') {targetHorizontalMode = true;}
      flapState = Serial.read();
      pitchState = Serial.read();
      rollState = Serial.read();
      yawState = Serial.read();
    }
  }

  if (horizontalMode == targetHorizontalMode) {
    if (horizontalMode) {
      servoAileronL.writeMicroseconds(aileronLPWMs[rollState+2]);
      servoAileronR.writeMicroseconds(aileronRPWMs[rollState+2]);
      servoElevator.writeMicroseconds(elevatorPWMs[pitchState+2]);
      servoRudder.writeMicroseconds(rudderPWMs[yawState+2]);
      targetRTiltPWM = tiltRPWMs[0];
      targetLTiltPWM = tiltLPWMs[0];
    }
    else {
      servoAileronL.writeMicroseconds(aileronLPWMs[2]);
      servoAileronR.writeMicroseconds(aileronRPWMs[2]);
      servoElevator.writeMicroseconds(elevatorPWMs[2]);
      servoRudder.writeMicroseconds(rudderPWMs[2]);
      targetRTiltPWM = tiltRPWMs[1] + verticalFlightAuthority * (pitchState + yawState);
      targetLTiltPWM = tiltLPWMs[1] + verticalFlightAuthority * (pitchState - yawState);
    }

    targetRFlapPWM = flapRPWMs[flapState];
    targetLFlapPWM = flapLPWMs[flapState];
  }


   // in middle of transition
  else if (targetHorizontalMode) {
    targetRFlapPWM = flapRPWMs[2];
    targetLFlapPWM = flapLPWMs[2];
    currentRFlapPWM = flapRPWMs[2];
    currentLFlapPWM = flapLPWMs[2];
    targetRTiltPWM = tiltRPWMs[0];
    targetLTiltPWM = tiltLPWMs[0];
    if (currentRTiltPWM == tiltRPWMs[0] and currentLTiltPWM == tiltLPWMs[0]) {
      horizontalMode = true;
    }
  }
  else {
    targetRFlapPWM = flapRPWMs[2];
    targetLFlapPWM = flapLPWMs[2];
    targetRTiltPWM = tiltRPWMs[0];
    targetLTiltPWM = tiltLPWMs[0];

    if (currentRFlapPWM == flapRPWMs[2] and currentLFlapPWM == flapLPWMs[2]) { // wait for flaps to extend
      targetRTiltPWM = tiltRPWMs[1];
      targetLTiltPWM = tiltLPWMs[1];
      if (currentRTiltPWM == tiltRPWMs[1] and currentLTiltPWM == tiltLPWMs[1]) {
        horizontalMode = false;
      }
    }
  }


  // move flaps and tilt servos slowly

  if (targetRTiltPWM > currentRTiltPWM) {currentRTiltPWM += 1;}
  if (targetRTiltPWM < currentRTiltPWM) {currentRTiltPWM -= 1;}
  if (targetLTiltPWM > currentLTiltPWM) {currentLTiltPWM += 1;}
  if (targetLTiltPWM < currentLTiltPWM) {currentLTiltPWM -= 1;}
  servoTiltR.writeMicroseconds(currentRTiltPWM);
  servoTiltL.writeMicroseconds(currentLTiltPWM);
  
  if (targetRFlapPWM > currentRFlapPWM) {currentRFlapPWM += 1;}
  if (targetRFlapPWM < currentRFlapPWM) {currentRFlapPWM -= 1;}
  if (targetLFlapPWM > currentLFlapPWM) {currentLFlapPWM += 1;}
  if (targetLFlapPWM < currentLFlapPWM) {currentLFlapPWM -= 1;}
  servoFlapR.writeMicroseconds(currentRFlapPWM);
  servoFlapL.writeMicroseconds(currentLFlapPWM);
  
  delay(10);
}
