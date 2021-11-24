#include <Arduino.h>
#include <Arduino_JSON.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h> 
#include "WiFiModule.h"
#include "SDcardModule.h"

WiFiClient client;

// Hosting data
const char* host = "192.168.0.200:1880"; // computer IP
String url = "/update-sensors"; 
const int httpPort = 80;

void connect_to_Wifi(String login, String password)
{
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.begin(login, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println();
  }
}


String getTime()
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

void sendRequest(String data, bool isMainRequest = false)
{
  Serial.print("Requesting URL: "); 
  Serial.println(url); //Post Data
  
  String address = host + url; 
  Serial.println(address);
  Serial.println(data);
  HTTPClient http; 
  String fullUrl = "http://" + address;
  http.begin(client, fullUrl); 
  http.addHeader("Content-Type", "application/json");
  auto httpCode = http.POST(data); 
  
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      Serial.println("Data was successfully sended!");
    } else {
      Serial.println("Failed to send data");
      writeDataToSdCard(data);
    }
  } else {
    Serial.println("Failed to connect to Node-Red");
    writeDataToSdCard(data);
    return;
  }

  http.end(); //Close connection 
  Serial.println("closing connection");

  if(isMainRequest){
    sendParsedDataToNodeRed();
  }
}