# VERN_Air-Quality-Meter
VERN' University
Smjer: IoT  
Course: IoT product development  
Course professor: doc. dr. sc. Petar Kolar  

**Smart air quality meter**  

Student: Antonio Šimunčić  
Zagreb, Croatia November 2024.

# Introduction to the Smart Air Quality Monitor Project

Air pollution is becoming an increasingly serious environmental and health issue, affecting the quality of life for people worldwide. Harmful particles and gases in the atmosphere can lead to numerous health problems, making air quality monitoring essential for protecting both health and the environment.

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

Block diagram:  

![alt text](https://github.com/antoniosimuncic/VERN_Air-Quality-Meter/blob/main/Documentation/VERN_Air-Quality-Monitor.drawio.png?raw=true)  

# Week 2 - Schematic drawing

In the second week of the project, I began creating the schematic in KiCad. The first task was to place all the selected components in the schematic, followed by a detailed study of the datasheets for the chosen components to ensure correct connections and desired system functionality. At this stage, I am also working on optimizing the schematic and the Bill of Materials (BOM) for a smooth transition to PCB design later on.

# Week 3 - Schematic release

In the third week of the project, I completed the schematic design and released the finalized version. This involved thoroughly verifying all component connections and ensuring compatibility across the system, focusing on power distribution, signal integrity, and communication pathways between components. After careful checks, I created a preliminary Bill of Materials (BOM) and added detailed annotations to the schematic to guide the upcoming PCB layout phase. The release of the schematic marks a significant milestone, setting the foundation for the PCB design and allowing for initial testing.

[Schematic Revision 10](https://github.com/antoniosimuncic/VERN_Air-Quality-Meter/blob/main/Hardware/100008_hw-vern-aqm/Schematic/100008_hw-vern-aqm.pdf)



# Week 4 - PCB design  


