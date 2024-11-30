// LDR.h

#ifndef LDR_H
#define LDR_H

#include "Sensor.h"

class LDR : public Sensor {
private:
    int pin;
    int state;
public:
    LDR(int pin);
    float readData() override;
    int getState() override;
};

#endif
