#include <Arduino.h>
#include <Wire.h>

#include <TFT_eSPI.h>
#include <Dps3xx.h>
#include <SensirionI2CScd4x.h>
#include <SensirionI2CSen5x.h>
#include <SensirionI2cSht4x.h>

#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// Custom code files
#include "secrets.h" // Secret credentials
#include "config.h"  // General configuration
#include "server_functions.h"
#include "lcd_functions.h"

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

uint64_t timestamp = 0;

// Creating sensor instances
Dps3xx dps368 = Dps3xx();
SensirionI2CScd4x scd4x = SensirionI2CScd4x();
SensirionI2CSen5x sen5x = SensirionI2CSen5x();
SensirionI2cSht4x sht4x = SensirionI2cSht4x();
// Creating display instances
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);
// Creating NTP instances
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3601, 60000); // Update every minute, UTC+1 timezone
String ipString;

// Function declarations
// Display functions
void drawWidget(int x, int y, int w, int h, const char* label, const char* value, uint16_t color);
void drawClockAndWiFi();
void updateUI();
void drawUI();

// Sensor functions
void printUint16Hex(uint16_t value);
void printSCD4xSerialNumber();
void printSEN5xSerialNumber();
void printSEN5xModuleVersions();
void printSHT4xSerialNumber();


// Setup function - executes once on power up
void setup() {
    pinMode(BACKLIGHT_PIN, OUTPUT);
    analogWrite(BACKLIGHT_PIN, BRIGHTNESS_VALUE);

    // Initialize serial communication
    Serial.begin(BAUD_RATE);
    while (!Serial) {
        delay(100);
    }
    Serial.println("Booting...");

    connectToWifi();
    IPAddress ip = WiFi.localIP();
    ipString = ip.toString();

    timeClient.begin();

    // Initialize the display in landscape mode
    tft.init();
    tft.setRotation(3);
    tft.fillScreen(BACKGROUND_COLOR);

    // Create a sprite for the display
    sprite.createSprite(480, 320);

    // Draw UI layout
    drawUI();

    // Initialize I2C bus
    Wire.begin(SDA_PIN, SCL_PIN);
    scd4x.begin(Wire);
    sen5x.begin(Wire);
    dps368.begin(Wire, DPS368_ADDRESS);
    sht4x.begin(Wire, SHT4X_ADDRESS);

    delay(100);

    // SCD4x Initialization
    // Stop SCD4x potentially previously started measurement
    uint16_t scd4xError;
    char scd4xErrorMessage[256];
    scd4xError = scd4x.stopPeriodicMeasurement();
    if (scd4xError) {
        Serial.print("Error trying to execute scd4x.stopPeriodicMeasurement(): ");
        errorToString(scd4xError, scd4xErrorMessage, 256);
        Serial.println(scd4xErrorMessage);
    }

    // SEN5x Initialization
    // Reset SEN5x 
    uint16_t sen5xError;
    char sen5xErrorMessage[256];
    sen5xError = sen5x.deviceReset();
    if (sen5xError != 0 && DEBUG_SERIAL == 1) {
        Serial.print("Error trying to execute sen5x.deviceReset(): ");
        errorToString(sen5xError, sen5xErrorMessage, 256);
        Serial.println(sen5xErrorMessage);
    }

    // SHT4x Initialization
    // Reset SHT4x 
    uint16_t sht4xError;
    char sht4xErrorMessage[64];
    sht4xError = sht4x.softReset();
    if (sht4xError != 0 && DEBUG_SERIAL == 1) {
        Serial.print("Error trying to execute sht4x.softReset(): ");
        errorToString(sht4xError, sht4xErrorMessage, 64);
        Serial.println(sht4xErrorMessage);
    }

    // Print sensor module information if I2C buffers are set 
    #ifdef USE_PRODUCT_INFO
        if (DEBUG_SERIAL == 1) {
        printSHT4xSerialNumber();
        printSCD4xSerialNumber();
        printSEN5xModuleVersions();
        // Add ambient pressure sensor
        }
    #endif

    #ifdef SEN5X_TEMP_OFFSET
        // Set temperature offset for SEN5x
        sen5xError = sen5x.setTemperatureOffsetSimple(SEN5X_TEMP_OFFSET);
        if (sen5xError != 0 && DEBUG_SERIAL == 1) {
            Serial.print("Error trying to execute sen5x.setTemperatureOffsetSimple(): ");
            errorToString(sen5xError, sen5xErrorMessage, 256);
            Serial.println(sen5xErrorMessage);
        } 
        else if (sen5xError == 0 && DEBUG_SERIAL == 1) {
            Serial.print("SEN5x Temperature Offset set to ");
            Serial.print(SEN5X_TEMP_OFFSET);
            Serial.println(" °C (SEN54/SEN55 only)");
        }
    #endif

    // Start SCD4x measurement
    scd4xError = scd4x.startPeriodicMeasurement();
    if (scd4xError != 0 && DEBUG_SERIAL == 1) {
        Serial.print("Error trying to execute scd4x.startPeriodicMeasurement(): ");
        errorToString(scd4xError, scd4xErrorMessage, 256);
        Serial.println(scd4xErrorMessage);
    }

    // Start SEN5x measurement
    sen5xError = sen5x.startMeasurement();
    if (sen5xError != 0 && DEBUG_SERIAL == 1) {
        Serial.print("Error trying to execute sen5x.startMeasurement(): ");
        errorToString(sen5xError, sen5xErrorMessage, 256);
        Serial.println(sen5xErrorMessage);
    }

    // add external sensor initializations here

    if (DEBUG_SERIAL == 1) {
        Serial.println("Booting process completed ");
        Serial.println("Waiting for first measurement... (5 sec)");
    }

}

