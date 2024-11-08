#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include "settings.h"
#include "webserver.h"

WiFiClient espClient;
Adafruit_NeoPixel strip;
boolean ready = false;

void setupMode() {
  Serial.println("Setup mode: Connect to WiFi setup network and visit IP address in browser.");
  WiFi.softAP("Device Setup");
}

bool connectToWiFi() {
  if (!settings.ssid || !settings.password)
  {
    return false;
  }
  Serial.print("Connecting to ");
  Serial.println(settings.ssid);

  WiFi.begin(settings.ssid, settings.password);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    return true;
  } else {
    return false;
  }
}

// unsigned long hexToDec(const char* hex) {
//   if (hex[0] == '#') {
//     hex++;
//   }
//   return strtoul(hex, NULL, 16);
// }

void handleData() {
  if (!server.hasArg("plain")) {
    server.send (400, "application/json", "{\"success\":false}");
    return;
  }

  JsonDocument data;
  DeserializationError error = deserializeJson(data, server.arg("plain"));

  if (error) {
    Serial.print("Ошибка парсинга JSON: ");
    Serial.println(error.f_str());
    server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }

  // strip.setPixelColor(data["index"], hexToDec(data["color"]));
  // strip.show();
  server.send ( 200, "text/json", "{\"success\":true}" );
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  
  digitalWrite(LED_BUILTIN, HIGH);
  readSettings();
  
  if (!connectToWiFi()) {
    Serial.println("Failed to connect to WiFi. Entering setup mode...");
    setupMode();
  } else {
    ready = true;
    digitalWrite(LED_BUILTIN, LOW);
  }

  setupWebServer();
  if (ready) {
    server.on("/data", HTTP_POST, handleData);
  }

  strip = Adafruit_NeoPixel(settings.led_count, settings.work_pin, NEO_GRB + NEO_KHZ800);
  strip.begin(); 
  strip.setBrightness(50);
  strip.show();
}

void loop() {
  server.handleClient();
  if (!ready) {
    return;
  }
}
