# VERN' University  
Internet of Things Department  
Professor: doc. dr. sc. Petar Kolar, Roman Rubčić, dipl. ing. graf. teh.  
Student: Antonio Šimunčić  

**Smart air quality monitor**  

Zagreb, Croatia  
November, 2024  


# Introduction to the Smart Air Quality Monitor Project

Air pollution is becoming an increasingly serious environmental and health issue, affecting people's quality of life worldwide. Harmful particles and gases in the atmosphere can lead to numerous health problems, making air quality monitoring essential for protecting health and the environment.

The goal of this project is to develop a Smart Air Quality Monitor, a smart meteorological station that integrates advanced sensors to measure airborne dust particles, CO2, humidity, pressure, and temperature. This device would enable real-time air quality monitoring, providing users with critical information to help them make informed decisions and take necessary precautions.

# Week 1 - System architecture

Project Requirements:  

- **Temperature and Humidity Measurement**
- **Dust Particle Measurement** (PM1, PM2.5, PM4, PM10)
- **VOC Index Measurement** (Volatile Organic Compounds)
- **CO2 Level Measurement** in the Air
- **Atmospheric Pressure Measurement**
- **User Data Display**
- **Wireless Network Connectivity**


After defining the device requirements, I began searching for suitable components on platforms like Mouser, TME, and Aliexpress to find those that best match the specifications. The goal was to select reliable components that ensure high accuracy and longevity of the system, while also being compatible with the microcontroller and its software support.

For the microcontroller, I chose the ESP32-S3-WROOM-1 (N16R8) due to its affordable price, dual-core processor, ample onboard flash memory (16MB), and integrated PSRAM (8MB), which serves as a frame buffer for generating the graphical interface. Additionally, the microcontroller module includes built-in Wi-Fi and Bluetooth RF interfaces and an antenna, which significantly shortens development time and reduces the risk of wireless communication issues. 

**Block diagram:**  

