#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// Definimos los pines
#define LDR_PIN 34         // Pin analógico para el LDR
#define DHT_PIN  4         // Pin para el DHT11
#define DHT_TYPE DHT11     // Tipo de sensor DHT

// Definimos las credenciales de WiFi y MQTT
const char* WIFI_SSID = "JhonnP";
const char* WIFI_PASS = "alberto1234";
const char* MQTT_BROKER = "abkb9nclg5hh-ats.iot.us-east-1.amazonaws.com";
const int MQTT_PORT = 8883;
const char* CLIENT_ID = "ESP32_SENSOR";

// Certificado raíz de Amazon, certificado y clave privada
const char AMAZON_ROOT_CA1[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char CERTIFICATE[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVANHvsKz61Kv7QrmvDw4m1k92CDIuMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yNDExMjgxNzAz
MDZaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDTkDuSoiTyRRI1ZCj9
Ph1n/2+UyJKxy1DemxdXoSkX0Hm2b+xVKFS3ZvCA1k7nb+s7BVhS+/cO/P7R39lG
fN20PdV27Uf35KnweEMY9U26rWyuCphOa9381/+cw2lipRFZe3wX7LfC4zeZx/I+
cYmsb6MYeYBAlBxWatpkRC3/L0gixERG+zyELGup0OLCxDO8F6eRieqRpYkyiCbF
K5DH1UcOSLK3oMGxJWU47e0yhfWUWI6VJWXUmxXMqfEVFytB/b9nbGRb4lNWIW2u
cqNOYDi7anj2XY1rkl/W169kKVSUbG7abp6WIq/2Nb44sXEq5dq7G+fwAhL2DisE
V16PAgMBAAGjYDBeMB8GA1UdIwQYMBaAFMu8vvmR3Xw6Ir6VsyMevo5+RbWOMB0G
A1UdDgQWBBT3ISGA7Qb65QUjwQ2TtGtXGFmDOzAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAXsRyx7zVALR7FFQGVNeGoOQ3
avuB9oCJUJ6ZSARXBQWGXlm+TdvTbr+Scka5vZHory8WKE3iEebEMSxvKKisnPfd
VAko+4kMrX5J3C61Cb8hLxtpxG5MbOlzg6U4+tCOL7eVA4XeG3h34n0o6NXv3R5u
gbqkoTMSd/eebOZsHNMfBnWBP8gOzqifuUVzeMV4oCn0XmIEPA3Fe6RQZ70rgMi+
3GFxnGBu+yUTIVAYX4+cDy0S+z9slGNRvW2NbFDM0x9SuGuUrNxWoNKvGgyVl1hj
y5CjgF53GKQG1h4eKHj8fcrKajZr/ciA0Y/ek4Sdj4sekUrnhSLnzpLsz6TD9w==
-----END CERTIFICATE-----
)KEY";

const char PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEogIBAAKCAQEA05A7kqIk8kUSNWQo/T4dZ/9vlMiSsctQ3psXV6EpF9B5tm/s
VShUt2bwgNZO52/rOwVYUvv3Dvz+0d/ZRnzdtD3Vdu1H9+Sp8HhDGPVNuq1srgqY
Tmvd/Nf/nMNpYqURWXt8F+y3wuM3mcfyPnGJrG+jGHmAQJQcVmraZEQt/y9IIsRE
Rvs8hCxrqdDiwsQzvBenkYnqkaWJMogmxSuQx9VHDkiyt6DBsSVlOO3tMoX1lFiO
lSVl1JsVzKnxFRcrQf2/Z2xkW+JTViFtrnKjTmA4u2p49l2Na5Jf1tevZClUlGxu
2m6eliKv9jW+OLFxKuXauxvn8AIS9g4rBFdejwIDAQABAoIBAF5N05A4QJnRmh08
iVzg/ECoRveeNG2WQHUJ0uzXxLZ90lM1LraC5Mhf1WLIpKFbbRKA/m73K5h9w6SP
0I0wCa9XRLjfag4g2nN7WAVfCwfkWCZ4SI3kXS5gpesd2/MBu2sUtSsA9KSlndop
m1ykRYccuCmntziSDNOki7bYAVzWyZObxNsFTlBaMSOC+kKNs7atYyild8hD0JwK
H/UPfQQI0RFzeBg9A0idjpYM7NTGhm1v5uIl3Gz4k0t0Nc9FaQREAnd3CV1R1Ooe
k2Gr4VSSk3HLcER1/omXGu7jQwSSTSLU2Bdtdt+lWyXTWqbxY0+D4uva1F6HtgUd
WXro1tECgYEA+utSvAecpmvuvmXXgaXYCN2PpDHmy65VtORa8xLzBKqmXav3foea
e5Z+Fw1PkeF5mZzWV6utlSw2JjUq0CSFHY6+a03dSILRBZOzSs9UTIyJOklr+MSE
aS6LCT/T5K0nTrU9M0iwApifGUPf5LYkyvt1SPC8adWomZ9CWXhSllkCgYEA19jn
Hy/mpmE3c1xaNwb/TdKK1hx6qDf+j2+99i0+bUENdidzLs8+F0tTbAn0XmdkMPUP
kjurVnlBxErd1tnqaRp6V6dO+OWmXTWgkquq1vEOj7XAnUWf37jvW4PQGTMi2j9R
NVhU8tL3cLTa0Y8iD1PJsAjf3zAAsp3zRiylTycCgYB6W0Ju+m9DICK9qb/bMtoU
CKbA9KyM6ihVG33WRolG30hdKTx48Ps+BPbbmtwQURkQ9U/4VHlC4tpM62RDN2vK
LA9jGAcuEsyGoa2ub0e2RFmumJsUJhBDkjSuf/d97Ynbcbm0Rl++BFqg4V5OkxuB
9/kLZokaOtwKfYTHSEtNkQKBgBcFjXEoHMO2LsHhIcYouuZEBIbdtr9maYJG+s3c
+YGRpKvrfT+K3yaA5D7ruOKwyGDoYmbIcnfgqwESsyKNT5EyP8ZFlAi8Bch/Rccf
BDO0tkbhrc+Yc3rbMLxkDLgDWj5I7pJv/29+zOF6iLQ9PfbAiwx5hdc6OXjTm/Tq
Ih+VAoGAV/NG5KIRPUEI4UmbgqoebEyphRh1Is7xSA4QdI09iHY1OyOd7NtZGS0n
B3EPULGK3MsLW/dqTxeREtA/q/NoJ7Ze1au+knhrlOM0qfwY1KkDdkE3TmPuMef6
GKK8bkkMqlPduhFrXjRDDqZM6ujT7dbUAfCB5YTzFfhFjDiEXhI=
-----END RSA PRIVATE KEY-----
)KEY";

