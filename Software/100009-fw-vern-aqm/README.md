// working code for sensors

	sensirion/Sensirion I2C SCD4x@^0.4.0
	sensirion/Sensirion I2C SEN5X@^0.3.0
	infineon/XENSIV Digital Pressure Sensor@^1.0.0

#include <Arduino.h>
#include <Wire.h>
#include <Dps3xx.h>
#include <SensirionI2CScd4x.h>
#include <SensirionI2CSen5x.h>

#define MAXBUF_REQUIREMENT 48 // The used commands use up to 48 bytes.
#if (defined(I2C_BUFFER_LENGTH) &&                 \
     (I2C_BUFFER_LENGTH >= MAXBUF_REQUIREMENT)) || \
    (defined(BUFFER_LENGTH) && BUFFER_LENGTH >= MAXBUF_REQUIREMENT)
#define USE_PRODUCT_INFO
#endif

// configuration
#define BAUD_RATE 115200
#define SEN5X_TEMP_OFFSET 0.0
#define DPS368_OVERSAMPLING 7

// pin declaration
#define SDA_PIN  21
#define SCL_PIN  38

// variables for measured values
// SEN5x
float massConcentrationPm1p0;
float massConcentrationPm2p5;
float massConcentrationPm4p0;
float massConcentrationPm10p0;
float sen5xHumidity;
float sen5xTemperature;
float vocIndex;
float noxIndex;
// SCD4x
uint16_t co2 = 0;
float scd4xTemperature = 0.0f;
float scd4xHumidity = 0.0f;
// DPS368
float dps368Pressure = 0.0f;
float dps368Temperature = 0.0f;

// creating sensor instances
SensirionI2CScd4x scd4x;
SensirionI2CSen5x sen5x;
Dps3xx dps368 = Dps3xx();

// serial print a 16-bit unsigned integer in 4 digit HEX format by adding leading zeros
void printUint16Hex(uint16_t value) {
    Serial.print(value < 4096 ? "0" : "");
    Serial.print(value < 256 ? "0" : "");
    Serial.print(value < 16 ? "0" : "");
    Serial.print(value, HEX);
}

