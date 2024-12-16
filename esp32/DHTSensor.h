#ifndef DHTSENSOR_H
#define DHTSENSOR_H

#include "Sensor.h"
#include <DHT.h>

class DHTSensor : public Sensor
{
private:
  DHT dht;        
  float lastTemperature; 

public:
  DHTSensor(int pin, int type) 
    : dht(pin, type), 
      lastTemperature(-1) {}

  void initialize()
  {
    dht.begin();
  }

  float readData() override
  {
    float temperature = dht.readTemperature();
    if (!isnan(temperature)) lastTemperature = temperature; 
    return lastTemperature;
  }

  int getState() override
  {
    if (lastTemperature > 0.0 && lastTemperature < 25.0) return 0; 
    else if (lastTemperature >= 25.0 && lastTemperature < 35.0) return 1;
    else return -1; 
  }
};

#endif
