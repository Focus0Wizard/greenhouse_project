#ifndef SERVOACTUATOR_H
#define SERVOACTUATOR_H

#include "Actuator.h"
#include <ESP32Servo.h>

class ServoActuator : public Actuator
{
private:
  Servo servo;
  int pin;
  int activePos;
  int inactivePos;

public:
  ServoActuator(int _pin, int _activePos, int _inactivePos) 
  : pin(_pin), 
  activePos(_activePos), 
  inactivePos(_inactivePos) {}

  void attach()
  {
    servo.attach(pin);
    servo.write(inactivePos);
  }

  void activate() override
  {
    servo.write(activePos);
  }

  void deactivate() override
  {
    servo.write(inactivePos);
  }

  void moveTo(int pos)
  {
    servo.write(pos);
  }
};

#endif
