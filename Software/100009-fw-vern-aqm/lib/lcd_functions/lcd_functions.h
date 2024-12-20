#ifndef _LCD_FUNCTIONS_
#define _LCD_FUNCTIONS_

#include <Arduino.h>
#include <TFT_eSPI.h>

// LCD functions
void drawWidget(int x, int y, int w, int h, const char* label, const char* value, uint16_t color);
void drawClockAndWiFi();
void updateUI();
void drawUI();



#endif