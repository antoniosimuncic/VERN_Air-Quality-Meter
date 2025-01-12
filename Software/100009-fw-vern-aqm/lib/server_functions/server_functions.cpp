#include "server_functions.h"
#include "config.h"
#include "secrets.h"
#include "HTTPClient.h"


/*
String lineProtocolFormatter(char* measurement, const char* tags, const char* fields = "", uint64_t timestamp) {
    // Return newline if input is bad
    if (measurement == nullptr || strlen(measurement) == 0) {
        return ""; 
    } else {
        // Append measurement if provided
        String lineProtocolMessage =  String(measurement);
        // Append tags if provided
        if (tags != nullptr && strlen(tags) > 0) {
            lineProtocolMessage += ",";
            lineProtocolMessage += tags;
        }
        // Append fields
        lineProtocolMessage += " ";
        lineProtocolMessage += fields;
        // Append timestamp if valid
        if (timestamp > 0) {
            lineProtocolMessage += " ";
            lineProtocolMessage += String(timestamp);
        }

        return lineProtocolMessage;
    }
}
*/

// Wi-Fi functions
void connectToWifi() {
    uint8_t attempt = 0;
    while (attempt < WIFI_RETRIES) {
        Serial.printf("Attempt %u to connect to Wi-Fi...\n", attempt + 1);
        WiFi.mode(WIFI_STA);
        WiFi.begin(SECRET_SSID, SECRET_PASSWORD);
        unsigned long startAttemptTime = millis();
        // Try to connect without blocking
        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT) {
            delay(10);
            yield();
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("Connected to Wi-Fi");
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

String epochToHoursMinutes(unsigned long epochTime) {
    // Calculate total seconds in a day
    const unsigned long secondsInADay = 86400;
    
    // Calculate hours and minutes
    unsigned long hours = (epochTime % secondsInADay) / 3600;
    unsigned long minutes = (epochTime % 3600) / 60;

    // Format hours and minutes into a string
    String hoursStr = (hours < 10) ? "0" + String(hours) : String(hours);
    String minutesStr = (minutes < 10) ? "0" + String(minutes) : String(minutes);
    
    return hoursStr + ":" + minutesStr; // Return formatted time
}

void sendDataToHTTPServer(String mac_address, float temperature, float humidity, float pm1, float pm2_5, float pm4, float pm10, uint16_t co2, float voc, float pressure) {
    // HTTP POST request
    // create empty JSON string
    String jsonData;
    // JSON data insert
    jsonData = "{";
    jsonData += "\"mac_address\":\"" + mac_address + "\",";
    jsonData += "\"temperature\":" + String(temperature) + ",";
    jsonData += "\"humidity\":" + String(humidity) + ",";
    jsonData += "\"pm1\":" + String(pm1) + ",";
    jsonData += "\"pm2_5\":" + String(pm2_5) + ",";
    jsonData += "\"pm4\":" + String(pm4) + ",";
    jsonData += "\"pm10\":" + String(pm10) + ",";
    jsonData += "\"co2\":" + String(co2) + ",";
    jsonData += "\"voc\":" + String(voc) + ",";
    jsonData += "\"pressure\":" + String(pressure);
    jsonData += "}";

    if (DEBUG_SERIAL == 1) {
        Serial.println("-----------------------");
        Serial.println("HTTP POST request:");
    }
    if (WiFi.status() == WL_CONNECTED) {
        // Create HTTP instance
        HTTPClient http;

        // Begin the HTTP request
        http.begin(SERVER_URL);
        http.addHeader("Content-Type", "application/json");

        // Send POST request
        int httpResponseCode = http.POST(jsonData);

        // Check the HTTP response
        if (DEBUG_SERIAL == 1) {
            Serial.println(jsonData);
            if (httpResponseCode > 0) {
                String response = http.getString(); // Get the response payload
                Serial.println("Response code: " + String(httpResponseCode));
                Serial.println("Response: " + response);
            } else {
                Serial.println("Error on sending POST: " + String(httpResponseCode));
            }
        }
        // End HTTP connection
        http.end();
    } else {
        if (DEBUG_SERIAL == 1) {
            Serial.println("Wi-Fi not connected");
        }
    }
}