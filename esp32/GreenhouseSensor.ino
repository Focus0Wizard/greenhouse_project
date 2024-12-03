// GreenhouseSensor.ino

#include <WiFi.h>
#include "Sensor.h"
#include "LDR.h"
#include "DHTSensor.h"
#include "MQTTClient.h"
#include <ArduinoJson.h>

#define LDR_PIN 34     
#define DHT_PIN 4      
#define DHT_TYPE DHT11 

const char *WIFI_SSID = "JhonnP";
const char *WIFI_PASS = "alberto1234";
const char *MQTT_BROKER = "abkb9nclg5hh-ats.iot.us-east-1.amazonaws.com";
const int MQTT_PORT = 8883;
const char *CLIENT_ID = "ESP32_SENSOR";

const char AMAZON_ROOT_CA1[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
... Tu certificado CA aquí ...
-----END CERTIFICATE-----
)EOF";

const char CERTIFICATE[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
... Tu certificado de cliente aquí ...
-----END CERTIFICATE-----
)KEY";

const char PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
... Tu clave privada aquí ...
-----END RSA PRIVATE KEY-----
)KEY";

LDR ldrSensor(LDR_PIN, 500);
DHTSensor dhtSensor(DHT_PIN, DHT_TYPE, 25, 35);

MQTTClientWrapper mqttClient(MQTT_BROKER, MQTT_PORT);

class GreenhouseSensor
{
private:
    Sensor *ldr;
    Sensor *dht;
    MQTTClientWrapper *mqtt;
    const char *mqttTopic;

    int prevLdrState;
    int prevDhtState;

public:
    GreenhouseSensor(Sensor *ldrSensor, Sensor *dhtSensor, MQTTClientWrapper *mqttClient, const char *topic)
        : ldr(ldrSensor), dht(dhtSensor), mqtt(mqttClient), mqttTopic(topic),
          prevLdrState(-1), prevDhtState(-1) {}

    void setupSensor(const char *caCert, const char *clientCert, const char *privateKey, const char *clientId)
    {
        Serial.begin(115200);
        delay(10);
        Serial.println();
        Serial.print("Connecting to WiFi...");

        WiFi.begin(WIFI_SSID, WIFI_PASS);
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }
        Serial.println("\nConnected to WiFi");

        mqtt->setCallback(nullptr); 
        mqtt->connect(clientId, caCert, clientCert, privateKey);
    }

    void readAndPublish()
    {
        int ldrState = ldr->getState();

        int dhtState = dht->getState(); 

        Serial.print("LDR State: ");
        Serial.println(ldrState);
        Serial.print("Temperature State: ");
        Serial.println(dhtState);

        if (ldrState != prevLdrState || dhtState != prevDhtState)
        {
            prevLdrState = ldrState;
            prevDhtState = dhtState;

            StaticJsonDocument<200> jsonDoc;
            jsonDoc["state"]["reported"]["luxState"] = ldrState;
            jsonDoc["state"]["reported"]["tempState"] = dhtState;

            char jsonBuffer[512];
            serializeJson(jsonDoc, jsonBuffer);

            mqtt->publishMessage(mqttTopic, jsonBuffer);

            Serial.println("Changed status. Posting new state...");
            Serial.print("luxState: ");
            Serial.println(ldrState);
            Serial.print("tempState: ");
            Serial.println(dhtState);
        }
    }
};

GreenhouseSensor greenhouseSensor(&ldrSensor, &dhtSensor, &mqttClient, "$aws/things/greenhouse_0001/shadow/update");

void setup()
{
    greenhouseSensor.setupSensor(AMAZON_ROOT_CA1, CERTIFICATE, PRIVATE_KEY, CLIENT_ID);
}

void loop()
{
    if (!mqttClient.isConnected())
    {
        Serial.println("Reconnecting to MQTT...");
        mqttClient.connect(CLIENT_ID, AMAZON_ROOT_CA1, CERTIFICATE, PRIVATE_KEY);
    }
    mqttClient.loopClient();

    greenhouseSensor.readAndPublish();

    delay(5000); 
}
