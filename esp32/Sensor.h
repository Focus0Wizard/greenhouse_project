#ifndef SENSOR_H
#define SENSOR_H

class Sensor
{
public:
    virtual float readData() = 0;
    virtual int getState() = 0;  
    virtual ~Sensor() {}
};

#endif
