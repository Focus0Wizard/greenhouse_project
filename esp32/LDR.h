#include "Sensor.h"

class LDR : public Sensor
{
private:
    int pin;
    int threshold;

public:
    LDR(int ldrPin, int thresh = 500) : pin(ldrPin), threshold(thresh) {}

    float readData() override
    {
        return analogRead(pin);
    }

    int getState() override
    {
        float value = readData();
        return (value < threshold) ? 0 : 1;
    }
};