/*
#include <TFT_eSPI.h> // Include the TFT library
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();      // Create TFT instance
TFT_eSprite sprite = TFT_eSprite(&tft); // Create a sprite instance

// Define colors
#define BACKGROUND_COLOR TFT_BLACK
#define WIDGET_COLOR 0x39C4 // Anthracite
#define TEXT_COLOR TFT_WHITE
#define ACCENT_COLOR TFT_CYAN

// Mock sensor values
float temperature = 23.5;
float humidity = 45.2;
int co2 = 420;
float pressure = 1013.25;
int vocIndex = 125;
float pm1 = 12.3;
float pm25 = 25.7;
float pm10 = 35.6;

// Function prototypes
void drawWidget(int x, int y, int w, int h, const char* label, const char* value, uint16_t color);
void updateSensorData();
void updateUI();
void drawUI();

void setup() {
    pinMode(18, OUTPUT);
    analogWrite(18, 150);

    tft.init();
    tft.setRotation(3); // Landscape mode
    tft.fillScreen(BACKGROUND_COLOR);

    // Create a sprite to match the screen size
    sprite.createSprite(480, 320);

    // Draw UI layout
    drawUI();
}

void loop() {
    // Update mock data
    updateSensorData();

    // Refresh UI
    updateUI();

    delay(2000); // Refresh every 2 seconds
}

void drawUI() {
    // Start drawing into the sprite
    sprite.fillSprite(BACKGROUND_COLOR);
    sprite.fillRect(0,0,480,35, ACCENT_COLOR);
    // Header
    sprite.setTextColor(TEXT_COLOR, ACCENT_COLOR);
    sprite.setTextDatum(TC_DATUM);
    sprite.setTextSize(2);
    sprite.drawString("Air Quality Monitor", 240, 10);

    // Row 1
    drawWidget(10, 50, 110, 110, "Temp", "23.5 C", ACCENT_COLOR);
    drawWidget(130, 50, 110, 110, "Hum", "45.2 %", WIDGET_COLOR);
    drawWidget(250, 50, 110, 110, "CO2", "420 ppm", TFT_DARKGREEN);
    drawWidget(370, 50, 110, 110, "Pres", "1013 hPa", TFT_GREEN);

    // Row 2
    drawWidget(10, 180, 110, 110, "VOC", "125", TFT_GREENYELLOW);
    drawWidget(130, 180, 110, 110, "PM1", "12.3", TFT_ORANGE);
    drawWidget(250, 180, 110, 110, "PM2.5", "25.7", TFT_RED);
    drawWidget(370, 180, 110, 110, "PM10", "35.6", TFT_MAROON);

    // Push the sprite to the screen
    sprite.pushSprite(0, 0);
}

void drawWidget(int x, int y, int w, int h, const char* label, const char* value, uint16_t color) {
    // Draw the rounded rectangle
    sprite.fillRoundRect(x, y, w, h, 10, color);
    sprite.fillRoundRect(x + 5, y + 5, w - 10, h - 10, 10, BACKGROUND_COLOR);

    // Draw the label
    sprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
    sprite.setTextDatum(TC_DATUM);
    sprite.setTextSize(2);
    sprite.drawString(label, x + w / 2, y + 10);

    // Draw the value
    sprite.setTextSize(2);
    sprite.setTextColor(ACCENT_COLOR, BACKGROUND_COLOR);
    sprite.drawString(value, x + w / 2, y + h / 2);
}

void updateUI() {
    // Update each widget with new sensor values
    drawWidget(10, 50, 110, 110, "Temperature", String(temperature, 1).c_str(), ACCENT_COLOR);
    drawWidget(130, 50, 110, 110, "Humidity", String(humidity, 1).c_str(), BACKGROUND_COLOR);
    drawWidget(250, 50, 110, 110, "CO2", String(co2).c_str(), WIDGET_COLOR);
    drawWidget(370, 50, 110, 110, "Pressure", String(pressure, 2).c_str(), ACCENT_COLOR);

    drawWidget(10, 180, 110, 110, "VOC Index", String(vocIndex).c_str(), WIDGET_COLOR);
    drawWidget(130, 180, 110, 110, "PM1", String(pm1, 1).c_str(), WIDGET_COLOR);
    drawWidget(250, 180, 110, 110, "PM2.5", String(pm25, 1).c_str(), WIDGET_COLOR);
    drawWidget(370, 180, 110, 110, "PM10", String(pm10, 1).c_str(), WIDGET_COLOR);

    // Push the sprite to the screen
    sprite.pushSprite(0, 0);
}

void updateSensorData() {
    // Replace these with real sensor data
    temperature = random(200, 300) / 10.0;
    humidity = random(300, 700) / 10.0;
    co2 = random(400, 600);
    pressure = 1013.25 + random(-5, 5) / 10.0;
    vocIndex = random(100, 150);
    pm1 = random(10, 20) / 1.0;
    pm25 = random(20, 40) / 1.0;
    pm10 = random(30, 50) / 1.0;
}
*/


#include <TFT_eSPI.h> // Include the TFT library
#include <SPI.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// WiFi credentials
const char* ssid = "ALHN-2B78";
const char* password = "DPgWqUGMw7";

// NTP Client setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // Update every minute

TFT_eSPI tft = TFT_eSPI();      // Create TFT instance
TFT_eSprite sprite = TFT_eSprite(&tft); // Create a sprite instance

