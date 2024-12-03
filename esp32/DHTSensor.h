#include "Sensor.h"
#include <DHT.h>

class DHTSensor : public Sensor
{
private:
    DHT dht;
    int tempThresholdLow;
    int tempThresholdHigh;

public:
    DHTSensor(int dhtPin, int type, int threshLow = 25, int threshHigh = 35)
        : dht(dhtPin, type), tempThresholdLow(threshLow), tempThresholdHigh(threshHigh)
    {
        dht.begin();
    }

    float readTemperature()
    {
        return dht.readTemperature();
    }

    float readHumidity()
    {
        return dht.readHumidity();
    }

    int getState() override
    {
        float temp = readTemperature();
        if (temp > 0.0 && temp < tempThresholdLow)
            return 0;
        else if (temp >= tempThresholdLow && temp < tempThresholdHigh)
            return 1;
        return -1;
    }
};
