class Actuator
{
public:
    virtual void activate() = 0;
    virtual void deactivate() = 0;
    virtual ~Actuator() {}
};
