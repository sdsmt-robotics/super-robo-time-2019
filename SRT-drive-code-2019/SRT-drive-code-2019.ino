#include "L289N.h"
#include <PS3BT.h>

USB Usb;
BTD Btd(&Usb);
PS3BT PS3(&Btd);

L289N leftMotor(2, 3, 4); //setup a motor object with pins 2 and 3 controlling direction and 4 controlling speed
L289N rightMotor(11, 12, 13); //same idea here, the true will invert the direction of the motor

int leftSpeed, rightSpeed;

void setup() {
  Serial.begin(9600);

  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  Serial.print(F("\r\nPS3 Bluetooth Library Started"));
}

void loop() {
  Usb.Task();

  if (PS3.PS3Connected)
  {
      leftSpeed = joyToPWM(PS3.getAnalogHat(LeftHatY));
      rightSpeed = joyToPWM(PS3.getAnalogHat(RightHatY));

      if (PS3.getAnalogButton(R2) < 50 && PS3.getAnalogButton(L2) < 50)
      {
        leftSpeed *= 0.5;
        rightSpeed *= 0.5;
      }

      leftMotor.setSpeedDirection(leftSpeed);
      rightMotor.setSpeedDirection(rightSpeed);

      Serial.print(leftSpeed);
      Serial.print("  ");
      Serial.println(rightSpeed);
  }
}

int joyToPWM(int joyVal)
{
  int preCurve;
  
  if (joyVal > 137)
  {
    preCurve = -(map(joyVal, 137, 255, 0, 255));
  }
  else if (joyVal < 117)
  {
    preCurve = (-2.18*joyVal)+255;
  }
  else
  {
    preCurve = 0;
  }

  if (preCurve > 0)
    return pow(2.718, 0.0198 * preCurve) + 99;
  else if (preCurve < 0)
    return -(pow(2.718, 0.0198 * abs(preCurve)) + 99);
  else
    return 0;
}
