#ifndef __WIFIMODULE_H
#define __WIFIICEMODULE_H

#include <Arduino.h>

void connectToWifi(String login, String password);
String getTime();
void sendRequest(String data, bool isMainRequest);

#endif