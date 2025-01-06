#include <TFT_eSPI.h> // Include the TFT library
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

TFT_eSPI tft = TFT_eSPI();      // Create TFT instance
TFT_eSprite sprite = TFT_eSprite(&tft); // Create a sprite instance

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // Update every minute

// Define colors
#define BACKGROUND_COLOR TFT_BLACK
#define WIDGET_COLOR 0x39C4 // Anthracite
#define TEXT_COLOR TFT_WHITE
#define ACCENT_COLOR TFT_CYAN

const char* ssid = "Vucjak";
const char* password = "2102971340015";


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
void drawClockAndWiFi();
void updateUI();
void drawUI();
void connectToWiFi();

void setup() {
    pinMode(18, OUTPUT);
    analogWrite(18, 50);
    //connectToWiFi();
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

    delay(500); // Refresh every 2 seconds
}

void drawUI() {
    // Start drawing into the sprite
    sprite.fillSprite(BACKGROUND_COLOR);
    sprite.fillRect(0,0,480,35, TFT_DARKGREY);

    drawClockAndWiFi();

    sprite.setTextDatum(TL_DATUM); // Top-left
    sprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
    sprite.setTextSize(3);
    sprite.drawString("SSID:", 20, 60); sprite.drawString(ssid, 120, 60);
    sprite.drawString("PASS:", 20, 120); sprite.drawString(password, 120, 120);

    sprite.pushSprite(0, 0);

    delay(5000);

    sprite.fillSprite(BACKGROUND_COLOR);
    sprite.fillRect(0,0,480,35, TFT_DARKGREY);

    drawClockAndWiFi();

    sprite.pushSprite(0, 0);

}

void drawClockAndWiFi() {
    // Draw clock
    String currentTime = timeClient.getFormattedTime();
    sprite.setTextDatum(TC_DATUM); // Top-left
    sprite.setTextColor(TEXT_COLOR, TFT_DARKGREY);
    sprite.setTextSize(3);
    sprite.drawString(currentTime, 240, 8);

    // Draw WiFi symbol
    if (WiFi.status() == WL_CONNECTED) {
        sprite.fillCircle(455, 18, 7, TFT_GREEN); // Indicate WiFi connection (green dot)
    } else {
        sprite.fillCircle(455, 18, 7, TFT_RED); // Indicate NO WiFi connection (red dot)
    }
}
