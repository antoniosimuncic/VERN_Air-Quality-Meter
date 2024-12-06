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