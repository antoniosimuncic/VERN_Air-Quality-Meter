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

In the second week of the project, I began creating the schematic in KiCad. The first task was to place all the selected components in the schematic, followed by a detailed study of the datasheets for the chosen components to ensure correct connections and desired system functionality. At this stage, I am also working on optimizing the schematic and the Bill of Materials (BOM) for a smooth transition to PCB design later on.

# Week 3 - Schematic release

In the third week of the project, I completed the schematic design and released the finalized version. This involved thoroughly verifying all component connections and ensuring compatibility across the system, focusing on power distribution, signal integrity, and communication pathways between components. After careful checks, I created a preliminary Bill of Materials (BOM) and added detailed annotations to the schematic to guide the upcoming PCB layout phase. The release of the schematic marks a significant milestone, setting the foundation for the PCB design and allowing for initial testing.

**Schematic:** [Revision 10](https://github.com/antoniosimuncic/VERN_Air-Quality-Meter/blob/main/Hardware/100008_hw-vern-aqm/Schematic/100008_hw-vern-aqm.pdf)

# Week 4 - PCB design

In the fourth week of the project, I began designing the PCB based on the finalized schematic. This phase involved selecting the appropriate board dimensions, component placement, and routing the connections while considering design rules such as trace width, clearance, and power and heat management. A key focus was on USB differential pair impedance control, ensuring that the USB signal traces were routed with the correct 90Ω differential impedance to maintain signal integrity and reliable data transfer. I also worked on optimizing the layout for signal integrity, minimizing the length of high-speed signal traces, and ensuring proper reference planes and return paths for the components. After completing the layout and routing, I performed a Design Rule Check (DRC) to identify and correct any potential issues before moving on to generating the Gerber files for fabrication.

**PCB render:**  

![PCB render photo](https://github.com/antoniosimuncic/VERN_Air-Quality-Meter/blob/main/Hardware/100008_hw-vern-aqm/Images/top-side-view.png)  

# Week 5 - Component procurement  

In the fifth week of the project, I focused on component procurement and ordering essential parts to prepare for assembly. I purchased components from Mouser to ensure the reliability of critical parts while sourcing additional items from Aliexpress for cost-effectiveness. Additionally, I ordered the PCBs and a stencil for solder application from JLCPCB. With components and PCBs on the way, the project is set to progress smoothly into the assembly phase once all parts arrive.

[Bill of Materials](https://html-preview.github.io/?url=https://github.com/antoniosimuncic/VERN_Air-Quality-Meter/blob/main/Hardware/100008_hw-vern-aqm/Manufacturing/Assembly/ibom/ibom.html)  

[Project expense report](https://github.com/antoniosimuncic/VERN_Air-Quality-Meter/blob/main/Documentation/project-expense-report.md)  

# Week 6 - PCB Assembly  

In the sixth week of the project, I began assembling the PCB as the components and boards had arrived. Using the stencil from JLCPCB, I applied solder paste to the pads, and then I carefully placed each component onto the board, paying special attention to orientation and alignment. After assembling the board, I placed the board on an SMD hotplate reflow soldering station to perform the reflow soldering process. After the board cooled down, I inspected the connections to ensure the soldering process finished successfully. With the PCB assembly complete, I began the first board power-up to do the initial testing and troubleshooting.  

**Production process:**  

![Photo of the production process](https://github.com/antoniosimuncic/VERN_Air-Quality-Meter/blob/main/Documentation/production-process.jpg)  

# Week 7 - Board Bring-Up  

In the seventh week of the project, I focused on the initial bring-up of the PCB. After assembling the board, I verified the power rails and ensured all components received the correct voltages. Basic functionality, including communication interfaces such as UART and I2C, was tested to confirm proper connectivity between the microcontroller and peripherals. Rigol DS1054z oscilloscope was used to identify and resolve minor issues on the UART lines. Soldering fixes were necessary to ensure the board operated as expected. This stage established a solid foundation for further firmware development and system testing.  

**Sensor test source code:** [sensors-bring-up.cpp](https://github.com/antoniosimuncic/VERN_Air-Quality-Meter/blob/main/Documentation/sensors-bring-up.cpp)  

# Week 8 - Case design  

In the eighth week of the project, I focused on designing a custom 3D-printed case for the device after I had confirmed the board functioned as expected. The design accounted for component placement, ventilation, and accessibility for ports and connectors. Aesthetics and durability were considered to ensure the case complemented the functionality and appearance of the project. This step helped prepare the device for practical use and testing in real-world conditions. The 3D print was made using PET-G on the BambuLab P1S printer.  

**Case design:**  

![Photo of the case design](https://github.com/antoniosimuncic/VERN_Air-Quality-Meter/blob/main/Documentation/case-design.jpg)  

# Week 9 - Firmware development - TODO  





