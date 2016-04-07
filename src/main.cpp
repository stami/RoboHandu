#include "Arduino.h"
#include <Servo.h>


void driveServoToNewPosition(int);
bool changeServoAngle(int servo_num, int delta);
int  angle(int);
int  potValueToDelta(int);
void readCtrl();
void printCtrlValues();
void calibrateController();


// Controller's potentiometers are connected to these pins
const int ctrl_right_X   = 0,
          ctrl_right_Y   = 1,
          ctrl_left_X    = 2,
          ctrl_left_Y    = 3,

          ctrl_right_2   = 4,
          ctrl_left_2    = 5,

          ctrl_right_Btn = 6,
          ctrl_left_Btn  = 7;

int ctrl_values[8] = { 0 };
int ctrl_pot_initials[6] = { 0 };


// Servo objects
Servo parallax;
Servo bottom;
Servo mid;
Servo grab_left;
Servo grab_right;

// Hold current angle of the servo
// 0: parallax
// 1: bottom
// 2: mid
// 3: grab_left
// 4: grab_right
int servo_angles[5] = { 90, 90, 90, 90, 90 };




void setup() {
  Serial.begin(9600);

  // Attach servo objects to correct pins
  parallax.attach(2);
  bottom.attach(3);
  mid.attach(4);
  grab_left.attach(6);
  grab_right.attach(7);

  calibrateController();

  // for(int i = 0; i < 6; i++) {
  //   Serial.println(ctrl_pot_initials[i]);
  // }

}


void loop() {

  // Read controller's potentiometers to values-array
  readCtrl();

  // Read each controller's potentiometer
  for (int ctrl = 0; ctrl <= 7; ctrl++) {

    switch (ctrl) {
      case ctrl_right_X:
      case ctrl_right_Y:
      case ctrl_right_2:
      case ctrl_left_X:
      case ctrl_left_Y:
      case ctrl_left_2:
        driveServoToNewPosition(ctrl);
        break;

      default:
        // For buttons, do nothing
        break;

    }

  }

  printCtrlValues();

  delay(100);
}



void driveServoToNewPosition(int ctrl) {

  int delta = potValueToDelta(ctrl_values[ctrl]);
  if (delta == 0) {
    return;
  }

  switch (ctrl) {
    // case ctrl_right_X:
    case ctrl_right_Y:
      if (changeServoAngle(1, delta)) {
        bottom.write(servo_angles[1]);
        //Serial.print("bottom kulmaan "); Serial.print(servo_angles[1]); Serial.print("\n");
      }
      break;
    // case ctrl_right_2:
    // case ctrl_left_X:
    case ctrl_left_Y:
      if (changeServoAngle(2, delta)) {
        mid.write(servo_angles[2]);
        //Serial.print("mid kulmaan "); Serial.print(servo_angles[2]); Serial.print("\n");
      }
      break;
    // case ctrl_left_2:
    //   break;
    default:
      break;
      // For buttons, do nothing
  }

}

bool changeServoAngle(int servo_num, int delta) {

  int angle = servo_angles[servo_num];

  if ((angle <= 1 && delta < 1) || (angle >= 176 && delta > -1)) {
    return false;
  }

  if (angle + delta <= 1) {
    servo_angles[servo_num] = 1;
  } else if (angle + delta >= 176) {
    servo_angles[servo_num] = 176;
  } else {
    servo_angles[servo_num] += delta;
  }
  return true;

}


int angle( int potvalue ) {
  if (potvalue < 400 || potvalue > 600) {
    return map(potvalue, 0, 1023, 0, 180);
  }
  return 90;
}

int potValueToDelta( int val ) {
  // https://www.arduino.cc/en/Reference/Map
  if (val < 400 || val > 600) {
    return map(val, 0, 1023, -5, 5);
  }
  return 0;
}


// Read controller's potentiometers
void readCtrl() {
  int deadzone = 40; // plus-minus
  int reading = 0;

  // Thumbsticks
  for (int ctrl = 0; ctrl <= 3; ctrl++) {
    // Snap to center (deadzone)
    reading = analogRead(ctrl);
    if (reading < ctrl_pot_initials[ctrl] - deadzone || reading > ctrl_pot_initials[ctrl] + deadzone) {
      ctrl_values[ctrl] = reading;
    } else {
      ctrl_values[ctrl] = 500;
    }
  }

  // L2 and R2
  for (int ctrl = 4; ctrl <= 5; ctrl++) {
    reading = analogRead(ctrl);
    ctrl_values[ctrl] = reading;
  }

  // Thumbstick buttons
  for (int ctrl = 6; ctrl <= 7; ctrl++) {
    reading = analogRead(ctrl);
    // ctrl_values[ctrl] = reading;
    ctrl_values[ctrl] = (reading > 100) ? 1 : 0;
  }

}


void printCtrlValues() {
  // print the values to serial console
  for (int i = 0; i < 8; i++) {
    Serial.print(ctrl_values[i]);
    Serial.print("\t");
  }
  Serial.print("\n");
}


void calibrateController() {
  // Take 10 values from potentiometer
  // Calculate average of them (to integer)
  // Use the average as the center value for the potentiometer

  // For each controller's potentiometer
  for (int ctrl = 0; ctrl <= 6; ctrl++) {
    for (int i = 0; i < 10; i++) {
      ctrl_pot_initials[ctrl] += analogRead(ctrl);
    }
    ctrl_pot_initials[ctrl] /= 10;
  }

}
