# Mini Portable Weather Station (ESP32)

## Introduction

This project is a compact and portable weather monitoring system built using the ESP32-WROOM-32U. It measures environmental parameters such as temperature, humidity, air quality, and dust levels, and displays them in real time on a TFT screen.

It also includes a simple method to estimate rain probability based on temperature and humidity.

This project helps in understanding embedded systems, sensor interfacing, and real-time data display.

---

## Objectives

- Build a low-cost portable weather system  
- Interface multiple sensors with ESP32  
- Display real-time data on TFT screen  
- Implement basic environmental analysis  

---

## Components Used

- ESP32-WROOM-32U  
- 2.4 inch TFT Display (ILI9341)  
- DHT22 Sensor  
- MQ135 Gas Sensor  
- GP2Y1010AU0F Dust Sensor  
- 18650 Battery  
- Resistors and Capacitors  
- Breadboard and Jumper Wires  

---

## Working Principle

The ESP32 reads data from multiple sensors:

- DHT22 provides temperature and humidity  
- MQ135 gives air quality values  
- Dust sensor measures smoke and particles  

The data is processed and displayed on the TFT screen.  
The display updates every 3 seconds.

Rain probability is calculated based on simple conditions:
- Higher humidity increases chance of rain  
- Lower temperature increases probability further  

---

## Hardware Connections

Refer to `PIN_DIAGRAM.md` for detailed pin connections.

---

## Software Requirements

- Arduino IDE  
- ESP32 Board Package  

### Required Libraries

- DHT sensor library  
- Adafruit GFX  
- Adafruit ILI9341  

---

## Installation Steps

1. Install Arduino IDE  

2. Add ESP32 board support:
   File → Preferences → Additional Boards Manager URL:
   https://dl.espressif.com/dl/package_esp32_index.json  

3. Install ESP32 from Boards Manager  

4. Install required libraries  

5. Open `WeatherStation.ino`  

6. Select board:
   ESP32 Dev Module  

7. Upload the code  

---