![Block diagram photo](https://github.com/antoniosimuncic/VERN_Air-Quality-Meter/blob/main/Documentation/VERN_Air-Quality-Monitor-ENG-wb.png)  

# Week 2 - Schematic drawing

In week 2, I began creating the schematic in KiCad. The first task was to place all the selected components in the schematic, followed by a detailed study of the datasheets for the chosen components to ensure correct connections and desired system functionality. At this stage, I am also working on optimizing the schematic and the Bill of Materials (BOM) for a smooth transition to PCB design later on.

# Week 3 - Schematic release

In week 3, I completed the schematic design and released the finalized version. This involved thoroughly verifying all component connections and ensuring compatibility across the system, focusing on power distribution, signal integrity, and communication pathways between components. After careful checks, I created a preliminary Bill of Materials (BOM) and added detailed annotations to the schematic to guide the upcoming PCB layout phase. The release of the schematic marks a significant milestone, setting the foundation for the PCB design and allowing for initial testing.

**Schematic:** [Revision 10](https://github.com/antoniosimuncic/VERN_Air-Quality-Meter/blob/main/Hardware/100008_hw-vern-aqm/Schematic/100008_hw-vern-aqm.pdf)

# Week 4 - PCB design

In the fourth week of the project, I began designing the PCB based on the finalized schematic. This phase involved selecting the appropriate board dimensions, component placement, and routing the connections while considering design rules such as trace width, clearance, and power and heat management. A key focus was on USB differential pair impedance control, ensuring that the USB signal traces were routed with the correct 90Ω differential impedance to maintain signal integrity and reliable data transfer. I also worked on optimizing the layout for signal integrity, minimizing the length of high-speed signal traces, and ensuring proper reference planes and return paths for the components. After completing the layout and routing, I performed a Design Rule Check (DRC) to identify and correct any potential issues before moving on to generating the Gerber files for fabrication.

**PCB render:**  

![PCB render photo](https://github.com/antoniosimuncic/VERN_Air-Quality-Meter/blob/main/Hardware/100008_hw-vern-aqm/Images/top-side-view.png)  

# Week 5 - Component procurement  

In week 5, I focused on component procurement and ordering essential parts to prepare for assembly. I purchased components from Mouser to ensure the reliability of critical parts while sourcing additional items from Aliexpress for cost-effectiveness. Additionally, I ordered the PCBs and a stencil for solder application from JLCPCB. With components and PCBs on the way, the project is set to progress smoothly into the assembly phase once all parts arrive.

[Bill of Materials](https://html-preview.github.io/?url=https://github.com/antoniosimuncic/VERN_Air-Quality-Meter/blob/main/Hardware/100008_hw-vern-aqm/Manufacturing/Assembly/ibom/ibom.html)  

[Project expense report](https://github.com/antoniosimuncic/VERN_Air-Quality-Meter/blob/main/Documentation/project-expense-report.md)  

# Week 6 - PCB Assembly  

In week 6, I began assembling the PCB as the components and boards had arrived. Using the stencil from JLCPCB, I applied solder paste to the pads, and then I carefully placed each component onto the board, paying special attention to orientation and alignment. After assembling the board, I placed the board on an SMD hotplate reflow soldering station to perform the reflow soldering process. After the board cooled down, I inspected the connections to ensure the soldering process finished successfully. With the PCB assembly complete, I began the first board power-up to do the initial testing and troubleshooting.  

**Production process:**  

![Photo of the production process](https://github.com/antoniosimuncic/VERN_Air-Quality-Meter/blob/main/Documentation/production-process.jpg)  

# Week 7 - Board Bring-Up  

In week 7, I focused on the initial bring-up of the PCB. After assembling the board, I verified the power rails and ensured all components received the correct voltages. Basic functionality, including communication interfaces such as UART and I2C, was tested to confirm proper connectivity between the microcontroller and peripherals. Rigol DS1054z oscilloscope was used to identify and resolve minor issues on the UART lines. Soldering fixes were necessary to ensure the board operated as expected. This stage established a solid foundation for further firmware development and system testing.  

**Sensor test source code:** [sensors-bring-up.cpp](https://github.com/antoniosimuncic/VERN_Air-Quality-Meter/blob/main/Documentation/sensors-bring-up.cpp)  

# Week 8 - Case design  

In week 8, I focused on designing a custom 3D-printed case for the device after I had confirmed the board functioned as expected. The design accounted for component placement, ventilation, and accessibility for ports and connectors. Aesthetics and durability were considered to ensure the case complemented the functionality and appearance of the project. This step helped prepare the device for practical use and testing in real-world conditions. The 3D print was made using PET-G on the BambuLab P1S printer.  

**Case design:**  

![Photo of the case design](https://github.com/antoniosimuncic/VERN_Air-Quality-Meter/blob/main/Documentation/case-design.jpg)  

# Week 9: Firmware Architecture and Server Communication

In week 9, the focus was on developing and refining the firmware responsible for Wi-Fi connectivity, data acquisition, and communication with an HTTP server. Below is a detailed breakdown of the tasks completed and code implementations.

### Wi-Fi Connectivity

The firmware establishes a Wi-Fi connection using the ESP32-S3 microcontroller. A robust retry mechanism was implemented to handle connection failures.

Code Snippet:
```c
void connectToWifi() {
    uint8_t attempt = 0;
    while (attempt < WIFI_RETRIES) {
        Serial.printf("Attempt %u to connect to Wi-Fi...\n", attempt++);
        WiFi.mode(WIFI_STA);
        WiFi.begin(SECRET_SSID, SECRET_PASSWORD);
        unsigned long startAttemptTime = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT) {
            delay(10);
            yield();
        }
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("Connected to Wi-Fi");
            Serial.print("IP Address: ");
            Serial.println(WiFi.localIP());
            return;
        }
        Serial.println("Failed to connect. Retrying...");
        delay(2000);
        attempt++;
    }
    Serial.println("Exceeded maximum retry attempts. Check your Wi-Fi credentials or network.");
}
```

### Server Communication

Data collected from sensors is sent to an HTTP server in JSON format. The sendDataToHTTPServer function handles this process, including constructing the JSON payload and managing HTTP requests.

- Request body:
```json
{
 "mac_address": "AA:BB:CC:DD:EE:FF",
 "temperature": 22.5,
 "humidity": 45.0,
 "pm1": 10,
 "pm2_5": 20,
 "pm4": 25,
 "pm10": 30,
 "co2": 450,
 "voc": 100,
 "pressure": 1013.25
}
```
     
Code Snippet:
```c
void sendDataToHTTPServer(String mac_address, float temperature, float humidity, float pm1, float pm2_5, float pm4, float pm10, uint16_t co2, float voc, float pressure) {
    String jsonData = "{";
    jsonData += "\"mac_address\":\"" + mac_address + "\",";
    jsonData += "\"temperature\":" + String(temperature) + ",";
    jsonData += "\"humidity\":" + String(humidity) + ",";
    jsonData += "\"pm1\":" + String(pm1) + ",";
    jsonData += "\"pm2_5\":" + String(pm2_5) + ",";
    jsonData += "\"pm4\":" + String(pm4) + ",";
    jsonData += "\"pm10\":" + String(pm10) + ",";
    jsonData += "\"co2\":" + String(co2) + ",";
    jsonData += "\"voc\":" + String(voc) + ",";
    jsonData += "\"pressure\":" + String(pressure);
    jsonData += "}";

    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(SERVER_URL);
        http.addHeader("Content-Type", "application/json");
        int httpResponseCode = http.POST(jsonData);
        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println("Response code: " + String(httpResponseCode));
            Serial.println("Response: " + response);
        } else {
            Serial.println("Error on sending POST: " + String(httpResponseCode));
        }
        http.end();
    } else {
        Serial.println("Wi-Fi not connected");
    }
}
```
View Full Code: [server_functions.cpp](https://github.com/antoniosimuncic/VERN_Air-Quality-Meter/blob/main/Software/100009-fw-vern-aqm/lib/server_functions/server_functions.cpp)

# Week 10: LCD Integration and Sensor Visualization

In week 10, the focus shifted to integrating the TFT display for visualizing sensor data. The firmware includes functionality for determining display colors based on sensor values, enhancing user experience.

### Dynamic Value Coloring

Each sensor value is displayed in a color-coded format to indicate its status (e.g., good, mediocre, bad, terrible).  
Functions like `getTempColor()` and `getHumidityColor()` determine the appropriate color for each value.

Code Snippet:
```c
uint16_t getHumidityColor(float humidity) {

    uint8_t targetHumidity = 50;
    if (abs(targetHumidity-humidity) <= 10) {
        return GOOD_COLOR;  // Green for humidity 40-60%
    } else if (abs(targetHumidity-humidity) <= 15) {
        return MEDIOCRE_COLOR;  // Yellow for humidity 35-65%
    } else if (abs(targetHumidity-humidity) <= 20) {
        return BAD_COLOR;  // Orange for humidity 30-70%
    } else {
        return TERRIBLE_COLOR;  // Red for humidity under 30 or over 70%
    }
}
```
View Full Code: [server_functions.cpp](https://github.com/antoniosimuncic/VERN_Air-Quality-Meter/blob/main/Software/100009-fw-vern-aqm/lib/lcd_functions/lcd_functions.cpp)

### Sensor Value Visualization

The display shows sensor values for temperature, humidity, CO2, VOC Index, and particulate matter (PM1, PM2.5, PM10). Each value is updated dynamically and rendered in real time.

Code Snippet:
```c
// Draw initial UI with Wi-Fi credentials
void drawUI() {
    sprite.fillSprite(BACKGROUND_COLOR);

    drawClockAndWiFi();

    sprite.setTextDatum(TL_DATUM); // Top-left
    sprite.setTextColor(TEXT_COLOR, TOP_BAR_COLOR);
    sprite.setTextSize(3);
    sprite.drawString("Time Zone", 10, 8);
    sprite.setTextDatum(TL_DATUM); // Top-left
    sprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
    sprite.setTextSize(3);
    sprite.drawString("SSID:", 20, 60);  sprite.drawString(SECRET_SSID, 120, 60);
    sprite.drawString("PASS:", 20, 120); sprite.drawString(SECRET_PASSWORD, 120, 120);
    sprite.drawString("IP:", 20, 180); sprite.drawString(ipString, 120, 180);

    sprite.pushSprite(0, 0);
    sprite.fillRect(0,35,480,285, BACKGROUND_COLOR);
    sprite.pushSprite(0, 0);

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
    uint8_t y = 55;
    uint8_t w = 110;
    uint8_t h = 110;
    uint8_t v_space = 25;

    drawWidget(x +   0, y, w, h, "Temp",     "C",   String(TEMPERATURE, 1).c_str(),  getTempColor(TEMPERATURE));
    drawWidget(x + 120, y, w, h, "RH",       "%",   String(HUMIDITY, 1).c_str(),     getHumidityColor(HUMIDITY));
    drawWidget(x + 240, y, w, h, "CO2",      "ppm", String(CO2).c_str(),             getCO2Color(CO2));
    drawWidget(x + 360, y, w, h, "Pressure", "hPa", String(PRESSURE/100, 2).c_str(), getPressureColor(PRESSURE));

    drawWidget(x +   0, y + h + v_space, w, h, "VOC",   "Index", String(VOC_INDEX, 0).c_str(), getVOCColor(VOC_INDEX));
    drawWidget(x + 120, y + h + v_space, w, h, "PM1",   "ug/m3", String(PM1, 0).c_str(),       getPM1Color(PM1));
    drawWidget(x + 240, y + h + v_space, w, h, "PM2.5", "ug/m3", String(PM2_5, 0).c_str(),     getPM2_5Color(PM2_5));
    drawWidget(x + 360, y + h + v_space, w, h, "PM10",  "ug/m3", String(PM10, 0).c_str(),      getPM10Color(PM10));

    drawClockAndWiFi();

    sprite.pushSprite(0, 0);
}
```
### Time and Wi-Fi indicator  

The top bar of the display is reserved for a digital clock that syncs with a NTP time server on the Wi-Fi network, there is also a red/green dot in the upper right corner that shows Wi-Fi status.

```c
// Draw clock and Wi-Fi indicator
void drawClockAndWiFi() {
    // Draw clock
    unsigned long currentEpoch = timeClient.getEpochTime();
    String currentTime = epochToHoursMinutes(currentEpoch);

    sprite.fillRect(0,0,480,35, TOP_BAR_COLOR);
    sprite.setTextDatum(TC_DATUM); // Top-center
    sprite.setTextColor(TEXT_COLOR, TOP_BAR_COLOR);
    sprite.setTextSize(3);
    sprite.drawString(currentTime, 240, 8);

    // Draw WiFi symbol and indication - TODO
    if (WiFi.status() == WL_CONNECTED) {
        sprite.fillCircle(455, 18, 7, TFT_GREEN); // Indicate WiFi connection (green dot)
    } else {
        sprite.fillCircle(455, 18, 7, TFT_RED); // Indicate NO WiFi connection (red dot)
    }
}
```
View Full Code: [main.cpp](https://github.com/antoniosimuncic/VERN_Air-Quality-Meter/blob/main/Software/100009-fw-vern-aqm/src/main.cpp)

# Week 11 and 12: Node.js Server with visual data dashboard

Last part of the project was to design a web-application for managing the sensors and viewing and storing the data.  

**Web-App Repository**: [Nodejs-IoT-Server](https://github.com/antoniosimuncic/nodejs-iot-server)  

![IoT Sensor Dashboard](https://github.com/antoniosimuncic/VERN_Air-Quality-Meter/blob/main/Documentation/dashboard-screenshot.png)  

---


