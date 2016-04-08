#include "Arduino.h"
#include <Servo.h>



void driveServoToNewPosition(int);
bool changeServoAngle(int servo_num, int delta);
int  angle(int);
int  deltaForCtrl(int);
void readCtrl();
void printCtrlValues();
void calibrateController();

void handleThumbsticks();
void handleL2R2();


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

const int ctrl_deadzone = 40; // plus-minus


// Servo objects
Servo parallax;
Servo bottom;
Servo mid;
Servo grab_left;
Servo grab_right;

const int servo_parallax   = 0,
          servo_bottom     = 1,
          servo_mid        = 2,
          servo_grab_left  = 3,
          servo_grab_rigth = 4;

int servo_angles[5] = { 90, 90, 90, 90, 90 };




void setup() {
  Serial.begin(9600);

  // Attach servo objects to correct pins
  parallax.attach(2);
  bottom.attach(3);
  mid.attach(4);
  grab_left.attach(6);
  grab_right.attach(7);


  // for(int i = 0; i < 6; i++) {
  //   Serial.println(ctrl_pot_initials[i]);
  // }

  // Wait for right button before continuing

  int button;
  bool pressed = false;

  do {
    bottom.write(90);
    mid.write(90);
    grab_left.write(90);
    grab_right.write(90);
    button = analogRead(ctrl_right_Btn);
    pressed = (button < 100);
    delay(15);
  } while (!pressed);

  calibrateController();

  Serial.println("Program started!");

}


void loop() {

  // Read controller's potentiometers to values-array
  readCtrl();

  handleThumbsticks();
  handleL2R2();

  //printCtrlValues();

  delay(50);
}


void handleThumbsticks() {
  int delta;

  // Parallax servo
  // TODO

  // Bottom servo
  delta = deltaForCtrl(ctrl_right_Y);
  // Serial.println(delta);
  if (delta != 0 && changeServoAngle( servo_bottom, delta )) {
    Serial.print("bottom to "); Serial.println(servo_angles[servo_bottom]);
    bottom.write(servo_angles[ servo_bottom ]);
  }

  // Mid servo
  delta = deltaForCtrl(ctrl_left_Y);
  // Serial.println(delta);
  if (delta != 0 && changeServoAngle( servo_mid, delta )) {
    Serial.print("mid to "); Serial.println(servo_angles[servo_mid]);
    mid.write(servo_angles[ servo_mid ]);
  }

  // Serial.println(" ");

}

void handleL2R2() {

}



/**
 * Change angle of a servo
 * @param  servo_num The servo
 * @param  delta     Amount of change
 * @return           True if change succeeded
 *                   False if servo is already on limit
 */
bool changeServoAngle(int servo_num, int delta) {

  int angle = servo_angles[servo_num];

  // Check if servo is already on min or max angle
  if ((angle <= 1 && delta < 1) || (angle >= 170 && delta > -1)) {
    return false;
  }

  if (angle + delta <= 1) {
    servo_angles[servo_num] = 1;
  } else if (angle + delta >= 170) {
    servo_angles[servo_num] = 170;
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


int deltaForCtrl( int ctrl ) {

  int value = ctrl_values[ctrl];
  int initial = ctrl_pot_initials[ctrl];

  if (value < initial - ctrl_deadzone) {
     return map(value, 0, initial - ctrl_deadzone, -5, 0);
  } else if (value > initial + ctrl_deadzone) {
    return map(value, initial + ctrl_deadzone, 1023, 0, 5);
  } else {
     return 0;
   }
}


// Read controller's potentiometers
void readCtrl() {
  int reading = 0;

  // Thumbsticks, L2 and R2
  for (int ctrl = 0; ctrl <= 5; ctrl++) {
    reading = analogRead(ctrl);
    ctrl_values[ctrl] = reading;
  }

  // Thumbstick buttons -> 0/1
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
