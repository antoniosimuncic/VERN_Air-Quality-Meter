#include <Arduino.h>
#include <Wire.h>
#include <Dps3xx.h>
#include <SensirionI2CScd4x.h>
#include <SensirionI2CSen5x.h>
#include <TFT_eSPI.h>

#define MAXBUF_REQUIREMENT 48 // The used commands use up to 48 bytes.
#if (defined(I2C_BUFFER_LENGTH) && (I2C_BUFFER_LENGTH >= MAXBUF_REQUIREMENT)) || \
    (defined(BUFFER_LENGTH) && BUFFER_LENGTH >= MAXBUF_REQUIREMENT)
#define USE_PRODUCT_INFO
#endif

// Wi-Fi credentials
const char* ssid = "ALHN-2B78";
const char* password = "DPgWqUGMw7";

// configuration
#define BAUD_RATE 115200
#define SEN5X_TEMP_OFFSET -2.0
#define DPS368_OVERSAMPLING 7
#define BRIGHTNESS_VALUE 150

// pin declaration
#define SDA_PIN  21
#define SCL_PIN  38
#define BACKLIGHT_PIN 18

// Define colors for UI
#define BACKGROUND_COLOR TFT_BLACK
#define TEXT_COLOR TFT_WHITE
#define LABEL_COLOR TFT_WHITE
#define TOP_BAR_COLOR TFT_DARKGREY
#define MEASUREMENT_TEXT_COLOR TFT_CYAN
#define ACCENT_COLOR TFT_CYAN
// Define colors for value representation
#define GOOD_COLOR TFT_GREEN
#define MEDIOCRE_COLOR TFT_YELLOW
#define BAD_COLOR TFT_ORANGE
#define TERRIBLE_COLOR TFT_RED
/*
struct TempThresholds
{
    bool delta = true;
    u_int8_t nominal = 21;
    u_int8_t good_delta = 3;
    u_int8_t mediocre_delta = 5;
    u_int8_t bad_delta = 7;
    u_int8_t terrible_delta = 8;
};


uint16_t getColor(float value, Thresholds values){
    uint16_t color;
    if (values.delta) {

    }


    return color;
}
*/
// Creating sensor instances
Dps3xx dps368 = Dps3xx();
SensirionI2CScd4x scd4x = SensirionI2CScd4x();
SensirionI2CSen5x sen5x = SensirionI2CSen5x();

// Creating display instances
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

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


// Function declarations
// Display functions
void drawWidget(int x, int y, int w, int h, const char* label, const char* value, uint16_t color);
void drawClockAndWiFi();
void updateUI();
void drawUI();

// Wi-Fi functions
void connectToWiFi();

// Sensor functions
void printUint16Hex(uint16_t value);
void printSCD4xSerialNumber();
void printSEN5xSerialNumber();
void printSEN5xModuleVersions();


// Setup function - executes once on power up
void setup() {
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

    // Initialize serial communication
    Serial.begin(BAUD_RATE);
    while (!Serial) {
        delay(100);
    }

    // Initialize I2C bus
    Wire.begin(SDA_PIN, SCL_PIN);
    scd4x.begin(Wire);  // default 0x62 I2C address
    sen5x.begin(Wire);  // default 0x69 I2C address
    dps368.begin(Wire); // default 0x76 I2C address

    delay(100);
    Serial.println("Booting...");

    // SCD4x Initialization
    // Stop SCD4x potentially previously started measurement
    uint16_t scd4xError;
    char scd4xErrorMessage[256];
    scd4xError = scd4x.stopPeriodicMeasurement();
    if (scd4xError) {
        Serial.print("SCD4x Error trying to execute stopPeriodicMeasurement(): ");
        errorToString(scd4xError, scd4xErrorMessage, 256);
        Serial.println(scd4xErrorMessage);
    }

    // SEN5x Initialization
    // Reset SEN5x 
    uint16_t sen5xError;
    char sen5xErrorMessage[256];
    sen5xError = sen5x.deviceReset();
    if (sen5xError) {
        Serial.print("SEN5x Error trying to execute deviceReset(): ");
        errorToString(sen5xError, sen5xErrorMessage, 256);
        Serial.println(sen5xErrorMessage);
    }

    // Print sensor module information if I2C buffers are set 
    #ifdef USE_PRODUCT_INFO
        printSCD4xSerialNumber();
        printSEN5xModuleVersions();
        // Add ambient pressure sensor
    #endif

    #ifdef SEN5X_TEMP_OFFSET
        // Set temperature offset for SEN5x
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

    // Start SCD4x measurement
    scd4xError = scd4x.startPeriodicMeasurement();
    if (scd4xError) {
        Serial.print("SCD4x Error trying to execute startPeriodicMeasurement(): ");
        errorToString(scd4xError, scd4xErrorMessage, 256);
        Serial.println(scd4xErrorMessage);
    }

    // Start SEN5x measurement
    sen5xError = sen5x.startMeasurement();
    if (sen5xError) {
        Serial.print("SEN5x Error trying to execute startMeasurement(): ");
        errorToString(sen5xError, sen5xErrorMessage, 256);
        Serial.println(sen5xErrorMessage);
    }

    // add external sensor initializations here

    Serial.println("Waiting for first measurement... (5 sec)");
}

