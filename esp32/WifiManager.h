#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <WiFi.h>

class WifiManager
{
private:
  const char *ssid;
  const char *password;

public:
  WifiManager(const char *ssid, const char *password) 
  : ssid(ssid), 
  password(password) {}

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