// serial print the SCD4x serial number using the printUint16Hex number formater function
void printSCD4xSerialNumber() {
    uint16_t serial0;
    uint16_t serial1;
    uint16_t serial2;

    uint16_t error;
    char errorMessage[256];

    error = scd4x.getSerialNumber(serial0, serial1, serial2);
    if (error) {
        Serial.print("Error trying to execute getSerialNumber(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.print("SCD40 Serial Number: 0x");
        printUint16Hex(serial0);
        printUint16Hex(serial1);
        printUint16Hex(serial2);
        Serial.println();
    }
    
}

// serial print the SEN5x serial number
void printSEN5xSerialNumber() {
    uint16_t error;
    char errorMessage[256];
    unsigned char serialNumber[32];
    uint8_t serialNumberSize = 32;

    error = sen5x.getSerialNumber(serialNumber, serialNumberSize);
    if (error) {
        Serial.print("Error trying to execute getSerialNumber(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.print(" Serial Number: ");
        Serial.println((char*)serialNumber);
    }
}

// serial print the SEN5x serial number, firmware and hardware version
void printSEN5xModuleVersions() {
    uint16_t error;
    char errorMessage[256];

    unsigned char productName[32];
    uint8_t productNameSize = 32;

    error = sen5x.getProductName(productName, productNameSize);

    if (error) {
        Serial.print("Error trying to execute getProductName(): ");
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
        Serial.print("Error trying to execute getVersion(): ");
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

void setup() {

    Serial.begin(BAUD_RATE);
    while (!Serial) {
        delay(100);
    }

    Wire.begin(SDA_PIN, SCL_PIN);

    scd4x.begin(Wire);  // default 0x62 I2C address
    sen5x.begin(Wire);  // default 0x69 I2C address
    dps368.begin(Wire); // default 0x76 I2C address

    delay(100);
    Serial.println("Booting...");

    // SCD4x Initialization
    // stop SCD4x potentially previously started measurement
    uint16_t scd4xError;
    char scd4xErrorMessage[256];
    scd4xError = scd4x.stopPeriodicMeasurement();
    if (scd4xError) {
        Serial.print("SCD4x Error trying to execute stopPeriodicMeasurement(): ");
        errorToString(scd4xError, scd4xErrorMessage, 256);
        Serial.println(scd4xErrorMessage);
    }

    // SEN5x Initialization
    // reset SEN5x 
    uint16_t sen5xError;
    char sen5xErrorMessage[256];
    sen5xError = sen5x.deviceReset();
    if (sen5xError) {
        Serial.print("SEN5x Error trying to execute deviceReset(): ");
        errorToString(sen5xError, sen5xErrorMessage, 256);
        Serial.println(sen5xErrorMessage);
    }

    // print sensor module information if I2C buffers are large enough
    #ifdef USE_PRODUCT_INFO
        printSCD4xSerialNumber();
        printSEN5xModuleVersions();
        // add ambient pressure sensor
    #endif

    #ifdef SEN5X_TEMP_OFFSET
        // set temperature offset for SEN5x
        sen5xError = sen5x.setTemperatureOffsetSimple(SEN5X_TEMP_OFFSET);
        if (sen5xError) {
            Serial.print("SEN5x Error trying to execute setTemperatureOffsetSimple(): ");
            errorToString(sen5xError, sen5xErrorMessage, 256);
            Serial.println(sen5xErrorMessage);
        } else {
            Serial.print("SEN5x Temperature Offset set to ");
            Serial.print(SEN5X_TEMP_OFFSET);
            Serial.println(" °C (SEN54/SEN55 only)");
        }
    #endif
    
    // start SCD4x Measurement
    scd4xError = scd4x.startPeriodicMeasurement();
    if (scd4xError) {
        Serial.print("SCD4x Error trying to execute startPeriodicMeasurement(): ");
        errorToString(scd4xError, scd4xErrorMessage, 256);
        Serial.println(scd4xErrorMessage);
    }

    // SEN5x start Measurement
    sen5xError = sen5x.startMeasurement();
    if (sen5xError) {
        Serial.print("SEN5x Error trying to execute startMeasurement(): ");
        errorToString(sen5xError, sen5xErrorMessage, 256);
        Serial.println(sen5xErrorMessage);
    }

    // add more sensor initializations here

    Serial.println("Waiting for first measurement... (5 sec)");
}

void loop() {

    // SCD4x
    uint16_t scd4xError;
    char scd4xErrorMessage[256];

    // check if SCD4x is ready to send data
    bool isDataReady = false;
    scd4xError = scd4x.getDataReadyFlag(isDataReady);
    if (scd4xError) {
        Serial.print("Error trying to execute getDataReadyFlag(): ");
        errorToString(scd4xError, scd4xErrorMessage, 256);
        Serial.println(scd4xErrorMessage);
        return;
    }
    if (!isDataReady) {
        return;
    }

    Serial.println("-----------------------");
    Serial.println("SCD4x Measurements:");
    // read and serial print SCD4x measurements
    scd4xError = scd4x.readMeasurement(co2, scd4xTemperature, scd4xHumidity);
    if (scd4xError) {
        Serial.print("Error trying to execute readMeasurement(): ");
        errorToString(scd4xError, scd4xErrorMessage, 256);
        Serial.println(scd4xErrorMessage);
    } else if (co2 == 0) {
        Serial.println("Invalid sample detected, skipping.");
    } else {
        Serial.print("CO2: "); Serial.print(co2); Serial.println(" ppm");
        Serial.print("Temperature: "); Serial.print(scd4xTemperature); Serial.println(" °C");
        Serial.print("Humidity: "); Serial.print(scd4xHumidity); Serial.println(" %");
    }

    // SEN5x
    Serial.println("-----------------------");
    Serial.println("SEN5x Measurements:");

    uint16_t sen5xError;
    char sen5xErrorMessage[256];

    // read and serial print SEN5x measurements
    sen5xError = sen5x.readMeasuredValues(
        massConcentrationPm1p0,
        massConcentrationPm2p5,
        massConcentrationPm4p0,
        massConcentrationPm10p0,
        sen5xHumidity, 
        sen5xTemperature, 
        vocIndex,
        noxIndex);

    if (sen5xError) {
        Serial.print("SEN5x Error trying to execute readMeasuredValues(): ");
        errorToString(sen5xError, sen5xErrorMessage, 256);
        Serial.println(sen5xErrorMessage);
    } else {
        Serial.print("PM 1.0: "); Serial.print(massConcentrationPm1p0); Serial.println(" µg/m³");
        Serial.print("PM 2.5: "); Serial.print(massConcentrationPm2p5); Serial.println(" µg/m³");
        Serial.print("PM 4.0: "); Serial.print(massConcentrationPm4p0); Serial.println(" µg/m³");
        Serial.print("PM 10: "); Serial.print(massConcentrationPm10p0); Serial.println(" µg/m³");

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
        if (isnan(vocIndex)) {
            Serial.println("n/a");
        } else {
            Serial.println(vocIndex);
        }

        Serial.print("NoxIndex: ");
        if (isnan(noxIndex)) {
            Serial.println("n/a");
        } else {
            Serial.println(noxIndex);
        }
    }

    // DPS368
    Serial.println("-----------------------");
    Serial.println("DPS368 Measurements:");

    int16_t dps368Error;

    dps368Error = dps368.measureTempOnce(dps368Temperature, DPS368_OVERSAMPLING);
    if (dps368Error) {
        Serial.print("DPS368 Error trying to execute measureTempOnce(): "); 
        Serial.println(dps368Error);
    } else {
        Serial.print("Temperature: "); Serial.print(dps368Temperature); Serial.println(" °C");
    }

    dps368Error = dps368.measurePressureOnce(dps368Pressure, DPS368_OVERSAMPLING);
    if (dps368Error) {
        Serial.print("DPS368 Error trying to execute measurePressureOnce(): ");
        Serial.println(dps368Error);
    } else {
        Serial.print("Pressure: "); Serial.print(dps368Pressure/100); Serial.println(" hPa"); 
    }


    delay(5000);
}




// working code for UI
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
    sprite.drawString("18:32", 240, 8);

    // Draw WiFi symbol
    if (WiFi.status() == WL_CONNECTED) {
        sprite.fillCircle(455, 18, 7, TFT_GREEN); // Indicate WiFi connection (green dot)
    } else {
        sprite.fillCircle(455, 18, 7, TFT_RED); // Indicate NO WiFi connection (red dot)
    }
}

void drawWidget(int x, int y, int w, int h, const char* label, const char* value, uint16_t color) {
    // Draw the rounded rectangle
    sprite.fillRoundRect(x, y, w, h, 10, color);
    sprite.fillRoundRect(x + 5, y + 5, w - 10, h - 10, 5, BACKGROUND_COLOR);

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
    drawWidget(5, 50, 110, 110, "Temp", String(temperature, 1).c_str(), ACCENT_COLOR);
    drawWidget(125, 50, 110, 110, "RH", String(humidity, 1).c_str(), TFT_GREENYELLOW);
    drawWidget(245, 50, 110, 110, "CO2", String(co2).c_str(), TFT_GREEN);
    drawWidget(365, 50, 110, 110, "Pressure", String(pressure, 2).c_str(), TFT_ORANGE);

    drawWidget(5, 180, 110, 110, "VOC", String(vocIndex).c_str(), TFT_GREEN);
    drawWidget(125, 180, 110, 110, "PM1", String(pm1, 1).c_str(), TFT_YELLOW);
    drawWidget(245, 180, 110, 110, "PM2.5", String(pm25, 1).c_str(), TFT_ORANGE);
    drawWidget(365, 180, 110, 110, "PM10", String(pm10, 1).c_str(), TFT_RED);

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
    Serial.print("Connecting to Wi-Fi...");
    WiFi.begin(ssid, password);
    unsigned long startAttemptTime = millis();

    // Try to connect without blocking
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
        delay(10);  // Short delay to allow other processes
        yield();    // Allow other tasks to run
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Connected to Wi-Fi!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        Serial.print("Free heap: ");
        Serial.println(ESP.getFreeHeap());

    } else {
        Serial.println("Failed to connect to Wi-Fi.");
    }
}





// semi working code for ui, wifi and clock

#include <TFT_eSPI.h> // Include the TFT library
#include <SPI.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// WiFi credentials
const char* ssid = "Vucjak";
const char* password = "2102971340015";

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
    Serial.begin(115200);

    // Connect to Wi-Fi
    //connectToWiFi();

    // Reinitialize display if needed
    tft.init(); Serial.println("tft.init()");
    tft.setRotation(3);
    tft.fillScreen(BACKGROUND_COLOR);

    // Draw initial UI
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
    Serial.println("drawUI()");
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

    Serial.print("Free heap: ");
    Serial.println(ESP.getFreeHeap());

    // Push the sprite to the screen
    sprite.pushSprite(0, 0);
    //sprite.deleteSprite();
    Serial.print("Free heap: ");
    Serial.println(ESP.getFreeHeap());

}

void drawClockAndWiFi() {
    // Draw clock
    String currentTime = timeClient.getFormattedTime();
    sprite.setTextDatum(TC_DATUM); // Top-left
    sprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
    sprite.setTextSize(2);
    sprite.drawString(currentTime, 10, 10);

    // Draw WiFi symbol
    if ("WiFi.status()" == WL_CONNECTED) {
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
    Serial.print("Connecting to Wi-Fi...");
    WiFi.begin(ssid, password);
    unsigned long startAttemptTime = millis();

    // Try to connect without blocking
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
        delay(10);  // Short delay to allow other processes
        yield();    // Allow other tasks to run
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Connected to Wi-Fi!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        Serial.print("Free heap: ");
        Serial.println(ESP.getFreeHeap());

    } else {
        Serial.println("Failed to connect to Wi-Fi.");
    }
}
