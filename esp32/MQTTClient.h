#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

typedef void (*MQTTCallbackFunction)(char *, byte *, unsigned int);

class MQTTClientWrapper
{
private:
    WiFiClientSecure wifiClient;
    PubSubClient client;
    MQTTCallbackFunction callbackFunction;

public:
    MQTTClientWrapper(const char *broker, int port) : client(broker, port, nullptr, wifiClient), callbackFunction(nullptr) {}

    void setCallback(MQTTCallbackFunction callback)
    {
        callbackFunction = callback;
        client.setCallback(callbackFunction);
    }

    bool connect(const char *clientId, const char *caCert, const char *clientCert, const char *privateKey)
    {
        wifiClient.setCACert(caCert);
        wifiClient.setCertificate(clientCert);
        wifiClient.setPrivateKey(privateKey);

        while (!client.connected())
        {
            Serial.print("Conectando a MQTT...");
            if (client.connect(clientId))
            {
                Serial.println("Conectado a MQTT");
                return true;
            }
            else
            {
                Serial.print("Error en la conexión MQTT: ");
                Serial.println(client.state());
                delay(5000);
            }
        }
        return false;
    }

    void subscribeTopic(const char *topic)
    {
        client.subscribe(topic);
    }

    void publishMessage(const char *topic, const char *payload)
    {
        client.publish(topic, payload);
    }

    void loopClient()
    {
        client.loop();
    }

    bool isConnected()
    {
        return client.connected();
    }
};