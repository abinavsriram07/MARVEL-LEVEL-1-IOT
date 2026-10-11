/*
  Task 3: Control the ESP32 onboard LED using MQTT.
  Install the PubSubClient library.
  Change Wi-Fi credentials and use a unique topic for your own testing.
  Default onboard LED pin is GPIO 2; this varies by ESP32 board.
*/

#include <WiFi.h>
#include <PubSubClient.h>

const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

const char* MQTT_BROKER = "broker.emqx.io";
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC = "marvel/task3b/esp32_demo_731"; // Change to a unique topic
const int LED_PIN = 2;

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message.trim();

  Serial.print("Message on topic ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(message);

  if (message == "LED 1 ON") {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Onboard LED turned ON.");
  } else if (message == "LED 1 OFF") {
    digitalWrite(LED_PIN, LOW);
    Serial.println("Onboard LED turned OFF.");
  } else {
    Serial.println("Unknown command. Use LED 1 ON or LED 1 OFF.");
  }
}

void connectMqtt() {
  while (!mqtt.connected()) {
    String clientId = "ESP32Client-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    Serial.print("Connecting to MQTT broker...");
    if (mqtt.connect(clientId.c_str())) {
      Serial.println("connected.");
      mqtt.subscribe(MQTT_TOPIC);
      Serial.print("Subscribed to: ");
      Serial.println(MQTT_TOPIC);
    } else {
      Serial.print("Failed, state=");
      Serial.print(mqtt.state());
      Serial.println(". Retrying in 3 seconds.");
      delay(3000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected.");

  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setCallback(mqttCallback);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
    delay(500);
    return;
  }
  if (!mqtt.connected()) {
    connectMqtt();
  }
  mqtt.loop();
}
