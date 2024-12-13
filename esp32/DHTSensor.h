#ifndef DHT11_H
#define DHT11_H

#include "Sensor.h"
#include <DHT.h>

class DHT11Sensor : public Sensor
{
private:
  DHT dht;
  float lastTemp;

public:
  DHT11Sensor(int pin) : dht(pin, DHT11), lastTemp(-1) {}

  void begin()
  {
    dht.begin();
  }

  float readData() override
  {
    float t = dht.readTemperature();
    if (!isnan(t)) lastTemp = t;
    return lastTemp;
  }

  int getState() override
  {
    if (lastTemp > 0.0 && lastTemp < 25.0) return 0; 
    else if (lastTemp >= 25.0 && lastTemp < 35.0) return 1; 
  }
};

#endif
