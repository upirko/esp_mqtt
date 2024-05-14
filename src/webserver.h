#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <Arduino.h>
#include <ESP8266WebServer.h>

extern ESP8266WebServer server;

void setupWebServer();
void handleRoot();
void handleSave();

#endif
