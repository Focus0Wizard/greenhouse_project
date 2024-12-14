#ifndef DHTSENSOR_H
#define DHTSENSOR_H

#include "Sensor.h"
#include <DHT.h>

class DHTSensor : public Sensor
{
private:
  DHT dht;        
  float lastTemp; 

public:
  DHTSensor(int pin, int type) 
    : dht(pin, type), 
      lastTemp(-1) {}

  void initialize()
  {
    dht.begin();
  }

  float readData() override
  {
    float temp = dht.readTemperature();
    if (!isnan(temp)) lastTemp = temp; 
    return lastTemp;
  }

  int getState() override
  {
    if (lastTemp > 0.0 && lastTemp < 25.0) return 0; 
    else if (lastTemp >= 25.0 && lastTemp < 35.0) return 1;
    else return -1; 
  }
};

#endif
