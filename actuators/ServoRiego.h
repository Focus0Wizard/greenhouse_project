// ServoRiego.h

#ifndef SERVORIEGO_H
#define SERVORIEGO_H

#include "Actuador.h"
#include <Servo.h>

class ServoRiego : public Actuador {
private:
    Servo servo;
    int pin;
public:
    ServoRiego(int pin);
    void actuate(int state) override;
};

#endif
