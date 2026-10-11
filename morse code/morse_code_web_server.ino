/*
  MARVEL Task 6: ESP32 Web-Based Morse Code Generator
  ---------------------------------------------------
  Enter text in a browser. The ESP32 converts letters and digits
  into Morse code and flashes the onboard LED.

  Before uploading:
  1. Replace YOUR_WIFI_NAME and YOUR_WIFI_PASSWORD.
  2. Select your ESP32 board and correct COM/serial port in Arduino IDE.
  3. The default onboard LED pin is GPIO 2; change LED_PIN if your
     particular ESP32 board uses a different pin.

  Supported input: A-Z, 0-9, and spaces.
*/

#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

const int LED_PIN = 2;
WebServer server(80);

const unsigned int DOT_TIME = 250;
const unsigned int DASH_TIME = 750;
const unsigned int SYMBOL_GAP = 250;
const unsigned int LETTER_GAP = 750;
const unsigned int WORD_GAP = 1750;

const char* letters[26] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.",
  "....", "..", ".---", "-.-", ".-..", "--", "-.",
  "---", ".--.", "--.-", ".-.", "...", "-", "..-",
  "...-", ".--", "-..-", "-.--", "--.."
};

const char* numbers[10] = {
  "-----", ".----", "..---", "...--", "....-",
  ".....", "-....", "--...", "---..", "----."
};

// Basic HTML escaping prevents entered text from being interpreted as HTML.
String escapeHtml(const String& input) {
  String output;
  for (unsigned int i = 0; i < input.length(); i++) {
    char c = input[i];
    if (c == '&') output += "&amp;";
    else if (c == '<') output += "&lt;";
    else if (c == '>') output += "&gt;";
    else if (c == '"') output += "&quot;";
    else if (c == '\'') output += "&#39;";
    else output += c;
  }
  return output;
}

String convertToMorse(String text) {
  text.toUpperCase();
  String morse = "";
  bool previousWasSpace = true;

  for (unsigned int i = 0; i < text.length(); i++) {
    char c = text[i];

    if (c >= 'A' && c <= 'Z') {
      if (morse.length() > 0 && !previousWasSpace) morse += " ";
      morse += letters[c - 'A'];
      previousWasSpace = false;
    } else if (c >= '0' && c <= '9') {
      if (morse.length() > 0 && !previousWasSpace) morse += " ";
      morse += numbers[c - '0'];
      previousWasSpace = false;
    } else if (c == ' ') {
      if (morse.length() > 0 && morse[morse.length() - 1] != '/') {
        morse += " /";
      }
      previousWasSpace = true;
    }
    // Other punctuation is ignored.
  }

  return morse;
}

void dot() {
  digitalWrite(LED_PIN, HIGH);
  delay(DOT_TIME);
  digitalWrite(LED_PIN, LOW);
  delay(SYMBOL_GAP);
}

void dash() {
  digitalWrite(LED_PIN, HIGH);
  delay(DASH_TIME);
  digitalWrite(LED_PIN, LOW);
  delay(SYMBOL_GAP);
}

void flashMorse(const String& morse) {
  for (unsigned int i = 0; i < morse.length(); i++) {
    char symbol = morse[i];

    if (symbol == '.') {
      dot();
    } else if (symbol == '-') {
      dash();
    } else if (symbol == ' ') {
      delay(LETTER_GAP > SYMBOL_GAP ? LETTER_GAP - SYMBOL_GAP : 0);
    } else if (symbol == '/') {
      delay(WORD_GAP > LETTER_GAP ? WORD_GAP - LETTER_GAP : 0);
    }
  }

  digitalWrite(LED_PIN, LOW);
}

String makePage(const String& message = "",
                const String& morse = "",
                const String& status = "Ready") {
  String html = R"HTML(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 Morse Code Generator</title>
  <style>
    body{font-family:Arial,sans-serif;background:#101820;color:#fff;text-align:center;padding:24px 12px}
    .box{max-width:600px;margin:20px auto;padding:22px;background:#202b36;border-radius:12px}
    input{width:90%;max-width:420px;box-sizing:border-box;padding:12px;font-size:17px;border-radius:6px;border:0}
    button{margin-top:15px;padding:12px 22px;background:#00a86b;color:white;font-size:16px;border:0;border-radius:6px;cursor:pointer}
    .morse{overflow-wrap:anywhere;font-size:20px;line-height:1.7}
    .hint{color:#c4d0dc;font-size:14px}
  </style>
</head>
<body>
  <h1>ESP32 Morse Code Generator</h1>
  <div class="box">
    <form action="/send" method="GET">
      <input type="text" name="text" maxlength="80"
        placeholder="Enter a message (A-Z, 0-9)" required>
      <br><button type="submit">Flash Morse Code</button>
    </form>
    <p class="hint">Supported characters: A-Z, digits 0-9 and spaces.</p>
    <h3>Status</h3><p>)HTML";
  html += escapeHtml(status);
  html += R"HTML(</p><h3>Message</h3><p>)HTML";
  html += escapeHtml(message);
  html += R"HTML(</p><h3>Generated Morse Code</h3><p class="morse">)HTML";
  html += escapeHtml(morse);
  html += R"HTML(</p></div></body></html>)HTML";
  return html;
}

void handleRoot() {
  server.send(200, "text/html; charset=utf-8", makePage());
}

void handleSend() {
  String message = server.arg("text");
  message.trim();

  if (message.length() == 0) {
    server.send(400, "text/html; charset=utf-8",
                makePage("", "", "Please enter a message."));
    return;
  }

  if (message.length() > 80) {
    server.send(400, "text/plain", "Message is too long. Maximum: 80 characters.");
    return;
  }

  String morse = convertToMorse(message);
  if (morse.length() == 0) {
    server.send(400, "text/html; charset=utf-8",
                makePage(message, "", "Please enter letters or digits."));
    return;
  }

  // The browser receives the response before the LED starts flashing.
  server.send(200, "text/html; charset=utf-8",
              makePage(message, morse, "Flashing Morse code..."));

  Serial.println("\n--- Morse transmission ---");
  Serial.print("Message: ");
  Serial.println(message);
  Serial.print("Morse code: ");
  Serial.println(morse);

  flashMorse(morse);
  Serial.println("Morse transmission complete.");
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);
  delay(500);
  Serial.println("\nMARVEL Task 6 - ESP32 Morse Code Generator");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 30000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi connection failed. Check credentials and 2.4 GHz Wi-Fi.");
    return;
  }

  Serial.println("Wi-Fi connected.");
  Serial.print("Open http://");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/send", HTTP_GET, handleSend);
  server.onNotFound([]() {
    server.send(404, "text/plain", "Page not found.");
  });
  server.begin();
  Serial.println("Web server started.");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    server.handleClient();
  }
}
