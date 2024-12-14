#ifndef SERVOACTUATOR_H
#define SERVOACTUATOR_H

#include "Actuator.h"
#include <ESP32Servo.h>

class ServoActuator : public Actuator
{
private:
  Servo servo;     
  int pin;         
  int activePosition;   
  int inactivePosition; 

public:
  ServoActuator(int pin, int activePosition, int inactivePosition) 
    : pin(pin), 
      activePosition(activePosition), 
      inactivePosition(inactivePosition) {}

  void initialize()
  {
    servo.attach(pin);
    servo.write(inactivePosition); 
  }

  void activate() override
  {
    servo.write(activePosition);
  }

  void deactivate() override
  {
    servo.write(inactivePosition);
  }

  void moveTo(int position)
  {
    servo.write(position);
  }
};

#endif