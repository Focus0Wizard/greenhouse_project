// Actuador.h

#ifndef ACTUADOR_H
#define ACTUADOR_H

// Clase base Actuador
class Actuador {
public:
    virtual void actuate(int state) = 0; // Método para accionar el actuador según el estado
};

#endif
