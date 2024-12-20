#include "server_functions.h"
#include "config.h"
#include "secrets.h"

// Wi-Fi functions
void connectToWifi() {
    uint8_t attempt = 0;
    while (attempt < WIFI_RETRIES) {
        Serial.printf("Attempt %u to connect to Wi-Fi...\n", attempt + 1);
        WiFi.mode(WIFI_STA);
        WiFi.begin(SECRET_SSID, SECRET_PASSWORD);
        unsigned long startAttemptTime = millis();
        Serial.println(xPortGetCoreID());
        // Try to connect without blocking
        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT) {
            delay(10);
            yield();
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("Connected to Wi-Fi!");
            Serial.print("IP Address: ");
            Serial.println(WiFi.localIP());
            return;
        }

        Serial.println("Failed to connect. Retrying...");
        delay(2000); // Wait before retrying
        attempt++;
    }

    Serial.println("Exceeded maximum retry attempts. Check your Wi-Fi credentials or network.");
}


void checkWifi(char* ssid, char* password) {
    delay(10);
}


