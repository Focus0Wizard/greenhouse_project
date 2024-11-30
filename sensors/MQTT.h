#ifndef MQTT_H
#define MQTT_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char *WIFI_SSID = "your_wifi_ssid";
const char *WIFI_PASS = "your_wifi_password";

const char *MQTT_BROKER = "your-iot-endpoint.amazonaws.com";
const int MQTT_PORT = 8883;
const char *CLIENT_ID = "ESP32_Device";

const char AMAZON_ROOT_CA1[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)EOF";

const char CERTIFICATE[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)KEY";

const char PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
-----END RSA PRIVATE KEY-----
)KEY";

const char *PUBLISH_TOPIC = "$aws/things/greenhouse_0001/shadow/update";
const char *SUBSCRIBE_TOPIC = "$aws/things/greenhouse_0001/shadow/update/delta";

class MQTT
{
private:
    WiFiClientSecure wiFiClient;
    PubSubClient client;

public:
    MQTT() : client(wiFiClient) {}

    void setup()
    {
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        Serial.print("Connected to WiFi");
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(1000);
            Serial.print(".");
        }
        Serial.println(" Connected!");

        wiFiClient.setCACert(AMAZON_ROOT_CA1);
        wiFiClient.setCertificate(CERTIFICATE);
        wiFiClient.setPrivateKey(PRIVATE_KEY);

        client.setServer(MQTT_BROKER, MQTT_PORT);
        client.setCallback(callback);

        connectToMQTT();
    }

    void connectToMQTT()
    {
        while (!client.connected())
        {
            Serial.print("Trying to connect to MQTT...");
            if (client.connect(CLIENT_ID))
            {
                Serial.println("Connected to MQTT!");
                client.subscribe(SUBSCRIBE_TOPIC); // Suscripción a los tópicos
            }
            else
            {
                Serial.print("Connection error, code: ");
                Serial.println(client.state());
                delay(5000);
            }
        }
    }

    void loop()
    {
        client.loop();
    }

    void publishData(const char *data)
    {
        client.publish(PUBLISH_TOPIC, data);
    }

    static void callback(char *topic, byte *payload, unsigned int length)
    {
        Serial.print("Message received: ");
        for (unsigned int i = 0; i < length; i++)
            Serial.print((char)payload[i]);
        Serial.println();
    }
};

#endif
