#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <Arduino.h>
#include "secrets.h"

// general configuration
#define BRIGHTNESS_VALUE 40
#define BAUD_RATE 115200
#define SEN5X_TEMP_OFFSET -2.0
#define SEN5X_CLEANING_INTERVAL 86400
#define DPS368_OVERSAMPLING 7
#define SHT4X_ADDRESS 0x44
#define DPS368_ADDRESS 0x77
#define WIFI_TIMEOUT 5000
#define WIFI_RETRIES 3
#define DEBUG_SERIAL 1

// pin declaration
#define SDA_PIN  21
#define SCL_PIN  38
#define BACKLIGHT_PIN 18

// Define colors for UI
#define BACKGROUND_COLOR TFT_BLACK
#define TEXT_COLOR TFT_WHITE
#define LABEL_COLOR TFT_WHITE
#define TOP_BAR_COLOR  0x632c //0x39a6
#define MEASUREMENT_TEXT_COLOR TFT_CYAN
#define ACCENT_COLOR TFT_CYAN
// Define colors for value representation
#define GOOD_COLOR TFT_GREEN
#define MEDIOCRE_COLOR TFT_YELLOW
#define BAD_COLOR TFT_ORANGE
#define TERRIBLE_COLOR TFT_RED

// struct TempThresholds
// {
//     bool delta = true;
//     u_int8_t nominal = 21;
//     u_int8_t good_delta = 3;
//     u_int8_t mediocre_delta = 5;
//     u_int8_t poor_delta = 7;
//     u_int8_t verypoor_delta = 8;
// };


// uint16_t getColor(float value, Thresholds values){
//     uint16_t color;
//     if (values.delta) {

//     }


//     return color;
// }

// I2C buffer configuration
#define MAXBUF_REQUIREMENT 48 // The used commands use up to 48 bytes.
#if (defined(I2C_BUFFER_LENGTH) && (I2C_BUFFER_LENGTH >= MAXBUF_REQUIREMENT)) || \
    (defined(BUFFER_LENGTH) && BUFFER_LENGTH >= MAXBUF_REQUIREMENT)
#define USE_PRODUCT_INFO
#endif

// SHT4x error define
#ifdef NO_ERROR
#undef NO_ERROR
#endif
#define NO_ERROR 0    

#endif