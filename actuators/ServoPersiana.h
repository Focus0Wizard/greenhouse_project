// ServoPersiana.h

#ifndef SERVOPERSIANA_H
#define SERVOPERSIANA_H

#include "Actuador.h"
#include <Servo.h>

class ServoPersiana : public Actuador {
private:
    Servo servo;
    int pin;
public:
    ServoPersiana(int pin);
    void actuate(int state) override;
};

#endif
