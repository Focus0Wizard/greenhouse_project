#ifndef CONFIG_H
#define CONFIG_H

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

#endif
