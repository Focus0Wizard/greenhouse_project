#ifndef GREENHOUSE_H
#define GREENHOUSE_H

#include "LDR.h"
#include "DHT11.h"
#include "ServoActuator.h"
#include "MQTTClient.h"
#include "WifiManager.h"
#include "Config.h"

class Greenhouse
{
private:
  LDR ldr;
  DHT11Sensor dht;

  ServoActuator persianas;
  ServoActuator riego;

  WifiManager wifi;
  MQTTClient mqtt;

  int currentLuxState;
  int currentTempState;

  static void onShadowUpdate(int luxState, int tempState)
  {
    Greenhouse *instance = (Greenhouse *)PubSubClient::getDefaultCallbackObject();
    if (!instance) return;
    instance->updateActuators(luxState, tempState);
  }

  void updateActuators(int luxState, int tempState)
  {
      // Luz alta -> cerrar persianas
    if (luxState == 1) persianas.deactivate();

    
    // Luz baja -> abrir persianas
    else persianas.activate();

    if (tempState == 1) // Temperatura alta -> activar riego
    {
      riego.activate();
      delay(3000);
      riego.moveTo(90); // posición neutral
    }
    else // Temperatura baja -> desactivar riego
    {
      riego.deactivate();
      delay(3000);
      riego.moveTo(90);
    }
  }

public:
  Greenhouse() : ldr(34, 500),
                 dht(4),
                 persianas(22, 120, 0),
                 riego(26, 60, 120),
                 wifi(WIFI_SSID, WIFI_PASS),
                 mqtt(MQTT_BROKER, MQTT_PORT, AMAZON_ROOT_CA1, CERTIFICATE, PRIVATE_KEY, CLIENT_ID),
                 currentLuxState(-1),
                 currentTempState(-1) {}

  void begin()
  {
    Serial.begin(115200);
    wifi.connect();
    dht.begin();
    persianas.attach();
    riego.attach();

    mqtt.setCallback(onShadowUpdate);
    mqtt.connect(CLIENT_ID);
  }

  void monitorAndControl()
  {
    mqtt.loop();

    float currentTemp = dht.readData();
    ldr.readData();

    int newLuxState = ldr.getState();
    int newTempState = dht.getState();

    mqtt.publishState(newLuxState, newTempState);
  }
};

#endif