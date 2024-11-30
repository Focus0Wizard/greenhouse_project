#include <DHT.h>
#include "MQTT.h"
#include "LDR.h"
#include "DHT11Sensor.h"

#define LDR_PIN 34            
#define DHT_PIN 4             
#define DHT_TYPE DHT11        

LDR ldr(LDR_PIN);
DHT11Sensor dht11(DHT_PIN);

MQTT mqtt;

void setup() {
    Serial.begin(115200);
    
    mqtt.setup();
    
    dht11.begin();
}

void loop() {
    float lightLevel = ldr.readData();
    float temperature = dht11.readData();

    StaticJsonDocument<200> doc;
    doc["lightLevel"] = lightLevel;
    doc["temperature"] = temperature;

    char output[128];
    serializeJson(doc, output);

    mqtt.publishData(output);
    
    delay(5000);
    
    mqtt.loop();
}

