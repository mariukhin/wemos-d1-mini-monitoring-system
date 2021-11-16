#include <Arduino.h>
#include <Arduino_JSON.h>
#include <ESP8266HTTPClient.h>
#include "ServiceModule.h"

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

String getTime(WiFiClient client)
{
  HTTPClient http;

  String url = "http://worldtimeapi.org/api/timezone/Europe/Kiev";
  http.begin(client, url);
  auto result = http.GET();
  String payload = "{}";
  delay(1000);

  if (result > 0) {
    Serial.print(result);
    payload = http.getString();
  } else {
    Serial.println("Failed to get date");
  }
  http.end(); //Close connection

  JSONVar myObject = JSON.parse(payload);
  String time = (const char*) myObject["utc_datetime"];
  return time;
}