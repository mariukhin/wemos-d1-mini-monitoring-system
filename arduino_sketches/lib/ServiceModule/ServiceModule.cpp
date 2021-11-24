#include <Arduino.h>
#include <Arduino_JSON.h>
#include <ESP8266HTTPClient.h>
#include "ServiceModule.h"
#include "SensorsModule.h"
#include "WiFiModule.h"

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

String getStringifiedData(SensorsData sensorsData, String serialNumber)
{
  JSONVar jsonData;
  jsonData["humidity"] = (int)sensorsData.humidity;
  jsonData["temp"] = sensorsData.dallasTemp;
  jsonData["temp_dht"] = sensorsData.dhtTemp;
  jsonData["temp_bpm"] = sensorsData.bmpTemp;
  jsonData["pressure"] = sensorsData.pressure;
  jsonData["voltage"] = sensorsData.voltage;
  jsonData["time"] = getTime();
  jsonData["device_number"] = serialNumber;

  return JSON.stringify(jsonData);
}