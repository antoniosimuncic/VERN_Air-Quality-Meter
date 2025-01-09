#include <Arduino.h>
#include <Wire.h>
#include <TFT_eSPI.h>
#include <Dps3xx.h>
#include <SensirionI2CScd4x.h>
#include <SensirionI2CSen5x.h>
#include <SensirionI2cSht4x.h>

#include "config.h"  // General configuration
#include "secrets.h" // Secret credentials
#include "server_functions.h"

// Display variables - Define which values to display
#define TEMPERATURE sen5xTemperature
#define HUMIDITY sen5xHumidity
#define CO2 scd4xCo2
#define PRESSURE dps368Pressure
#define VOC_INDEX sen5xVocIndex
#define PM1 sen5xPm1
#define PM2_5 sen5xPm2_5
#define PM10 sen5xPm10

// Variables for sensor measured values
// SEN5x
float sen5xPm1   = 0.0f;
float sen5xPm2_5 = 0.0f;
float sen5xPm4   = 0.0f;
float sen5xPm10  = 0.0f;
float sen5xHumidity = 0.0f;
float sen5xTemperature = 0.0f;
float sen5xVocIndex = 0.0f;
float sen5xNoxIndex = 0.0f;
// SCD4x
uint16_t scd4xCo2 = 0;
float scd4xTemperature = 0.0f;
float scd4xHumidity = 0.0f;
// DPS368
float dps368Pressure = 0.0f;
float dps368Temperature = 0.0f;
// SHT40 - external sensor
float sht4xTemperature = 0.0f;
float sht4xHumidity = 0.0f;


// Creating display instances
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);


// Function declarations
// Display functions
void drawWidget(int x, int y, int w, int h, const char* label, const char* value, uint16_t color);
void drawClockAndWiFi();
void updateUI();
void drawUI();




// Setup function - executes once on power up
void setup() {

    // Initialize serial communication
    Serial.begin(BAUD_RATE);
    while (!Serial) {
        delay(100);
    }
    Serial.println("Booting...");

    Serial.println(xPortGetCoreID());

    pinMode(BACKLIGHT_PIN, OUTPUT);
    analogWrite(BACKLIGHT_PIN, BRIGHTNESS_VALUE);

    // Initialize the display in landscape mode
    tft.init();
    tft.setRotation(3);
    tft.fillScreen(BACKGROUND_COLOR);

    // Create a sprite for the display
    sprite.createSprite(480, 320);

    // Draw UI layout
    drawUI();



}


// Main loop function - executes in loop after setup function is executed
void loop() {
    // Refresh UI
    updateUI();
    Serial.println("hello");
    connectToWifi();
    Serial.println(xPortGetCoreID());
    delay(500);
}

// Draw initial UI with Wi-Fi credentials
void drawUI() {
    sprite.fillSprite(BACKGROUND_COLOR);
    sprite.fillRect(0,0,480,35, TOP_BAR_COLOR);
 
    sprite.setTextDatum(TL_DATUM); // Top-left
    sprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
    sprite.setTextSize(3);
    sprite.drawString("SSID:", 20, 60); sprite.drawString(SECRET_SSID, 120, 60);
    sprite.drawString("PASS:", 20, 120); sprite.drawString(SECRET_PASSWORD, 120, 120);

    sprite.pushSprite(0, 0);

    delay(1000);

    sprite.fillRect(0,35,480,285, BACKGROUND_COLOR);

    drawClockAndWiFi();

    sprite.pushSprite(0, 0);

}

// Draw clock and Wi-Fi indicator
void drawClockAndWiFi() {
    // Draw clock
    sprite.setTextDatum(TC_DATUM); // Top-center
    sprite.setTextColor(TEXT_COLOR, TOP_BAR_COLOR);
    sprite.setTextSize(3);
    sprite.drawString("18:32", 240, 8);

    // Draw WiFi symbol and indication - TODO
    if (WiFi.status() == WL_CONNECTED) {
        sprite.fillCircle(455, 18, 7, TFT_GREEN); // Indicate WiFi connection (green dot)
    } else {
        sprite.fillCircle(455, 18, 7, TFT_RED); // Indicate NO WiFi connection (red dot)
    }
}

