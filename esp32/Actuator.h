#ifndef ACTUATOR_H
#define ACTUATOR_H

class Actuator
{
public:
    virtual void activate() = 0;
    virtual void deactivate() = 0;
    virtual ~Actuator() {}
};

#endif