// Define colors
#define BACKGROUND_COLOR TFT_BLACK
#define WIDGET_COLOR 0x39C4 // Anthracite
#define TEXT_COLOR TFT_WHITE
#define ACCENT_COLOR TFT_CYAN

// Mock sensor values
float temperature = 23.5;
float humidity = 45.2;
int co2 = 420;
float pressure = 1013.25;
int vocIndex = 125;
float pm1 = 12.3;
float pm25 = 25.7;
float pm10 = 35.6;

// Function prototypes
void drawWidget(int x, int y, int w, int h, const char* label, const char* value, uint16_t color);
void updateSensorData();
void updateUI();
void drawUI();
void drawClockAndWiFi();
void connectToWiFi();

void setup() {
    pinMode(18, OUTPUT);
    analogWrite(18, 200);

    tft.init();
    tft.setRotation(3); // Landscape mode
    tft.fillScreen(BACKGROUND_COLOR);

    // Create a sprite to match the screen size
    sprite.createSprite(480, 320);

    connectToWiFi();
    //timeClient.begin();

    // Draw UI layout
    drawUI();
}

void loop() {
    // Update mock data
    updateSensorData();

    // Sync time
    //timeClient.update();

    // Refresh UI
    updateUI();

    delay(2000); // Refresh every 2 seconds
}

void drawUI() {
    // Start drawing into the sprite
    sprite.fillSprite(BACKGROUND_COLOR);

    // Clock and WiFi status
    drawClockAndWiFi();

    // Row 1
    drawWidget(10, 50, 110, 110, "Temp", "23.5 C", ACCENT_COLOR);
    drawWidget(130, 50, 110, 110, "Hum", "45.2 %", WIDGET_COLOR);
    drawWidget(250, 50, 110, 110, "CO2", "420 ppm", WIDGET_COLOR);
    drawWidget(370, 50, 110, 110, "Pres", "1013 hPa", ACCENT_COLOR);

    // Row 2
    drawWidget(10, 180, 110, 110, "VOC", "125", WIDGET_COLOR);
    drawWidget(130, 180, 110, 110, "PM1", "12.3", WIDGET_COLOR);
    drawWidget(250, 180, 110, 110, "PM2.5", "25.7", WIDGET_COLOR);
    drawWidget(370, 180, 110, 110, "PM10", "35.6", WIDGET_COLOR);

    // Push the sprite to the screen
    sprite.pushSprite(0, 0);
}

void drawClockAndWiFi() {
    // Draw clock
    String currentTime = timeClient.getFormattedTime();
    sprite.setTextDatum(TC_DATUM); // Top-left
    sprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
    sprite.setTextSize(2);
    sprite.drawString(currentTime, 10, 10);

    // Draw WiFi symbol
    if (WiFi.status() == WL_CONNECTED) {
        sprite.fillCircle(460, 20, 5, ACCENT_COLOR); // WiFi symbol (simple dot)
    } else {
        sprite.fillCircle(460, 20, 5, TFT_RED); // Indicate no WiFi connection
    }
}

void drawWidget(int x, int y, int w, int h, const char* label, const char* value, uint16_t color) {
    // Draw the rounded rectangle
    sprite.fillRoundRect(x, y, w, h, 10, color);
    sprite.fillRoundRect(x + 5, y + 5, w - 10, h - 10, 10, BACKGROUND_COLOR);

    // Draw the label
    sprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
    sprite.setTextDatum(TC_DATUM);
    sprite.setTextSize(2);
    sprite.drawString(label, x + w / 2, y + 10);

    // Draw the value
    sprite.setTextSize(2);
    sprite.setTextColor(ACCENT_COLOR, BACKGROUND_COLOR);
    sprite.drawString(value, x + w / 2, y + h / 2);
}

void updateUI() {
    // Redraw the UI with updated data
    drawWidget(10, 50, 110, 110, "Temperature", String(temperature, 1).c_str(), ACCENT_COLOR);
    drawWidget(130, 50, 110, 110, "Humidity", String(humidity, 1).c_str(), BACKGROUND_COLOR);
    drawWidget(250, 50, 110, 110, "CO2", String(co2).c_str(), WIDGET_COLOR);
    drawWidget(370, 50, 110, 110, "Pressure", String(pressure, 2).c_str(), ACCENT_COLOR);

    drawWidget(10, 180, 110, 110, "VOC Index", String(vocIndex).c_str(), WIDGET_COLOR);
    drawWidget(130, 180, 110, 110, "PM1", String(pm1, 1).c_str(), WIDGET_COLOR);
    drawWidget(250, 180, 110, 110, "PM2.5", String(pm25, 1).c_str(), WIDGET_COLOR);
    drawWidget(370, 180, 110, 110, "PM10", String(pm10, 1).c_str(), WIDGET_COLOR);

    // Push the sprite to the screen
    sprite.pushSprite(0, 0);
}

void updateSensorData() {
    // Replace these with real sensor data
    temperature = random(200, 300) / 10.0;
    humidity = random(300, 700) / 10.0;
    co2 = random(400, 600);
    pressure = 1013.25 + random(-5, 5) / 10.0;
    vocIndex = random(100, 150);
    pm1 = random(10, 20) / 1.0;
    pm25 = random(20, 40) / 1.0;
    pm10 = random(30, 50) / 1.0;
}

void connectToWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
}