// Main loop function - executes in loop after setup function is executed
void loop() {
    // SCD4x
    // Check if SCD4x is ready to send data
    bool isDataReady = false;
    uint16_t scd4xError;
    char scd4xErrorMessage[256];
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

    // Read and serial print SCD4x measurements
    Serial.println("-----------------------");
    Serial.println("SCD4x Measurements:");
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
    // Read and serial print SEN5x measurements
    Serial.println("-----------------------");
    Serial.println("SEN5x Measurements:");
    uint16_t sen5xError;
    char sen5xErrorMessage[256];

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
            Serial.print(sen5xHumidity); Serial.print(" %");
            Serial.print("  Humidity-10: "); Serial.print(sen5xHumidity-10); Serial.println(" %");
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
    /*
    dps368Error = dps368.measureTempOnce(dps368Temperature, DPS368_OVERSAMPLING);
    if (dps368Error) {
        Serial.print("DPS368 Error trying to execute measureTempOnce(): "); 
        Serial.println(dps368Error);
    } else {
        Serial.print("Temperature: "); Serial.print(dps368Temperature); Serial.println(" °C");
    }
    */
    dps368Error = dps368.measurePressureOnce(dps368Pressure, DPS368_OVERSAMPLING);
    if (dps368Error) {
        Serial.print("DPS368 Error trying to execute measurePressureOnce(): ");
        Serial.println(dps368Error);
    } else {
        Serial.print("Pressure: "); Serial.print(dps368Pressure/100); Serial.println(" hPa"); 
    }

    // Refresh UI
    updateUI();

    delay(100);
}

// Draw initial UI with Wi-Fi credentials
void drawUI() {
    sprite.fillSprite(BACKGROUND_COLOR);
    sprite.fillRect(0,0,480,35, TOP_BAR_COLOR);

    drawClockAndWiFi();

    sprite.setTextDatum(TL_DATUM); // Top-left
    sprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
    sprite.setTextSize(3);
    sprite.drawString("SSID:", 20, 60); sprite.drawString(ssid, 120, 60);
    sprite.drawString("PASS:", 20, 120); sprite.drawString(password, 120, 120);

    sprite.pushSprite(0, 0);

    delay(2000);

    sprite.fillSprite(BACKGROUND_COLOR);
    sprite.fillRect(0,0,480,35, TOP_BAR_COLOR);

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
    if (false) {
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
    sprite.drawString(label, x + w / 2, y + 10);

    // Draw the value
    sprite.setTextSize(2);
    sprite.setTextColor(MEASUREMENT_TEXT_COLOR, BACKGROUND_COLOR);
    sprite.drawString(value, x + w / 2, (y + h / 2)-8);

    // Draw the measurement unit
    sprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
    sprite.setTextDatum(TC_DATUM);
    sprite.setTextSize(2);
    sprite.drawString(unit, x + w / 2, y + 80);
}

// Update each widget with new sensor values
void updateUI() {
    drawWidget(5, 50, 110, 110, "Temp", "C", String(sen5xTemperature, 1).c_str(), GOOD_COLOR);
    drawWidget(125, 50, 110, 110, "RH", "%", String((scd4xHumidity+sen5xHumidity)/2, 1).c_str(), MEDIOCRE_COLOR);
    drawWidget(245, 50, 110, 110, "CO2", "ppm", String(co2).c_str(), BAD_COLOR);
    drawWidget(365, 50, 110, 110, "Pressure", "hPa", String(dps368Pressure/100, 2).c_str(), TERRIBLE_COLOR);

    drawWidget(5, 180, 110, 110, "VOC", "Index", String(vocIndex, 0).c_str(), TERRIBLE_COLOR);
    drawWidget(125, 180, 110, 110, "PM1", "ug/m3", String(massConcentrationPm1p0, 0).c_str(), BAD_COLOR);
    drawWidget(245, 180, 110, 110, "PM2.5", "ug/m3", String(massConcentrationPm2p5, 0).c_str(), MEDIOCRE_COLOR);
    drawWidget(365, 180, 110, 110, "PM10", "ug/m3", String(massConcentrationPm10p0, 0).c_str(), GOOD_COLOR);

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

// Serial print the SEN5x serial number
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

// Serial print the SEN5x serial number, firmware and hardware version
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