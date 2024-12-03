#include <WiFi.h>
#include "ServoActuator.h"
#include "MQTTClient.h"

#define SERVO_BLINDS_PIN 22 
#define SERVO_IRRIGATOR_PIN 26     

const char *WIFI_SSID = "JhonnP";
const char *WIFI_PASS = "alberto1234";

const char *MQTT_BROKER = "abkb9nclg5hh-ats.iot.us-east-1.amazonaws.com";
const int MQTT_PORT = 8883;
const char *MQTT_TOPIC = "$aws/things/greenhouse_0001/shadow/update";
const char *CLIENT_ID = "ESP32_ACTUADOR";

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

class GreenhouseActuator;

GreenhouseActuator *globalActuatorInstance = nullptr;

class GreenhouseActuator
{
private:
    ServoActuator servoBlinds;
    ServoActuator servoIrrigator;
    MQTTClientWrapper mqttClient;
    const char *mqttTopic;

    int prevLuxState;
    int prevTempState;

public:
    GreenhouseActuator(int blindsPin, int irrigatorPin, const char *broker, int port, const char *topic)
        : servoBlinds(blindsPin), servoIrrigator(irrigatorPin),
          mqttClient(broker, port), mqttTopic(topic),
          prevLuxState(-1), prevTempState(-1)
    {
        globalActuatorInstance = this;
    }

    void setupActuators(const char *caCert, const char *clientCert, const char *privateKey, const char *clientId)
    {
        servoBlinds.attachServo();
        servoIrrigator.attachServo();

        mqttClient.setCallback(mqttCallback);
        mqttClient.connect(clientId, caCert, clientCert, privateKey);
        mqttClient.subscribeTopic(mqttTopic);
    }

    void handleMessage(char *topic, byte *payload, unsigned int length)
    {
        Serial.print("Message received in the topic: ");
        Serial.println(topic);

        String message = "";
        for (unsigned int i = 0; i < length; i++)
            message += (char)payload[i];

        Serial.println("Status of received shadow:");
        Serial.println(message);

        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, message);
        if (error)
        {
            Serial.print("Error parsing JSON: ");
            Serial.println(error.c_str());
            return;
        }

        JsonObject state = doc["state"]["reported"];

        if (state.containsKey("luxState"))
        {
            int angleBlinds = state["luxState"];
            if (angleBlinds != prevLuxState)
            {                                   
                prevLuxState = angleBlinds; 
                if (angleBlinds == 1)
                {
                    Serial.print("Moving blinds to: ");
                    Serial.println(120);
                    servoBlinds.moveTo(120); 
                }
                else
                {
                    Serial.print("Moving blinds to: ");
                    Serial.println(0);
                    servoBlinds.moveTo(0); 
                }
            }
        }

        if (state.containsKey("tempState"))
        {
            int angleIrrigator = state["tempState"];
            if (angleIrrigator != prevTempState)
            {                                
                prevTempState = angleIrrigator; 
                if (angleIrrigator == 1)
                {
                    Serial.print("Moving irrigation to: ");
                    Serial.println(60);
                    servoIrrigator.moveTo(60);
                    delay(3000);
                    servoIrrigator.moveTo(90);
                }
                else
                {
                    Serial.print("Moving irrigation to: ");
                    Serial.println(120);
                    servoIrrigator.moveTo(120);
                    delay(3000);
                    servoIrrigator.moveTo(90);
                }
            }
        }
    }

    void loopActuators()
    {
        if (!mqttClient.isConnected())
        {
            Serial.println("Reconnecting to MQTT...");
            mqttClient.connect(CLIENT_ID, AMAZON_ROOT_CA1, CERTIFICATE, PRIVATE_KEY);
        }
        mqttClient.loopClient();
    }

    static void mqttCallback(char *topic, byte *payload, unsigned int length)
    {
        if (globalActuatorInstance != nullptr)
            globalActuatorInstance->handleMessage(topic, payload, length);
    }
};

GreenhouseActuator greenhouseActuator(SERVO_BLINDS_PIN, SERVO_IRRIGATOR_PIN, MQTT_BROKER, MQTT_PORT, MQTT_TOPIC);

void connectWiFi()
{
    Serial.print("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting...");
    }
    Serial.println("Connected to WiFi");
}

void setup()
{
    Serial.begin(115200);

    connectWiFi();

    greenhouseActuator.setupActuators(AMAZON_ROOT_CA1, CERTIFICATE, PRIVATE_KEY, CLIENT_ID);
}

void loop()
{
    greenhouseActuator.loopActuators();
}
