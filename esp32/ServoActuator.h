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
        Serial.print("Moving servo to pin ");
        Serial.print(pin);
        Serial.print(" to: ");
        Serial.println(angle);
        servo.write(angle);
        currentAngle = angle;
    }

void activate() override
{
    int anguloPersianas = state["luxState"];
    if (anguloPersianas != prevLuxState) { // Solo actuamos si hay un cambio
        prevLuxState = anguloPersianas; // Actualizamos el estado previo
        if (anguloPersianas == 1) {
            Serial.print("Moviendo persianas a: ");
            Serial.println(0);
            servoPersianas.write(0); // Mover el servo de las persianas
        } 
    }

    if (anguloRiego != prevTempState) { // Solo actuamos si hay un cambio
        prevTempState = anguloRiego; // Actualizamos el estado previo
        if (anguloRiego ==1) {
            Serial.print("Moviendo riego a: ");
            Serial.println(120);
            servoRiego.write(120);
            delay(3000);
            servoRiego.write(90);
        } 
    }
}

void deactivate() override
{
    int anguloPersianas = state["luxState"];
    if (anguloPersianas != prevLuxState) { // Solo actuamos si hay un cambio
        prevLuxState = anguloPersianas; // Actualizamos el estado previo
        if (anguloPersianas == 0) {
            Serial.print("Moviendo persianas a: ");
            Serial.println(0);
            servoPersianas.write(0); // Mover el servo de las persianas
        } 
    }

    if (anguloRiego != prevTempState) { // Solo actuamos si hay un cambio
        prevTempState = anguloRiego; // Actualizamos el estado previo
        if (anguloRiego == 0) {
            Serial.print("Moviendo riego a: ");
            Serial.println(120);
            servoRiego.write(120);
            delay(3000);
            servoRiego.write(90);
        } 
    }
}


};