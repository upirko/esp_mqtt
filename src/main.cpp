#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "settings.h"
#include "webserver.h"
#define PIN_1 5
#define PIN_2 4

WiFiClient espClient;
PubSubClient client(espClient);
boolean ready = false;
char actionTopic[32];
char statusTopic[32];

void setupMode() {
  Serial.println("Setup mode: Connect to WiFi setup network and visit IP address in browser.");
  WiFi.softAP("Device Setup");
}

bool connectToWiFi() {
  if (!settings.ssid || !settings.password) {
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

void callbackMqtt(char* topic, byte* payload, unsigned int length) {
  Serial.print(topic);
  Serial.print(": ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (strcmp(topic, actionTopic) == 0) {
    switch ((char)payload[0])
    {
    case '1':
      digitalWrite(PIN_1, (char)payload[1] == '1' ? HIGH : LOW);
      break;
    case '2':
      digitalWrite(PIN_2, (char)payload[1] == '1' ? HIGH : LOW);
      break;
    default:
      break;
    }
    char mess[50];
    snprintf(mess, sizeof(mess), "Door %c %c", (char)payload[0], (char)payload[1]);
    client.publish(statusTopic, mess);
  }
}

void reconnectMqtt() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(settings.device_name, settings.mqtt_user, settings.mqtt_password)) {
      Serial.println("connected");
      client.subscribe(actionTopic);
      digitalWrite(LED_BUILTIN, LOW);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_1, OUTPUT);
  pinMode(PIN_2, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(PIN_1, LOW);
  digitalWrite(PIN_2, LOW);
  readSettings();

  if (!connectToWiFi()) {
    Serial.println("Failed to connect to WiFi. Entering setup mode...");
    setupMode();
  } else {
    ready = true;
    snprintf(actionTopic, sizeof(actionTopic), "%s/action", settings.device_name);
    snprintf(statusTopic, sizeof(actionTopic), "%s/status", settings.device_name);
    client.setServer(settings.mqtt_server, 1883);
    client.setCallback(callbackMqtt);
    digitalWrite(LED_BUILTIN, LOW);
  }
  setupWebServer();
}

void loop() {
  server.handleClient();
  if (!ready) {
    return;
  }
  if (!client.connected()) {
    digitalWrite(LED_BUILTIN, HIGH);
    reconnectMqtt();
  }
  client.loop();
}
