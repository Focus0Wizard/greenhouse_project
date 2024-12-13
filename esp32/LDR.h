#ifndef LDR_H
#define LDR_H

#include "Sensor.h"
#include <Arduino.h>

class LDR : public Sensor
{
private:
  int pin;
  int threshold;
  float lastValue;

public:
  LDR(int _pin, int _threshold = 500) : pin(_pin), threshold(_threshold), lastValue(-1) {}

  float readData() override
  {
    int val = analogRead(pin);
    lastValue = val;
    return val;
  }

  int getState() override
  {
    // Estado 0: Bajo nivel de luz (valor alto de LDR)
    // Estado 1: Alto nivel de luz (valor bajo de LDR)
    // si val < threshold => luz alta
    return (lastValue < threshold) ? 1 : 0;
  }
};

#endif