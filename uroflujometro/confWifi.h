#ifndef CONFWIFI_H
#define CONFWIFI_H

#include <WiFi.h>
#include <WiFiUdp.h>

extern const char* ssid;
extern const char* password;
extern WiFiUDP udp;
extern const char* ip_remota;
extern const int puerto_remoto;

void iniciarWifi(const char* ssid, const char* password);

#endif