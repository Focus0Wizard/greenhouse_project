#ifndef DHT11SENSOR_H
#define DHT11SENSOR_H

#include "Sensor.h"
#include <DHT.h>

class DHT11Sensor : public Sensor
{
private:
    DHT dht;
    int state;

public:
    DHT11Sensor(int pin);
    float readData() override;
    int getState() override;
};

#endif
