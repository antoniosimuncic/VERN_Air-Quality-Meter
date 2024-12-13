import serial
import csv
import re
from datetime import datetime

# Function to parse data into a dictionary
def parse_data(data):
    parsed_data = {}
    
    # Debugging: Print raw data
    print("Parsing data:", data)

    # Parse SCD4x measurements
    scd4x_match = re.search(
        r"SCD4x Measurements:\nCO2: (\d+) ppm\nTemperature: ([\d.]+) °C\nHumidity: ([\d.]+) %",
        data,
    )
    if scd4x_match:
        parsed_data.update({
            "SCD4x_CO2_ppm": scd4x_match.group(1),
            "SCD4x_Temperature_C": scd4x_match.group(2),
            "SCD4x_Humidity_%": scd4x_match.group(3),
        })

    # Parse SEN5x measurements
    sen5x_match = re.search(
        r"SEN5x Measurements:\nPM 1\.0: ([\d.]+) µg/m³\nPM 2\.5: ([\d.]+) µg/m³\nPM 4\.0: ([\d.]+) µg/m³\nPM 10: ([\d.]+) µg/m³\nTemperature: ([\d.]+) °C\nHumidity: ([\d.]+) %\nVocIndex: ([\d.]+)\nNoxIndex: ([\w./]+)",
        data,
    )
    if sen5x_match:
        parsed_data.update({
            "SEN5x_PM1_ug_m3": sen5x_match.group(1),
            "SEN5x_PM2_5_ug_m3": sen5x_match.group(2),
            "SEN5x_PM4_ug_m3": sen5x_match.group(3),
            "SEN5x_PM10_ug_m3": sen5x_match.group(4),
            "SEN5x_Temperature_C": sen5x_match.group(5),
            "SEN5x_Humidity_%": sen5x_match.group(6),
            "SEN5x_VocIndex": sen5x_match.group(7),
            "SEN5x_NoxIndex": sen5x_match.group(8),
        })

    # Parse DPS368 measurements
    dps368_match = re.search(
        r"DPS368 Measurements:\nTemperature: ([\d.]+) °C\nPressure: ([\d.]+) Pa",
        data,
    )
    if dps368_match:
        parsed_data.update({
            "DPS368_Temperature_C": dps368_match.group(1),
            "DPS368_Pressure_Pa": dps368_match.group(2),
        })

    # Parse SHT4x measurements
    sht4x_match = re.search(
        r"SHT4x Measurements:\nTemperature: ([\d.]+) °C\nHumidity: ([\d.]+) %",
        data,
    )
    if sht4x_match:
        parsed_data.update({
            "SHT4x_Temperature_C": sht4x_match.group(1),
            "SHT4x_Humidity_%": sht4x_match.group(2),
        })

    return parsed_data

# Open the serial port
serial_port = 'COM6'
baud_rate = 115200  # Adjust as per your device
output_csv = 'sensor_data.csv'

try:
    ser = serial.Serial(serial_port, baud_rate, timeout=1)
    
    # Disable DTR and RTS lines
    ser.dtr = False
    ser.rts = False

    print(f"Listening to {serial_port} with DTR and RTS disabled...")

    # Open the CSV file
    with open(output_csv, mode='w', newline='') as file:
        csv_writer = csv.writer(file)

        # Write CSV header
        csv_writer.writerow([
            "Timestamp", "SCD4x_CO2_ppm", "SCD4x_Temperature_C", "SCD4x_Humidity_%",
            "SEN5x_PM1_ug_m3", "SEN5x_PM2_5_ug_m3", "SEN5x_PM4_ug_m3", "SEN5x_PM10_ug_m3",
            "SEN5x_Temperature_C", "SEN5x_Humidity_%", "SEN5x_VocIndex", "SEN5x_NoxIndex",
            "DPS368_Temperature_C", "DPS368_Pressure_Pa",
            "SHT4x_Temperature_C", "SHT4x_Humidity_%"
        ])

        while True:
            # Read data from serial port
            if ser.in_waiting > 0:
                line = ser.read(ser.in_waiting).decode('utf-8').strip()  # Read all available data
                print("Received:", line)

                # Parse the data
                parsed = parse_data(line)
                if parsed:
                    # Add timestamp
                    row = [datetime.now().isoformat()]

                    # Append parsed values or empty strings if not present
                    for key in [
                        "SCD4x_CO2_ppm", "SCD4x_Temperature_C", "SCD4x_Humidity_%",
                        "SEN5x_PM1_ug_m3", "SEN5x_PM2_5_ug_m3", "SEN5x_PM4_ug_m3", "SEN5x_PM10_ug_m3",
                        "SEN5x_Temperature_C", "SEN5x_Humidity_%", "SEN5x_VocIndex", "SEN5x_NoxIndex",
                        "DPS368_Temperature_C", "DPS368_Pressure_Pa",
                        "SHT4x_Temperature_C", "SHT4x_Humidity_%"
                    ]:
                        row.append(parsed.get(key, ""))

                    # Write to CSV
                    csv_writer.writerow(row)
                    print("Data written to CSV")
except serial.SerialException as e:
    print(f"Error: {e}")
except KeyboardInterrupt:
    print("Terminating...")
