#include <Arduino.h>
#include <Wire.h>
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
#define TEMP_OFFSET 0.0

// pin declaration
#define SDA_PIN  21
#define SCL_PIN  38

// variables for measured values
// SEN5x
float massConcentrationPm1p0;
float massConcentrationPm2p5;
float massConcentrationPm4p0;
float massConcentrationPm10p0;
float ambientHumidity;
float ambientTemperature;
float vocIndex;
float noxIndex;
// SCD4x
uint16_t co2 = 0;
float temperature = 0.0f;
float humidity = 0.0f;
// DPS368


// creating sensor instances
SensirionI2CScd4x scd4x;
SensirionI2CSen5x sen5x;

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
    scd4x.begin(Wire);
    sen5x.begin(Wire);
    delay(50);

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

    #ifdef TEMP_OFFSET
        // set temperature offset for SEN5x
        sen5xError = sen5x.setTemperatureOffsetSimple(TEMP_OFFSET);
        if (sen5xError) {
            Serial.print("SEN5x Error trying to execute setTemperatureOffsetSimple(): ");
            errorToString(sen5xError, sen5xErrorMessage, 256);
            Serial.println(sen5xErrorMessage);
        } else {
            Serial.print("SEN5x Temperature Offset set to ");
            Serial.print(TEMP_OFFSET);
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

    // add ambient pressure sensor


    Serial.println("Waiting for first measurement... (5 sec)");
}

void loop() {

    //SCD4x
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

    // read and serial print SCD4x measurements
    scd4xError = scd4x.readMeasurement(co2, temperature, humidity);
    if (scd4xError) {
        Serial.print("Error trying to execute readMeasurement(): ");
        errorToString(scd4xError, scd4xErrorMessage, 256);
        Serial.println(scd4xErrorMessage);
    } else if (co2 == 0) {
        Serial.println("Invalid sample detected, skipping.");
    } else {
        Serial.println("-----------------------");
        Serial.println("SCD4x Measurements:");
        Serial.print("CO₂: "); Serial.print(co2); Serial.println(" ppm");
        Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" °C");
        Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");
    }

    // SEN5x
    uint16_t sen5xError;
    char sen5xErrorMessage[256];

    // read and serial print SEN5x measurements
    sen5xError = sen5x.readMeasuredValues(
        massConcentrationPm1p0,
        massConcentrationPm2p5,
        massConcentrationPm4p0,
        massConcentrationPm10p0,
        ambientHumidity, 
        ambientTemperature, 
        vocIndex,
        noxIndex);

    if (sen5xError) {
        Serial.print("SEN5x Error trying to execute readMeasuredValues(): ");
        errorToString(sen5xError, sen5xErrorMessage, 256);
        Serial.println(sen5xErrorMessage);
    } else {
        Serial.println("-----------------------");
        Serial.println("SEN5x Measurements:");
        Serial.print("PM 1.0: "); Serial.print(massConcentrationPm1p0); Serial.println(" µg/m³");
        Serial.print("PM 2.5: "); Serial.print(massConcentrationPm2p5); Serial.println(" µg/m³");
        Serial.print("PM 4.0: "); Serial.print(massConcentrationPm4p0); Serial.println(" µg/m³");
        Serial.print("PM 10: "); Serial.print(massConcentrationPm10p0); Serial.println(" µg/m³");

        Serial.print("Temperature: ");
        if (isnan(ambientTemperature)) {
            Serial.println("n/a");
        } else {
            Serial.print(ambientTemperature); Serial.println(" °C");
        }

        Serial.print("Humidity: ");
        if (isnan(ambientHumidity)) {
            Serial.println("n/a");
        } else {
            Serial.print(ambientHumidity); Serial.println(" %");
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

    delay(5000);
}