// Draw widgets with specified values
void drawWidget(int x, int y, int w, int h, const char* label, const char* unit, const char* value, uint16_t color) {
    // Draw the rounded rectangle
    sprite.fillRoundRect(x, y, w, h, 10, color);
    sprite.fillRoundRect(x + 5, y + 5, w - 10, h - 10, 5, BACKGROUND_COLOR);

    // Draw the label
    sprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
    sprite.setTextDatum(TC_DATUM);
    sprite.setTextSize(2);
    sprite.drawString(label, x + w / 2, y + 12);

    // Draw the value
    sprite.setTextSize(2);
    sprite.setTextColor(MEASUREMENT_TEXT_COLOR, BACKGROUND_COLOR);
    sprite.drawString(value, x + w / 2, (y + h / 2)-10);

    // Draw the measurement unit
    sprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
    sprite.setTextDatum(TC_DATUM);
    sprite.setTextSize(2);
    sprite.drawString(unit, x + w / 2, y + 80);
}

// Update each widget with new sensor values
void updateUI() {
    uint8_t x = 3;
    uint8_t y = 60;
    uint8_t w = 110;
    uint8_t h = 110;
    uint8_t v_space = 25;

    drawWidget(x +   0, y, w, h, "Temp",     "C",   String(TEMPERATURE, 1).c_str(),  GOOD_COLOR);
    drawWidget(x + 120, y, w, h, "RH",       "%",   String(HUMIDITY, 1).c_str(),     MEDIOCRE_COLOR);
    drawWidget(x + 240, y, w, h, "CO2",      "ppm", String(CO2).c_str(),             BAD_COLOR);
    drawWidget(x + 360, y, w, h, "Pressure", "hPa", String(PRESSURE/100, 2).c_str(), TERRIBLE_COLOR);

    drawWidget(x +   0, y + h + v_space, w, h, "VOC",   "Index", String(VOC_INDEX, 0).c_str(), TERRIBLE_COLOR);
    drawWidget(x + 120, y + h + v_space, w, h, "PM1",   "ug/m3", String(PM1, 0).c_str(),       BAD_COLOR);
    drawWidget(x + 240, y + h + v_space, w, h, "PM2.5", "ug/m3", String(PM2_5, 0).c_str(),     MEDIOCRE_COLOR);
    drawWidget(x + 360, y + h + v_space, w, h, "PM10",  "ug/m3", String(PM10, 0).c_str(),      GOOD_COLOR);

    sprite.pushSprite(0, 0);
}

/*************************************************************************************/

#include <WiFi.h>
#include <HTTPClient.h>

// Wi-Fi credentials
const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

// Server endpoint
const char* serverUrl = "https://iot.simuncic.com/sensor-readings";

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  // Prepare JSON data
  String jsonData = R"({
    "mac_address": "00:A:7D:EA:91:89",
    "temperature": 22.5,
    "humidity": 45.0,
    "pm1": 10,
    "pm2_5": 20,
    "pm4": 25,
    "pm10": 30,
    "co2": 450,
    "voc": 100,
    "pressure": 1013.25
  })";

  // Send POST request
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Begin the HTTP request
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    // Send POST request
    int httpResponseCode = http.POST(jsonData);

    // Check the HTTP response
    if (httpResponseCode > 0) {
      String response = http.getString(); // Get the response payload
      Serial.println("Response code: " + String(httpResponseCode));
      Serial.println("Response: " + response);
    } else {
      Serial.println("Error on sending POST: " + String(httpResponseCode));
    }

    // End HTTP connection
    http.end();
  } else {
    Serial.println("Wi-Fi not connected");
  }
}

void loop() {
  // Nothing to do in loop
}


String macAddress = "00:A:7D:EA:91:89";
float temperature = 22.5;
float humidity = 45.0;

String jsonData = "{";
jsonData += "\"mac_address\":\"" + macAddress + "\",";
jsonData += "\"temperature\":" + String(temperature) + ",";
jsonData += "\"humidity\":" + String(humidity);
jsonData += "}";






/******************************************************/
#include <ArduinoJson.h>

// Create a JSON object
StaticJsonDocument<200> jsonDoc;

void setup() {
  Serial.begin(115200);

  // Add key-value pairs
  jsonDoc["mac_address"] = "00:A:7D:EA:91:89";
  jsonDoc["temperature"] = 22.5;
  jsonDoc["humidity"] = 45.0;

  // Serialize JSON to string
  String jsonData;
  serializeJson(jsonDoc, jsonData);

  // Print JSON string
  Serial.println(jsonData);

  // Now you can send `jsonData` with the HTTP POST request
}

void loop() {
  // Do nothing
}
