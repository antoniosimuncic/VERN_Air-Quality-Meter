#ifndef _LCD_FUNCTIONS_
#define _LCD_FUNCTIONS_

#include <Arduino.h>
#include <TFT_eSPI.h>

// LCD functions
void drawWidget(int x, int y, int w, int h, const char* label, const char* value, uint16_t color);
void drawClockAndWiFi();
void updateUI();
void drawUI();

// Value colors
uint16_t getTempColor(float temperature);
uint16_t getHumidityColor(float humidity);
uint16_t getPressureColor(float pressure);
uint16_t getCO2Color(uint16_t co2);

uint16_t getVOCColor(uint16_t voc);
uint16_t getPM1Color(uint16_t pm1);
uint16_t getPM2_5Color(uint16_t pm2_5);
uint16_t getPM10Color(uint16_t pm10);


#endif