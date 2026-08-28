#include <Servo.h>

Servo servoFlapL;
Servo servoFlapR;
Servo servoAileronL;
Servo servoAileronR;
Servo servoElevator;
Servo servoRudder;
Servo servoTiltL;
Servo servoTiltR;

byte flapState = 0;  // 0, 1, 2
int rollState = 2;  // 0, 1, 2, 3, 4
int pitchState = 2; // 0, 1, 2, 3, 4
int yawState = 2;   // 0, 1, 2, 3, 4
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

//const unsigned int aileronRPWMs[] = {1150, 1300, 1450, 1600, 1750};
const unsigned int aileronRPWMs[] = {1750, 1600, 1450, 1300, 1150};
const unsigned int aileronLPWMs[] = {1700, 1550, 1400, 1275, 1150};
const unsigned int flapRPWMs[] = {950, 1175, 1400};
const unsigned int flapLPWMs[] = {1950, 1750, 1550};
const unsigned int elevatorPWMs[] = {1500, 1500, 1500, 1500, 1500}; // tbd
const unsigned int rudderPWMs[] = {1500, 1500, 1500, 1500, 1500}; // tbd
const unsigned int tiltRPWMs[] = {1950, 1250};
const unsigned int tiltLPWMs[] = {1000, 1700};
const unsigned int verticalFlightAuthority = 60;

void setup() {
  delay(5000); // leave time for reflashing in case of power surge boot loop when powering servos
  servoFlapL.attach(7);
  servoFlapR.attach(2);
  servoAileronL.attach(5);
  servoAileronR.attach(4);
  servoElevator.attach(9);
  servoRudder.attach(10);
  servoTiltL.attach(6);
  servoTiltR.attach(3);

  Serial.begin(115200);
  Serial.setTimeout(100);
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

      if (flapState > 2) {flapState = 0;}
      if (rollState > 4) {rollState = 2;}
      if (pitchState > 4) {pitchState = 2;}
      if (yawState > 4) {yawState = 2;}
      //Serial.println(rollState);
    }
  }

  if (horizontalMode == targetHorizontalMode) {
    if (horizontalMode) {
      servoAileronL.writeMicroseconds(aileronLPWMs[rollState]);
      servoAileronR.writeMicroseconds(aileronRPWMs[rollState]);
      servoElevator.writeMicroseconds(elevatorPWMs[pitchState]);
      servoRudder.writeMicroseconds(rudderPWMs[yawState]);
      targetRTiltPWM = tiltRPWMs[0];
      targetLTiltPWM = tiltLPWMs[0];
    }
    else {
      servoAileronL.writeMicroseconds(aileronLPWMs[2]);
      servoAileronR.writeMicroseconds(aileronRPWMs[2]);
      servoElevator.writeMicroseconds(elevatorPWMs[2]);
      servoRudder.writeMicroseconds(rudderPWMs[2]);
      targetRTiltPWM = tiltRPWMs[1] + verticalFlightAuthority * (-(pitchState-2) - (yawState-2));
      targetLTiltPWM = tiltLPWMs[1] + verticalFlightAuthority * ((pitchState-2) - (yawState-2));
    }

    targetRFlapPWM = flapRPWMs[flapState];
    targetLFlapPWM = flapLPWMs[flapState];
  }


   // in middle of transition
  else if (targetHorizontalMode) {
    targetRFlapPWM = flapRPWMs[2];
    targetLFlapPWM = flapLPWMs[2];
    if (currentRFlapPWM == flapRPWMs[2] and currentLFlapPWM == flapLPWMs[2]) { // wait for flaps to extend
      targetRTiltPWM = tiltRPWMs[0];
      targetLTiltPWM = tiltLPWMs[0];
      if (currentRTiltPWM == tiltRPWMs[0] and currentLTiltPWM == tiltLPWMs[0]) {
        horizontalMode = true;
      }
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
