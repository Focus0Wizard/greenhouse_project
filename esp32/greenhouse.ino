#include "Greenhouse.h"

Greenhouse greenhouse;

void setup()
{
  greenhouse.begin();
}

void loop()
{
  greenhouse.monitorAndControl();
  delay(5000);
}
