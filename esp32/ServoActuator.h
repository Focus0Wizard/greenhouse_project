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
  ServoActuator(int pin, int activePos, int inactivePos) : pin(pin), 
                                                           activePos(activePos), 
                                                           inactivePos(inactivePos) {}

  void initialize()
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

  void moveTo(int position)
  {
    servo.write(position);
  }
};

#endif