// Inicializamos el cliente WiFi y MQTT
WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);

// Inicializamos el sensor DHT11
DHT dht(DHT_PIN, DHT_TYPE);

// Conectamos el sensor a la red WiFi
void setup_wifi() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.print("Conectando a WiFi");
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Conectado a WiFi");
}

// Conectamos al servidor MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando a MQTT...");
    
    if (client.connect(CLIENT_ID, "unused", "unused")) {
      Serial.println("Conectado a MQTT");
    } else {
      Serial.print("Error de conexión, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void setup() {
  setup_wifi();
  
  // Conectar al broker MQTT
  client.setServer(MQTT_BROKER, MQTT_PORT);
  
  // Iniciar el sensor DHT11
  dht.begin();
  
  // Configurar la conexión segura de MQTT
  wifiClient.setCACert(AMAZON_ROOT_CA1);
  wifiClient.setCertificate(CERTIFICATE);
  wifiClient.setPrivateKey(PRIVATE_KEY);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // Leemos el estado del LDR (sensor de luz)
  int ldrValue = analogRead(LDR_PIN); // Leemos el valor analógico
  int ldrState = (ldrValue < 500) ? 1 : 0; // Si el valor es bajo, 0 (luz baja), si es alto, 1 (luz alta)
  
  // Leemos el estado del DHT11 (sensor de temperatura)
  float temp = dht.readTemperature(); // Leemos la temperatura
  int dhtState = (temp < 20.0) ? 0 : 1; // Si la temperatura es menor que 20°C, 0 (temperatura baja), de lo contrario, 1 (temperatura alta)
  
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["state"]["reported"]["luxState"] = ldrState;
  jsonDoc["state"]["reported"]["tempState"] = dhtState;
  
  char jsonBuffer[512];
  serializeJson(jsonDoc, jsonBuffer);
  
  // Publicar los estados en el tópico correspondiente
  client.publish("$aws/things/greenhouse_0001/shadow/update", jsonBuffer);

  // Esperar un momento antes de la siguiente lectura
  delay(5000);  // 2 segundos
}