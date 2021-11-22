#ifndef __SERVICEMODULE_H
#define __SERVICEMODULE_H

#include <Arduino.h>
#include <ESP8266WiFi.h>

String getValue(String, char, int);
String getTime(WiFiClient);

#endif