#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <WiFi.h>

class WifiManager
{
private:
  const char *ssid;
  const char *password;

public:
  WifiManager(const char *_ssid, const char *_password) : ssid(_ssid), password(_password) {}

  void connect()
  {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) delay(500);
  }

  bool isConnected()
  {
    return WiFi.status() == WL_CONNECTED;
  }
};

#endif
