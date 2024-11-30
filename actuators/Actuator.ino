// Actuator.ino

#include <Servo.h>
#include "MQTT.h"

// Definir los pines de los servos
#define SERVO_PERSIANAS_PIN 26 // Pin del servo para las persianas
#define SERVO_RIEGO_PIN 23     // Pin del servo para la válvula de riego

// Instanciar los objetos de los servos
Servo servoPersianas;
Servo servoRiego;

// Instanciar el objeto MQTT
MQTT mqtt;

void setup() {
    Serial.begin(115200);

    // Configuración MQTT
    mqtt.setup();

    // Inicializar los servos
    servoPersianas.attach(SERVO_PERSIANAS_PIN);
    servoRiego.attach(SERVO_RIEGO_PIN);
}

void loop() {
    // Simular el control de los servos (esto puede cambiar según el caso)
    // Abrir las persianas y la válvula de riego en función de la entrada MQTT
    if (conditionToOpen()) {
        servoPersianas.write(90);  // Abrir persianas
        servoRiego.write(90);      // Abrir válvula de riego
    } else {
        servoPersianas.write(0);   // Cerrar persianas
        servoRiego.write(0);       // Cerrar válvula de riego
    }

    // Llamar al loop de MQTT
    mqtt.loop();
    
    // Publicar el estado de los actuadores
    StaticJsonDocument<200> doc;
    doc["persianas"] = servoPersianas.read();
    doc["riego"] = servoRiego.read();
    
    char output[128];
    serializeJson(doc, output);
    
    mqtt.publishData(output);
    
    // Esperar 5 segundos antes de la siguiente acción
    delay(5000);
}
