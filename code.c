#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "DHT.h"

// -------------------- Pin Definitions --------------------

// TFT Display
#define TFT_CS    5
#define TFT_DC    2
#define TFT_RST   4

// DHT Sensor
#define DHTPIN    15
#define DHTTYPE   DHT22

// MQ135 Air Quality
#define MQ135_PIN 35

// Dust Sensor (GP2Y1010)
#define DUST_LED     12
#define DUST_ANALOG  34

// -------------------- Objects --------------------

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
DHT dht(DHTPIN, DHTTYPE);

// -------------------- Variables --------------------

unsigned long lastUpdate = 0;
const int updateInterval = 3000;

// -------------------- Setup --------------------

void setup() {
  Serial.begin(115200);

  dht.begin();

  pinMode(DUST_LED, OUTPUT);
  digitalWrite(DUST_LED, HIGH);

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);

  drawUI();
}

// -------------------- UI Layout --------------------

void drawUI() {
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(30, 10);
  tft.print("Weather Station");

  tft.drawLine(0, 35, 320, 35, ILI9341_WHITE);

  tft.setTextSize(2);

  tft.setCursor(10, 50);  tft.print("Temp:");
  tft.setCursor(10, 80);  tft.print("Hum:");
  tft.setCursor(10, 110); tft.print("Air:");
  tft.setCursor(10, 140); tft.print("Dust:");
  tft.setCursor(10, 170); tft.print("Rain:");
}

// -------------------- Dust Sensor --------------------

float readDustDensity() {
  float total = 0;

  for (int i = 0; i < 10; i++) {
    digitalWrite(DUST_LED, LOW);
    delayMicroseconds(280);

    total += analogRead(DUST_ANALOG);

    delayMicroseconds(40);
    digitalWrite(DUST_LED, HIGH);
    delayMicroseconds(9680);
  }

  float avg = total / 10.0;
  float voltage = avg * (3.3 / 4095.0);

  float dustDensity = (voltage - 0.6) / 0.005;

  if (dustDensity < 0) dustDensity = 0;

  return dustDensity;
}

// -------------------- Rain Prediction --------------------

int calculateRain(float temp, float hum) {
  int chance = 0;

  if (hum > 80)       chance = 70;
  else if (hum > 65)  chance = 40;
  else                chance = 20;

  if (temp < 20) chance += 20;

  return constrain(chance, 0, 100);
}

// -------------------- Display Data --------------------

void displayData(float temp, float hum, int air, float dust, int rain) {

  tft.fillRect(120, 50, 180, 140, ILI9341_BLACK);

  tft.setTextSize(2);

  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor(120, 50);
  tft.print(temp, 1); tft.print(" C");

  tft.setTextColor(ILI9341_CYAN);
  tft.setCursor(120, 80);
  tft.print(hum, 1); tft.print(" %");

  tft.setTextColor(ILI9341_GREEN);
  tft.setCursor(120, 110);
  tft.print(air); tft.print(" ppm");

  tft.setTextColor(ILI9341_ORANGE);
  tft.setCursor(120, 140);
  tft.print(dust, 0); tft.print(" ug");

  tft.setTextColor(ILI9341_MAGENTA);
  tft.setCursor(120, 170);
  tft.print(rain); tft.print(" %");
}

// -------------------- Main Loop --------------------

void loop() {

  if (millis() - lastUpdate > updateInterval) {
    lastUpdate = millis();

    // Read DHT
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      temperature = 0;
      humidity = 0;
    }

    // Air Quality
    int airQuality = analogRead(MQ135_PIN);
    airQuality = map(airQuality, 0, 4095, 0, 500);

    // Dust
    float dustDensity = readDustDensity();

    // Rain
    int rainChance = calculateRain(temperature, humidity);

    // Display
    displayData(temperature, humidity, airQuality, dustDensity, rainChance);

    // Serial Output
    Serial.print("Temp: "); Serial.print(temperature);
    Serial.print(" C | Hum: "); Serial.print(humidity);
    Serial.print(" % | Air: "); Serial.print(airQuality);
    Serial.print(" ppm | Dust: "); Serial.print(dustDensity);
    Serial.print(" ug | Rain: "); Serial.print(rainChance);
    Serial.println(" %");
  }
}