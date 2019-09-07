#include "L289N.h"
#include <PS3BT.h>

USB Usb;
BTD Btd(&Usb);
PS3BT PS3(&Btd);

L289N leftMotor(2, 3, 4); //setup a motor object with pins 2 and 3 controlling direction and 4 controlling speed
L289N rightMotor(11, 12, 13, true); //same idea here, the true will invert the direction of the motor

int leftSpeed, rightSpeed;

void setup() {
  if (Usb.Init() == -1) {
    while (1); //halt
  }

  Serial.begin(9600);
}

void loop() {
  Usb.Task(); //this wil manage the controller connection

  if (PS3.PS3Connected) //only run the drive code if the controller is connected
  {
      //grab the analog value from the joystick and run it through the
      //  joyToPWM function to translate it to something the L289N library
      //  can use
      leftSpeed = joyToPWM(PS3.getAnalogHat(LeftHatY));
      rightSpeed = joyToPWM(PS3.getAnalogHat(RightHatY));

      //if the triggers aren't being pressed, cut the speed in half
      //  to allow for better control
      if (PS3.getAnalogButton(R2) < 50 && PS3.getAnalogButton(L2) < 50)
      {
        leftSpeed *= 0.5;
        rightSpeed *= 0.5;
      }

      //set the speed and direction of the motors using the L289N library
      leftMotor.setSpeedDirection(leftSpeed);
      rightMotor.setSpeedDirection(rightSpeed);
  }
}

//a function to map a raw joystick value to a value that works with
//  the L289N library
int joyToPWM(int joyVal)
{
  int preCurve;
  int postCurve;

  //if joyVal is >137 or <117, it's outside of the deadzone and
  // power should be send to the motors
  if (joyVal > 137)
  {
    preCurve = -(map(joyVal, 137, 255, 0, 255));
  }
  else if (joyVal < 117)
  {
    preCurve = (-2.18*joyVal)+255;
  }
  //otherwise, it must be in the deadzone and the motors should not
  // receive power
  else
  {
    return 0;
  }

  //apply an easing curve to the value
  postCurve = pow(2.718, 0.0198 * abs(preCurve)) + 99;

  //the curve doesn't handle negative values, manually negate if needed
  if (preCurve < 0)
    postCurve = -postCurve;

  return postCurve;
}
