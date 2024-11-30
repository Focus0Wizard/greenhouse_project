// Sensor.ino

#include <DHT.h>
#include "MQTT.h"
#include "LDR.h"
#include "DHT11Sensor.h"

// Definir los pines de los sensores
#define LDR_PIN 34            // Pin analógico para el LDR
#define DHT_PIN 4             // Pin para el DHT11
#define DHT_TYPE DHT11        // Tipo de sensor DHT

// Instanciar los objetos de sensores
LDR ldr(LDR_PIN);
DHT11Sensor dht11(DHT_PIN);

// Instanciar el objeto MQTT
MQTT mqtt;

void setup() {
    Serial.begin(115200);
    
    // Configuración MQTT
    mqtt.setup();
    
    // Inicializar los sensores
    dht11.begin();
}

void loop() {
    // Leer los datos de los sensores
    float lightLevel = ldr.readData();
    float temperature = dht11.readData();

    // Crear JSON para los datos de los sensores
    StaticJsonDocument<200> doc;
    doc["lightLevel"] = lightLevel;
    doc["temperature"] = temperature;

    char output[128];
    serializeJson(doc, output);

    // Publicar los datos en MQTT
    mqtt.publishData(output);
    
    // Esperar 5 segundos antes de la siguiente lectura
    delay(5000);
    
    // Llamar al loop de MQTT
    mqtt.loop();
}

