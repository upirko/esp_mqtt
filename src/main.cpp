#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <FastLED.h>
#include "settings.h"
#include "webserver.h"

WiFiClient espClient;
boolean ready = false;

#define NUM_LEDS 256
CRGB leds[NUM_LEDS];

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

  JsonArray pixs = data["data"];

  for (size_t i = 0; i < pixs.size(); i++) {
    JsonArray pix = pixs[i];
    CRGB color = CRGB(pix[0], pix[1], pix[2]);
    leds[i] = color;
  }
  FastLED.show();

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
  server.on("/data", HTTP_POST, handleData);
  
  FastLED.addLeds<WS2811, 2, RGB>(leds, NUM_LEDS);
}

void loop() {
  server.handleClient();
  if (!ready) {
    return;
  }
}
