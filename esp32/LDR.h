#ifndef LDR_H
#define LDR_H

#include "Sensor.h"
#include <Arduino.h>

class LDR : public Sensor
{
private:
  int pin;       
  int threshold; 
  int lastValue; 

public:
  LDR(int pin, int threshold = 500) 
      : pin(pin), 
        threshold(threshold), 
        lastValue(-1) {}

  void initialize()
  {
    pinMode(pin, INPUT);
  }

  float readData() override
  {
    lastValue = analogRead(pin);
    return lastValue;
  }

  int getState() override
  {
    // Estado 1: Alta luz (valor analógico bajo)
    // Estado 0: Baja luz (valor analógico alto)
    return (lastValue < threshold) ? 1 : 0;
  }
};

#endif
