#ifndef __WIFIMODULE_H
#define __WIFIICEMODULE_H

#include <Arduino.h>

void connect_to_Wifi(String login, String password);
String getTime();
void sendRequest(String data, bool isMainRequest);

#endif