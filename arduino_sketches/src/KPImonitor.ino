#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h> 
#include <SD.h>
#include <Arduino_JSON.h>
#include <LightSleepModule.h>
#include <ServiceModule.h>
#include <SDcardModule.h>
#include <SensorsModule.h>

#define SLEEP_INTERVAL 600 // s/10min

WiFiClient client;

File myFile;

// WiFi default parameters
struct WifiDataAndSerialNumber wifiDataAndSerialNum;

// sensors data
struct SensorsData sensorsData;

// Hosting data
const char* host = "192.168.0.200:1880"; // computer IP
String url = "/update-sensors"; 
const int httpPort = 80;

void send_request(String data);
void sendParsedDataToNodeRed()
{
  String parsedData;

  // re-open the file for reading:
  myFile = SD.open("data.txt");
  if (myFile) {
    while (myFile.available()) {
      char data = myFile.read();
      parsedData.concat(data);
    }
    // close the file:
    myFile.close();

    for (int i = 0; i < parsedData.length(); i++)
    {
      String item = getValue(parsedData, '\n', i);
      if (item.equals("")) {
        break;
      } else {
        send_request(item);
        delay(5000);
      }
    }
    SD.remove("data.txt");
  } else {
    Serial.println("error opening data.txt");
  }
}

void connect_to_Wifi()
{
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiDataAndSerialNum.login, wifiDataAndSerialNum.password);

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

void send_request(String data = "0")
{
  String postData;
  Serial.print("Requesting URL: "); 
  Serial.println(url); //Post Data
  String time = getTime(client);

  if (data != "0") {
    postData = data;
  } else {
    JSONVar jsonData;
    jsonData["humidity"] = (int)sensorsData.humidity;
    jsonData["temp"] = sensorsData.dallasTemp;
    jsonData["temp_dht"] = sensorsData.dhtTemp;
    jsonData["temp_bpm"] = sensorsData.bmpTemp;
    jsonData["pressure"] = sensorsData.pressure;
    jsonData["voltage"] = sensorsData.voltage;
    jsonData["time"] = time;
    jsonData["device_number"] = wifiDataAndSerialNum.serialNumber;
    postData = JSON.stringify(jsonData);
  }
  
  String address = host + url; 
  Serial.println(address);
  Serial.println(postData);
  HTTPClient http; 
  String fullUrl = "http://" + address;
  http.begin(client, fullUrl); 
  http.addHeader("Content-Type", "application/json");
  auto httpCode = http.POST(postData); 
  
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      Serial.println("Data was successfully sended!");

      if(data == "0"){
        sendParsedDataToNodeRed();
      }
    } else {
      Serial.println("Failed to send data");
      writeDataToSdCard(myFile, postData);
    }
  } else {
    Serial.println("Failed to connect to Node-Red");
    writeDataToSdCard(myFile, postData);
    return;
  }

  http.end(); //Close connection 
  Serial.println("closing connection");
}

void setup(void)
{ 
  Serial.begin(9600);
  delay(500);

  setupSensors();
}

void loop(){
  wifiDataAndSerialNum = getDataFromSdCard(myFile);
  delay(2000);

  connect_to_Wifi();  // initialize WiFi connection

  sensorsData = readSensors(); // read sensors data
  send_request(); // send HTTP POST-request

  sleep(SLEEP_INTERVAL * 1000); // light-sleep for 10min
}
