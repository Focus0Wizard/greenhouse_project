#include <ESP32Servo.h>
#include "Actuator.h"

class ServoActuator : public Actuator
{
private:
    Servo servo;
    int pin;
    int currentAngle;

public:
    ServoActuator(int servoPin) : pin(servoPin), currentAngle(90) {}

    void attachServo()
    {
        servo.attach(pin);
        servo.write(currentAngle);
    }

    void moveTo(int angle)
    {
        Serial.print("Moviendo servo en el pin ");
        Serial.print(pin);
        Serial.print(" a: ");
        Serial.println(angle);
        servo.write(angle);
        currentAngle = angle;
    }

    void activate() override
    {
    }

    void deactivate() override
    {
    }
};