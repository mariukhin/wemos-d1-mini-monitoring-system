#ifndef __SERVICEMODULE_H
#define __SERVICEMODULE_H

#include <Arduino.h>
#include <ESP8266WiFi.h>

String getValue(String data, char separator, int index);
String getTime(WiFiClient client);

#endif