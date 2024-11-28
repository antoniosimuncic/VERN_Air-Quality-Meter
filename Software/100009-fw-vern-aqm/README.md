    /*
    set a temperature offset in degrees celsius
    Note: supported by SEN54 and SEN55 sensors
    By default, the temperature and humidity outputs from the sensor
    are compensated for the modules self-heating. If the module is
    designed into a device, the temperature compensation might need
    to be adapted to incorporate the change in thermal coupling and
    self-heating of other device components.
    
    A guide to achieve optimal performance, including references
    to mechanical design-in examples can be found in the app note
    “SEN5x – Temperature Compensation Instruction” at www.sensirion.com.
    Please refer to those application notes for further information
    on the advanced compensation settings used
    in `setTemperatureOffsetParameters`, `setWarmStartParameter` and
    `setRhtAccelerationMode`.
    
    Adjust tempOffset to account for additional temperature offsets
    exceeding the SEN module's self heating.
    */

#include <Wire.h>
#include "SensirionI2CSen5x.h"
#include "SensirionI2CScd4x.h"
#include <SPI.h> // Ensure SPI is included

#define SDA_PIN 21
#define SCL_PIN 38

SensirionI2CSen5x sen54; // SEN54 object
SensirionI2CScd4x scd40; // SCD40 object

void setup() {
    Serial.begin(115200);
    delay(1000); // Wait for Serial to initialize

    // Initialize I2C
    Wire.begin(SDA_PIN, SCL_PIN);

    // Initialize SEN54
    sen54.begin(Wire);
    uint16_t error = sen54.startMeasurement();
    if (error) {
        Serial.print("SEN54 initialization failed with error code: ");
        Serial.println(error);
    } else {
        Serial.println("SEN54 initialized successfully.");
    }

    // Initialize SCD40
    scd40.begin(Wire);
    uint16_t scdError = scd40.startPeriodicMeasurement();
    if (scdError) {
        Serial.print("SCD40 initialization failed with error code: ");
        Serial.println(scdError);
    } else {
        Serial.println("SCD40 initialized successfully.");
    }
}

void loop() {
    // Read data from SEN54
    float massConcentrationPm1p0, massConcentrationPm2p5, massConcentrationPm4p0, massConcentrationPm10p0;
    float ambientHumidity, ambientTemperature, vocIndex, noxIndex;
    uint16_t sen54Error = sen54.readMeasuredValues(
        massConcentrationPm1p0,
        massConcentrationPm2p5,
        massConcentrationPm4p0,
        massConcentrationPm10p0,
        ambientHumidity,
        ambientTemperature,
        vocIndex,
        noxIndex
    );

    if (!sen54Error) {
        Serial.println("SEN54 Measurements:");
        Serial.print("PM1.0: "); Serial.print(massConcentrationPm1p0); Serial.println(" µg/m³");
        Serial.print("PM2.5: "); Serial.print(massConcentrationPm2p5); Serial.println(" µg/m³");
        Serial.print("PM4.0: "); Serial.print(massConcentrationPm4p0); Serial.println(" µg/m³");
        Serial.print("PM10: "); Serial.print(massConcentrationPm10p0); Serial.println(" µg/m³");
        Serial.print("Humidity: "); Serial.print(ambientHumidity); Serial.println(" %");
        Serial.print("Temperature: "); Serial.print(ambientTemperature); Serial.println(" °C");
        Serial.print("VOC Index: "); Serial.println(vocIndex);
        //Serial.print("NOx Index: "); Serial.println(noxIndex); - Not available on SEN54, Only for SEN55 models
    } else {
        Serial.print("Error reading from SEN54: ");
        Serial.println(sen54Error);
    }

    // Read data from SCD40
    uint16_t co2;
    float temperature, humidity;
    uint16_t scdError = scd40.readMeasurement(co2, temperature, humidity);
    if (!scdError) {
        Serial.println("SCD40 Measurements:");
        Serial.print("CO2: "); Serial.print(co2); Serial.println(" ppm");
        Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" °C");
        Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");
    } else {
        Serial.print("Error reading from SCD40: ");
        Serial.println(scdError);
    }

    Serial.println("----------------------------------");
    delay(5000); // Wait 5 seconds before the next measurement
}


// working scd40 code

#include <Arduino.h>
#include <SensirionI2CScd4x.h>
#include <Wire.h>

// configuration
#define BAUD_RATE 115200

// pin declaration
#define SDA_PIN  21
#define SCL_PIN  38


// creating sensor instances
SensirionI2CScd4x scd4x;

// function declaration

// serial print a 16-bit unsigned integer in 4 digit HEX format by adding leading zeros
void printUint16Hex(uint16_t value) {
    Serial.print(value < 4096 ? "0" : "");
    Serial.print(value < 256 ? "0" : "");
    Serial.print(value < 16 ? "0" : "");
    Serial.print(value, HEX);
}

// serial print the formatted serial number
void printSCD4xSerialNumber(uint16_t serial0, uint16_t serial1, uint16_t serial2) {
    Serial.print("SCD40 Serial Number: 0x");
    printUint16Hex(serial0);
    printUint16Hex(serial1);
    printUint16Hex(serial2);
    Serial.println();
}


void setup() {

    Serial.begin(BAUD_RATE);
    while (!Serial) {
        delay(100);
    }

    Serial.println("Booting...");

    Wire.begin(SDA_PIN, SCL_PIN);

    uint16_t error;
    char errorMessage[256];

    scd4x.begin(Wire);

    // stop potentially previously started measurement
    error = scd4x.stopPeriodicMeasurement();
    if (error) {
        Serial.print("Error trying to execute stopPeriodicMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }

    uint16_t serial0;
    uint16_t serial1;
    uint16_t serial2;
    error = scd4x.getSerialNumber(serial0, serial1, serial2);
    if (error) {
        Serial.print("Error trying to execute getSerialNumber(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        printSCD4xSerialNumber(serial0, serial1, serial2);
    }

    // Start Measurement
    error = scd4x.startPeriodicMeasurement();
    if (error) {
        Serial.print("Error trying to execute startPeriodicMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }

    Serial.println("Waiting for first measurement... (5 sec)");
}

void loop() {
    uint16_t error;
    char errorMessage[256];

    delay(100);

    // read SCD40 Measurements
    uint16_t co2 = 0;
    float temperature = 0.0f;
    float humidity = 0.0f;
    bool isDataReady = false;
    error = scd4x.getDataReadyFlag(isDataReady);
    if (error) {
        Serial.print("Error trying to execute getDataReadyFlag(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
        return;
    }
    if (!isDataReady) {
        return;
    }
    error = scd4x.readMeasurement(co2, temperature, humidity);
    if (error) {
        Serial.print("Error trying to execute readMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else if (co2 == 0) {
        Serial.println("Invalid sample detected, skipping.");
    } else {
        Serial.println("----------------------------------");
        Serial.println("SCD40 Measurements:");
        Serial.print("CO²: "); Serial.print(co2); Serial.println(" ppm");
        Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" °C");
        Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");
    }
}