// Main loop function - executes in loop after setup function is executed
void loop() {
    timeClient.update();
    // Refresh UI
    updateUI();

    // SCD4x
    // Check if SCD4x is ready to send data
    bool isDataReady = false;
    uint16_t scd4xError;
    char scd4xErrorMessage[256];
    scd4xError = scd4x.getDataReadyFlag(isDataReady);
    if (scd4xError != 0 && DEBUG_SERIAL == 1) {
        Serial.print("Error trying to execute scd4x.getDataReadyFlag(): ");
        errorToString(scd4xError, scd4xErrorMessage, 256);
        Serial.println(scd4xErrorMessage);
        return;
    }
    if (!isDataReady) {
        return;
    }

    // Read and serial print SCD4x measurements

    if (DEBUG_SERIAL == 1) {
        Serial.println("-----------------------");
        Serial.println("SCD4x Measurements:");
    }
    scd4xError = scd4x.readMeasurement(scd4xCo2, scd4xTemperature, scd4xHumidity);
    if (scd4xError != 0 && DEBUG_SERIAL == 1) {
        Serial.print("Error trying to execute scd4x.readMeasurement(): ");
        errorToString(scd4xError, scd4xErrorMessage, 256);
        Serial.println(scd4xErrorMessage);
    } else if (scd4xCo2 == 0 && DEBUG_SERIAL == 1) {
        Serial.println("SCD4x Invalid sample detected, skipping.");
    } else if (scd4xError == 0 && DEBUG_SERIAL == 1){
        Serial.print("CO2: "); Serial.print(scd4xCo2); Serial.println(" ppm");
        Serial.print("Temperature: "); Serial.print(scd4xTemperature); Serial.println(" °C");
        Serial.print("Humidity: "); Serial.print(scd4xHumidity); Serial.println(" %");
    }

    // SEN5x
    // Read and serial print SEN5x measurements
    if (DEBUG_SERIAL == 1) {
        Serial.println("-----------------------");
        Serial.println("SEN5x Measurements:");
    }
    uint16_t sen5xError;
    char sen5xErrorMessage[256];

    sen5xError = sen5x.readMeasuredValues(
        sen5xPm1,
        sen5xPm2_5,
        sen5xPm4,
        sen5xPm10,
        sen5xHumidity, 
        sen5xTemperature, 
        sen5xVocIndex,
        sen5xNoxIndex);

    if (sen5xError != 0 && DEBUG_SERIAL == 1) {
        Serial.print("Error trying to execute sen5x.readMeasuredValues(): ");
        errorToString(sen5xError, sen5xErrorMessage, 256);
        Serial.println(sen5xErrorMessage);
    } else if (sen5xError == 0 && DEBUG_SERIAL == 1) {
        Serial.print("PM 1.0: "); Serial.print(sen5xPm1);   Serial.println(" µg/m³");
        Serial.print("PM 2.5: "); Serial.print(sen5xPm2_5); Serial.println(" µg/m³");
        Serial.print("PM 4.0: "); Serial.print(sen5xPm4);   Serial.println(" µg/m³");
        Serial.print("PM 10: ");  Serial.print(sen5xPm10);  Serial.println(" µg/m³");

        Serial.print("Temperature: ");
        if (isnan(sen5xTemperature)) {
            Serial.println("n/a");
        } else {
            Serial.print(sen5xTemperature); Serial.println(" °C");
        }

        Serial.print("Humidity: ");
        if (isnan(sen5xHumidity)) {
            Serial.println("n/a");
        } else {
            Serial.print(sen5xHumidity); Serial.println(" %");
        }

        Serial.print("VocIndex: ");
        if (isnan(sen5xVocIndex)) {
            Serial.println("n/a");
        } else {
            Serial.println(sen5xVocIndex);
        }

        Serial.print("NoxIndex: ");
        if (isnan(sen5xNoxIndex)) {
            Serial.println("n/a");
        } else {
            Serial.println(sen5xNoxIndex);
        }
    }

    // DPS368
    // Read and serial print DPS368 measurements
    if (DEBUG_SERIAL == 1) {
        Serial.println("-----------------------");
        Serial.println("DPS368 Measurements:");
    }

    int16_t dps368Error;
    dps368Error = dps368.measureTempOnce(dps368Temperature, DPS368_OVERSAMPLING);
    if (dps368Error != 0 && DEBUG_SERIAL == 1) {
        Serial.print("Error trying to execute dps368.measureTempOnce(): "); 
        Serial.println(dps368Error);
    } else if (dps368Error == 0 && DEBUG_SERIAL == 1) {
        Serial.print("Temperature: "); Serial.print(dps368Temperature); Serial.println(" °C");
    }
    
    dps368Error = dps368.measurePressureOnce(dps368Pressure, DPS368_OVERSAMPLING);
    if (dps368Error != 0 && DEBUG_SERIAL == 1) {
        Serial.print("Error trying to execute dps368.measurePressureOnce(): ");
        Serial.println(dps368Error);
    } else if (dps368Error == 0 && DEBUG_SERIAL == 1) {
        Serial.print("Pressure: "); Serial.print(dps368Pressure); Serial.println(" Pa"); 
    }

    // SHT4x
    // Read and serial print SHT4x measurements
    if (DEBUG_SERIAL == 1) {
        Serial.println("-----------------------");
        Serial.println("SHT4x Measurements:");
    }
    int16_t sht4xError;
    char sht4xErrorMessage[64];
    sht4xError = sht4x.measureHighPrecision(sht4xTemperature, sht4xHumidity);
    if (sht4xError != 0 && DEBUG_SERIAL == 1) {
        Serial.print("Error trying to execute sht4x.measureHighPrecision(): ");
        errorToString(sht4xError, sht4xErrorMessage, 64);
        Serial.println(sht4xErrorMessage);
    } else if (sht4xError == 0 && DEBUG_SERIAL == 1) {
        Serial.print("Temperature: "); Serial.print(sht4xTemperature); Serial.println(" °C");
        Serial.print("Humidity: "); Serial.print(sht4xHumidity); Serial.println(" %");
    } else if (sht4xError == 0) {
        
    }

}

