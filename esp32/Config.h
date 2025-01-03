#ifndef CONFIG_H
#define CONFIG_H

#define DHT11_TYPE DHT11

const char *WIFI_SSID = "JhonnP";
const char *WIFI_PASS = "alberto1234";

const char *MQTT_BROKER = "abkb9nclg5hh-ats.iot.us-east-1.amazonaws.com";
const int   MQTT_PORT = 8883;
const char *CLIENT_ID = "GREENHOUSE_ESP32";

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

const int LDR_PIN = 34;
const int LDR_THRESHOLD =  500;

const int DHT11_PIN = 4;

const int BLIND_PIN = 22;
const int BLIND_ACTIVE_POSITION = 120;
const int BLIND_INACTIVE_POSITION = 0;

const int IRRIGATION_PIN = 26;
const int IRRIGATION_ACTIVE_POSITION_PIN = 60;
const int IRRIGATION_INACTIVE_POSITION = 120;

const int INVALID_STATE = -1;
#endif
