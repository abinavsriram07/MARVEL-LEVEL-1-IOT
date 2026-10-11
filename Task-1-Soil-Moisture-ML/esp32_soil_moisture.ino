/*
  Task 1: Soil moisture monitoring with ThingSpeak
  Board: ESP32
  Wiring: sensor VCC -> 3V3, GND -> GND, AO -> GPIO34

  Field 1 = simulated temperature (no DHT11 attached)
  Field 2 = simulated humidity (no DHT11 attached)
  Field 3 = estimated soil moisture from the analog sensor

  Replace all placeholders before use.
*/

#include <WiFi.h>
#include <ThingSpeak.h>

const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

unsigned long CHANNEL_ID = 1234567;  // Replace with your ThingSpeak Channel ID
const char* WRITE_API_KEY = "YOUR_THINGSPEAK_WRITE_API_KEY";

const int SOIL_PIN = 34;

// Calibration examples only. Read your sensor in dry air/soil and wet soil,
// then replace these values with your own readings.
const int DRY_RAW = 3200;
const int WET_RAW = 1400;

WiFiClient client;

float estimateSoilMoisture(int rawValue) {
  float percent = (float)(DRY_RAW - rawValue) * 100.0 / (DRY_RAW - WET_RAW);
  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;
  return percent;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(SOIL_PIN, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  ThingSpeak.begin(client);
  randomSeed(analogRead(SOIL_PIN));
}

void loop() {
  int rawSoil = analogRead(SOIL_PIN);
  float soilMoisture = estimateSoilMoisture(rawSoil);

  // Simulated values because a DHT11 sensor is not attached.
  float simulatedTemperature = random(250, 311) / 10.0;  // 25.0–31.0 C
  float simulatedHumidity = random(550, 801) / 10.0;     // 55.0–80.0 %

  Serial.println("----- Sensor update -----");
  Serial.print("Raw soil reading: ");
  Serial.println(rawSoil);
  Serial.print("Estimated soil moisture: ");
  Serial.print(soilMoisture, 1);
  Serial.println("%");
  Serial.print("Simulated temperature: ");
  Serial.print(simulatedTemperature, 1);
  Serial.println(" C");
  Serial.print("Simulated humidity: ");
  Serial.print(simulatedHumidity, 1);
  Serial.println("%");

  ThingSpeak.setField(1, simulatedTemperature);
  ThingSpeak.setField(2, simulatedHumidity);
  ThingSpeak.setField(3, soilMoisture);

  int response = ThingSpeak.writeFields(CHANNEL_ID, WRITE_API_KEY);
  if (response == 200) {
    Serial.println("ThingSpeak update successful.");
  } else {
    Serial.print("ThingSpeak update failed. HTTP response: ");
    Serial.println(response);
  }

  // ThingSpeak free accounts generally require at least 15 seconds between updates.
  delay(20000);
}
