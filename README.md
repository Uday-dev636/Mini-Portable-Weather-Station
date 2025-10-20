# 🌦️ Mini Portable Weather Station (ESP32)

A compact and portable **weather station** built using an **ESP32-WROOM-32U** that measures **temperature, humidity, air quality, and dust density**, and even predicts **rain chances** — all displayed live on a **2.4” ILI9341 TFT screen**.

---

## 🧭 Overview

The weather changes constantly — sunny one moment, raining the next.  
This project helps track those changes accurately with a small, battery-powered, and display-equipped weather station.

It integrates multiple sensors with the ESP32 to measure and display:
- 🌡️ **Temperature & Humidity (DHT22)**
- 💨 **Air Quality (MQ135)**
- 🌫️ **Dust/Smoke Density (GP2Y1010AU0F)**
- ☔ **Rain Prediction (based on humidity & temperature logic)**

All readings are updated live on the TFT display every few seconds.

---

## 🧰 Components Required

| Component | Purpose |
|------------|----------|
| **ESP32-WROOM-32U** | Main controller |
| **2.4” TFT Display (ILI9341)** | Display readings |
| **DHT22** | Temperature + Humidity |
| **GP2Y1010AU0F** | Dust / Smoke sensor |
| **MQ135** | Air quality sensor (CO₂, NH₃, etc.) |
| **Resistors & Capacitors** | For stability |
| **18650 Battery** | Power source |
| **Jumper wires & Breadboard** | Connections |

---

## ⚙️ Hardware Connections

### 📺 TFT Display (ILI9341 SPI)

| TFT Pin | ESP32 Pin | Function |
|----------|------------|----------|
| VCC | 3.3V | Power |
| GND | GND | Ground |
| CS | GPIO5 | Chip Select |
| RESET | GPIO4 | Reset |
| DC (A0) | GPIO2 | Data/Command |
| SDI (MOSI) | GPIO23 | SPI Data |
| SCK | GPIO18 | SPI Clock |
| LED | 3.3V (through 100Ω resistor) | Backlight |
| SDO (MISO) | GPIO19 | Optional |

---

### 🌡️ DHT22 (Temperature + Humidity)

| Pin | ESP32 Pin | Notes |
|-----|------------|-------|
| VCC | 3.3V | Power |
| DATA | GPIO15 | Data (with 10kΩ pull-up resistor) |
| GND | GND | Ground |

---

### 🌫️ GP2Y1010AU0F (Dust / Smoke Sensor)

| Pin | ESP32 Pin | Notes |
|------|------------|-------|
| VLED | 5V | LED Power |
| LED-GND | GND | LED Ground |
| LED | GPIO13 | LED Control (via 220Ω resistor) |
| Vo | GPIO34 | Analog Output |
| GND | GND | Common Ground |
| VCC | 5V | Power |

---

### 💨 MQ135 (Air Quality Sensor)

| Pin | ESP32 Pin | Notes |
|------|------------|-------|
| VCC | 5V | Power |
| GND | GND | Ground |
| AOUT | GPIO35 | Analog Signal |
| DOUT | — | Not Used |

---

## 💻 Software Setup

### 1️⃣ Install Arduino IDE
Download from [Arduino.cc](https://www.arduino.cc/en/software) and install.

### 2️⃣ Add ESP32 Board Support
In **File → Preferences**, paste this into “Additional Boards Manager URLs”: https://dl.espressif.com/dl/package_esp32_index.json

Then go to **Tools → Board → Boards Manager**, search for “ESP32”, and install.

Select **ESP32 Dev Module** under Tools → Board.

### 3️⃣ Install Required Libraries
Go to **Sketch → Include Library → Manage Libraries**, install:
- DHT sensor library (for DHT22)
- Adafruit ILI9341
- Adafruit GFX
- MQ135 (or use analog readings)
- GP2Y1010 (optional)

---

## 📟 Complete Code

Save this as `WeatherStation.ino` inside a folder named `WeatherStation/`.

```cpp
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "DHT.h"

// === Pin Configurations ===
#define TFT_CS   5
#define TFT_DC   2
#define TFT_RST  4
#define DHTPIN   15
#define DHTTYPE  DHT22
#define MQ135_PIN 34

// === GP2Y1010 Dust Sensor Pins ===
#define DUST_LED 12
#define DUST_ANALOG 35

// === Object Initialization ===
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastUpdate = 0;

void setup() {
  Serial.begin(115200);
  
  // Initialize sensors
  dht.begin();
  pinMode(DUST_LED, OUTPUT);
  digitalWrite(DUST_LED, LOW);
  
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
  drawStaticUI();
}

void drawStaticUI() {
  // Header - Centered
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(35, 15);
  tft.print("Weather Station");
  
  // Divider line
  tft.drawLine(0, 40, 320, 40, ILI9341_WHITE);
  
  // Labels
  tft.setTextSize(1.5);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(30, 55); tft.print("Temperature:");
  tft.setCursor(30, 75); tft.print("Humidity:");
  tft.setCursor(30, 95); tft.print("Air Quality:");
  tft.setCursor(30, 115); tft.print("Dust:");
  tft.setCursor(30, 135); tft.print("Rain:");
}

float readDustDensity() {
  float voMeasured = 0;
  for (int i = 0; i < 10; i++) {
    digitalWrite(DUST_LED, LOW);
    delayMicroseconds(280);
    voMeasured += analogRead(DUST_ANALOG);
    delayMicroseconds(40);
    digitalWrite(DUST_LED, HIGH);
    delayMicroseconds(9680);
    delay(50);
  }
  voMeasured /= 10;
  float voltage = voMeasured * (3.3 / 4095.0);
  float dustDensity = (voltage - 0.6) / 0.005;
  if (dustDensity < 0) dustDensity = 0;
  return dustDensity;
}

int calculateRainChance(float temp, float hum) {
  int chance = 0;
  if (hum > 80) chance = 70;
  else if (hum > 65) chance = 40;
  else chance = 20;
  if (temp < 20) chance += 20;
  return constrain(chance, 0, 100);
}

void displayData(float temp, float hum, float air, float dust, int rain) {
  tft.fillRect(120, 55, 150, 90, ILI9341_BLACK);
  tft.setTextSize(1.5);

  tft.setTextColor(ILI9341_YELLOW); tft.setCursor(120, 55);
  tft.print(temp, 1); tft.print("C");

  tft.setTextColor(ILI9341_CYAN); tft.setCursor(120, 75);
  tft.print(hum, 1); tft.print("%");

  tft.setTextColor(ILI9341_GREEN); tft.setCursor(120, 95);
  tft.print(air, 0); tft.print("ppm");

  tft.setTextColor(ILI9341_ORANGE); tft.setCursor(120, 115);
  tft.print(dust, 0); tft.print("ug");

  tft.setTextColor(ILI9341_MAGENTA); tft.setCursor(120, 135);
  tft.print(rain); tft.print("%");
}

void loop() {
  if (millis() - lastUpdate > 3000) {
    lastUpdate = millis();

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    if (isnan(temperature) || isnan(humidity)) {
      temperature = 0; humidity = 0;
    }

    int airQuality = analogRead(MQ135_PIN);
    airQuality = map(airQuality, 0, 4095, 0, 500);

    float dustDensity = readDustDensity();
    int rainChance = calculateRainChance(temperature, humidity);

    displayData(temperature, humidity, airQuality, dustDensity, rainChance);

    Serial.printf("Temp: %.1fC | Hum: %.1f%% | Air: %dppm | Dust: %.0fug | Rain: %d%%\n",
                  temperature, humidity, airQuality, dustDensity, rainChance);
  }
}

