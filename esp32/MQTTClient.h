#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

class MQTTClient
{
private:
  PubSubClient *client;
  WiFiClientSecure wifiClient;
  String shadowTopicUpdate;
  String shadowTopicAccepted;

  int prevLdrState;
  int prevDhtState;

  void (*userCallback)(int luxState, int tempState);

  static void internalCallback(char *topic, byte *payload, unsigned int length)
  {
    MQTTClient *instance = (MQTTClient *)PubSubClient::getDefaultCallbackObject();
    if (!instance) return;

    instance->handleCallback(topic, payload, length);
  }

  void handleCallback(char *topic, byte *payload, unsigned int length)
  {
    String message;
    for (unsigned int i = 0; i < length; i++) message += (char)payload[i];

    DynamicJsonDocument doc(1024);
    DeserializationError err = deserializeJson(doc, message);
    if (err) return;

    if (!doc["state"]["reported"].isNull())
    {
      JsonObject state = doc["state"]["reported"];

      int luxState = state["luxState"] | -1;
      int tempState = state["tempState"] | -1;

      if (luxState != -1 && tempState != -1 && userCallback) userCallback(luxState, tempState);
    }
  }

public:
  MQTTClient(const char *broker, int port, const char *rootCA, const char *cert, const char *key, const char *clientId)
      : prevLdrState(-1), prevDhtState(-1), userCallback(nullptr)
  {
    wifiClient.setCACert(rootCA);
    wifiClient.setCertificate(cert);
    wifiClient.setPrivateKey(key);
    client = new PubSubClient(wifiClient);
    client->setServer(broker, port);
    PubSubClient::setDefaultCallbackObject(this);

    shadowTopicUpdate = String("$aws/things/greenhouse_0001/shadow/update");
    shadowTopicAccepted = String("$aws/things/greenhouse_0001/shadow/update/accepted");
  }

  void setCallback(void (*cb)(int, int))
  {
    userCallback = cb;
    client->setCallback(internalCallback);
  }

  bool connect(const char *clientId)
  {
    while (!client->connected())
    {
      if (client->connect(clientId)) client->subscribe(shadowTopicAccepted.c_str());
      else delay(5000);
    }
    return true;
  }

  void loop()
  {
    client->loop();
  }

  void publishState(int luxState, int tempState)
  {
    if (luxState == prevLdrState && tempState == prevDhtState) return;
    
    prevLdrState = luxState;
    prevDhtState = tempState;

    StaticJsonDocument<200> jsonDoc;
    jsonDoc["state"]["reported"]["luxState"] = luxState;
    jsonDoc["state"]["reported"]["tempState"] = tempState;

    char jsonBuffer[512];
    serializeJson(jsonDoc, jsonBuffer);
    client->publish(shadowTopicUpdate.c_str(), jsonBuffer);
  }
};

#endif