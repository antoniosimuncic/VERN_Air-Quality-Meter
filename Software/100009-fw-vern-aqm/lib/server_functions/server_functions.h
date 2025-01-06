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


#endif