// Draw initial UI with Wi-Fi credentials
void drawUI() {
    sprite.fillSprite(BACKGROUND_COLOR);

    drawClockAndWiFi();

    sprite.setTextDatum(TL_DATUM); // Top-left
    sprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
    sprite.setTextSize(3);
    sprite.drawString("SSID:", 20, 60);  sprite.drawString(SECRET_SSID, 120, 60);
    sprite.drawString("PASS:", 20, 120); sprite.drawString(SECRET_PASSWORD, 120, 120);
    sprite.drawString("IP:", 20, 180); sprite.drawString(ipString, 120, 180);

    sprite.pushSprite(0, 0);

    delay(3000);

    sprite.fillRect(0,35,480,285, BACKGROUND_COLOR);

    sprite.pushSprite(0, 0);

}

// Draw clock and Wi-Fi indicator
void drawClockAndWiFi() {
    // Draw clock
    unsigned long currentEpoch = timeClient.getEpochTime();
    String currentTime = epochToHoursMinutes(currentEpoch);

    sprite.fillRect(0,0,480,35, TOP_BAR_COLOR);
    sprite.setTextDatum(TC_DATUM); // Top-center
    sprite.setTextColor(TEXT_COLOR, TOP_BAR_COLOR);
    sprite.setTextSize(3);
    sprite.drawString(currentTime, 240, 8);

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
    uint8_t y = 55;
    uint8_t w = 110;
    uint8_t h = 110;
    uint8_t v_space = 25;

    drawWidget(x +   0, y, w, h, "Temp",     "C",   String(TEMPERATURE, 1).c_str(),  getTempColor(TEMPERATURE));
    drawWidget(x + 120, y, w, h, "RH",       "%",   String(HUMIDITY, 1).c_str(),     getHumidityColor(HUMIDITY));
    drawWidget(x + 240, y, w, h, "CO2",      "ppm", String(CO2).c_str(),             getCO2Color(CO2));
    drawWidget(x + 360, y, w, h, "Pressure", "hPa", String(PRESSURE/100, 2).c_str(), getPressureColor(PRESSURE));

    drawWidget(x +   0, y + h + v_space, w, h, "VOC",   "Index", String(VOC_INDEX, 0).c_str(), getVOCColor(VOC_INDEX));
    drawWidget(x + 120, y + h + v_space, w, h, "PM1",   "ug/m3", String(PM1, 0).c_str(),       getPM1Color(PM1));
    drawWidget(x + 240, y + h + v_space, w, h, "PM2.5", "ug/m3", String(PM2_5, 0).c_str(),     getPM2_5Color(PM2_5));
    drawWidget(x + 360, y + h + v_space, w, h, "PM10",  "ug/m3", String(PM10, 0).c_str(),      getPM10Color(PM10));

    drawClockAndWiFi();

    sprite.pushSprite(0, 0);
}



