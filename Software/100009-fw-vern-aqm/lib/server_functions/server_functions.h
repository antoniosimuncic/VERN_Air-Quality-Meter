#ifndef _SERVER_FUNCTIONS_
#define _SERVER_FUNCTIONS_

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

String lineProtocolFormatter(const char* measurement, const char* tags, const char* fields = "", uint64_t timestamp = 0);

// Wi-Fi functions
void connectToWifi();
void checkWifi();
String epochToHoursMinutes(unsigned long epochTime);

void sendDataToHTTPServer(String mac_address, float temperature, float humidity, float pm1, float pm2_5, float pm4, float pm10, uint16_t co2, float voc, float pressure);

#endif