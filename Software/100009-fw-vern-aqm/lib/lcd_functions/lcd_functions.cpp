#include "lcd_functions.h"
#include "config.h"

// Value colors
uint16_t getTempColor(float temperature) {

    uint8_t targetTemp = 23;
    if (abs(targetTemp-temperature) <= 2) {
        return GOOD_COLOR;   // Green for temperatures 21-25°C
    } else if (abs(targetTemp-temperature) <= 3) {
        return MEDIOCRE_COLOR;  // Yellow for temperatures 20-26°C
    } else if (abs(targetTemp-temperature) <= 4) {
        return BAD_COLOR;  // Orange for temperatures 19-27°C
    } else {
        return TERRIBLE_COLOR;     // Red for temperatures under 19 or over 27°C
    }
}

uint16_t getHumidityColor(float humidity) {

    uint8_t targetHumidity = 50;
    if (abs(targetHumidity-humidity) <= 10) {
        return GOOD_COLOR;   // Green for humidity 40-60%
    } else if (abs(targetHumidity-humidity) <= 15) {
        return MEDIOCRE_COLOR;  // Yellow for humidity 35-65%
    } else if (abs(targetHumidity-humidity) <= 20) {
        return BAD_COLOR;  // Orange for humidity 30-70%
    } else {
        return TERRIBLE_COLOR;     // Red for humidity under 30 or over 70%
    }
}
 
uint16_t getPressureColor(float pressure) {

    uint16_t targetPressure = 1013;
    if (abs(targetPressure-pressure) <= 5) {
        return GOOD_COLOR;   // Green for pressure 1008-1018hPa
    } else if (abs(targetPressure-pressure) <= 7) {
        return MEDIOCRE_COLOR;  // Yellow for pressure 1006-1020hPa
    } else if (abs(targetPressure-pressure) <= 12) {
        return BAD_COLOR;  // Orange for pressure 1001-1025hPa
    } else {
        return TERRIBLE_COLOR;     // Red for pressure under 1001 or over 1025hPa
    }
}

uint16_t getCO2Color(uint16_t co2) {

    if (co2 < 700) {
        return GOOD_COLOR;
    } else if (co2 >= 700 && co2 < 1200) {
        return MEDIOCRE_COLOR;
    } else if (co2 >= 1200 && co2 < 1500) {
        return BAD_COLOR;
    } else {
        return TERRIBLE_COLOR;
    }
}


uint16_t getVOCColor(uint16_t voc) {

    if (voc < 100) {
        return GOOD_COLOR;
    } else if (voc >= 100 && voc < 200) {
        return MEDIOCRE_COLOR;
    } else if (voc >= 200 && voc < 300) {
        return BAD_COLOR;
    } else {
        return TERRIBLE_COLOR;
    }
}


uint16_t getPM1Color(uint16_t pm1) {

    if (pm1 < 10) {
        return GOOD_COLOR;
    } else if (pm1 >= 10 && pm1 < 25) {
        return MEDIOCRE_COLOR;
    } else if (pm1 >= 25 && pm1 < 50) {
        return BAD_COLOR;
    } else {
        return TERRIBLE_COLOR;
    }
}


uint16_t getPM2_5Color(uint16_t pm2_5) {

    if (pm2_5 < 12) {
        return GOOD_COLOR;
    } else if (pm2_5 >= 12 && pm2_5 < 35) {
        return MEDIOCRE_COLOR;
    } else if (pm2_5 >= 35 && pm2_5 < 55) {
        return BAD_COLOR;
    } else {
        return TERRIBLE_COLOR;
    }
}


uint16_t getPM10Color(uint16_t pm10) {
    
    if (pm10 < 20) {
        return GOOD_COLOR;
    } else if (pm10 >= 20 && pm10 < 50) {
        return MEDIOCRE_COLOR;
    } else if (pm10 >= 50 && pm10 < 100) {
        return BAD_COLOR;
    } else {
        return TERRIBLE_COLOR;
    }
}



/*
// Draw initial UI with Wi-Fi credentials
void drawUI() {
    sprite.fillSprite(BACKGROUND_COLOR);
    sprite.fillRect(0,0,480,35, TOP_BAR_COLOR);
 
    sprite.setTextDatum(TL_DATUM); // Top-left
    sprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
    sprite.setTextSize(3);
    sprite.drawString("SSID:", 20, 60); sprite.drawString(SECRET_SSID, 120, 60);
    sprite.drawString("PASS:", 20, 120); sprite.drawString(SECRET_PASSWORD, 120, 120);

    sprite.pushSprite(0, 0);

    delay(1000);

    sprite.fillRect(0,35,480,285, BACKGROUND_COLOR);

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
    uint8_t y = 60;
    uint8_t w = 110;
    uint8_t h = 110;
    uint8_t v_space = 25;

    drawWidget(x +   0, y, w, h, "Temp",     "C",   String(TEMPERATURE, 1).c_str(),  GOOD_COLOR);
    drawWidget(x + 120, y, w, h, "RH",       "%",   String(HUMIDITY, 1).c_str(),     MEDIOCRE_COLOR);
    drawWidget(x + 240, y, w, h, "CO2",      "ppm", String(CO2).c_str(),             BAD_COLOR);
    drawWidget(x + 360, y, w, h, "Pressure", "hPa", String(PRESSURE/100, 2).c_str(), TERRIBLE_COLOR);

    drawWidget(x +   0, y + h + v_space, w, h, "VOC",   "Index", String(VOC_INDEX, 0).c_str(), TERRIBLE_COLOR);
    drawWidget(x + 120, y + h + v_space, w, h, "PM1",   "ug/m3", String(PM1, 0).c_str(),       BAD_COLOR);
    drawWidget(x + 240, y + h + v_space, w, h, "PM2.5", "ug/m3", String(PM2_5, 0).c_str(),     MEDIOCRE_COLOR);
    drawWidget(x + 360, y + h + v_space, w, h, "PM10",  "ug/m3", String(PM10, 0).c_str(),      GOOD_COLOR);

    sprite.pushSprite(0, 0);
}
*/
