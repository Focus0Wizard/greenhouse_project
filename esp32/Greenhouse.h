#ifndef GREENHOUSE_H
#define GREENHOUSE_H

#include "LDR.h"
#include "DHTSensor.h"
#include "ServoActuator.h"
#include "MQTTClient.h"
#include "WifiManager.h"
#include "Config.h"

class Greenhouse
{
private:
  LDR ldr;
  DHTSensor dht;

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
    if (luxState == 1) persianas.deactivate(); // Luz alta -> cerrar persianas
    else persianas.activate(); // Luz baja -> abrir persianas

    if (tempState == 1) riego.activate(); // Temperatura alta -> activar riego
    else riego.deactivate(); // Temperatura baja -> desactivar riego
  }

public:
  Greenhouse()
      : ldr(LDR_PIN, LDR_THRESHOLD),
        dht(DHT11_PIN, DHT11_TYPE),
        persianas(BLIND_PIN, BLIND_ACTIVE_POSITION, BLIND_INACTIVE_POSITION),
        riego(IRRIGATION_PIN, IRRIGATION_ACTIVE_POSITION_PIN, IRRIGATION_INACTIVE_POSITION),
        wifi(WIFI_SSID, WIFI_PASS),
        mqtt(MQTT_BROKER, MQTT_PORT, AMAZON_ROOT_CA1, CERTIFICATE, PRIVATE_KEY, CLIENT_ID),
        currentLuxState(INVALID_STATE),
        currentTempState(INVALID_STATE) {}

  void begin()
  {
    Serial.begin(115200);
    wifi.connect();
    dht.initialize();
    persianas.initialize();
    riego.initialize();

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

    if (newLuxState != currentLuxState || newTempState != currentTempState)
    {
      currentLuxState = newLuxState;
      currentTempState = newTempState;

      mqtt.publishState(newLuxState, newTempState);
    }
  }
};

#endif