// Sensor function definition
// Serial print a 16-bit unsigned integer in 4 digit HEX format by adding leading zeros
void printUint16Hex(uint16_t value) {
    Serial.print(value < 4096 ? "0" : "");
    Serial.print(value < 256 ? "0" : "");
    Serial.print(value < 16 ? "0" : "");
    Serial.print(value, HEX);
}

// Serial print the SCD4x serial number using the printUint16Hex number formater function
void printSCD4xSerialNumber() {
    uint16_t serial0;
    uint16_t serial1;
    uint16_t serial2;

    uint16_t error;
    char errorMessage[256];
    error = scd4x.getSerialNumber(serial0, serial1, serial2);
    if (error) {
        Serial.print("Error trying to execute scd4x.getSerialNumber(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.print("SCD4x Serial Number: 0x");
        printUint16Hex(serial0);
        printUint16Hex(serial1);
        printUint16Hex(serial2);
        Serial.println();
    }
    
}

// Serial print the SEN5x serial number
void printSEN5xSerialNumber() {
    unsigned char serialNumber[32];
    uint8_t serialNumberSize = 32;

    uint16_t error;
    char errorMessage[256];
    error = sen5x.getSerialNumber(serialNumber, serialNumberSize);
    if (error) {
        Serial.print("Error trying to execute sen5x.getSerialNumber(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.print(" Serial Number: ");
        Serial.println((char*)serialNumber);
    }
}

// Serial print the SEN5x serial number, firmware and hardware version
void printSEN5xModuleVersions() {
    unsigned char productName[32];
    uint8_t productNameSize = 32;

    uint16_t error;
    char errorMessage[256];
    error = sen5x.getProductName(productName, productNameSize);
    if (error) {
        Serial.print("Error trying to execute sen5x.getProductName(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        //Serial.print("ProductName:");
        Serial.print((char*)productName);
        printSEN5xSerialNumber();
    }

    bool firmwareDebug;
    uint8_t firmwareMajor;
    uint8_t firmwareMinor;
    uint8_t hardwareMajor;
    uint8_t hardwareMinor;
    uint8_t protocolMajor;
    uint8_t protocolMinor;

    error = sen5x.getVersion(firmwareMajor, firmwareMinor, firmwareDebug,
                             hardwareMajor, hardwareMinor, protocolMajor,
                             protocolMinor);
    if (error) {
        Serial.print("Error trying to execute sen5x.getVersion(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.print("Firmware: ");
        Serial.print(firmwareMajor);
        Serial.print(".");
        Serial.print(firmwareMinor);
        Serial.print(", ");

        Serial.print("Hardware: ");
        Serial.print(hardwareMajor);
        Serial.print(".");
        Serial.println(hardwareMinor);
    }
}

// Serial print the SHT4x serial number
void printSHT4xSerialNumber() {
    uint32_t serialNumber = 0;

    int16_t error;
    char errorMessage[64];
    error = sht4x.serialNumber(serialNumber);
    if (error) {
        Serial.print("Error trying to execute sht4x.serialNumber(): ");
        errorToString(error, errorMessage, 64);
        Serial.println(errorMessage);
    }
    Serial.print("SHT4x Serial Number: "); Serial.println(serialNumber);
}
