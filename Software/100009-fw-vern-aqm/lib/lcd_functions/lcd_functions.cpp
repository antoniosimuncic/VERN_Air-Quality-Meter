#include "lcd_functions.h"
#include "config.h"



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