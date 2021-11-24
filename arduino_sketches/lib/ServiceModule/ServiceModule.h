#ifndef __SERVICEMODULE_H
#define __SERVICEMODULE_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SensorsModule.h>

String getValue(String data, char separator, int index);
String getStringifiedData(SensorsData sensorsData, String serialNumber);

#endif