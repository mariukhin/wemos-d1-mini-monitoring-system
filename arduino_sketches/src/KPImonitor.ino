#include <WiFiModule.h>
#include <LightSleepModule.h>
#include <ServiceModule.h>
#include <SDcardModule.h>
#include <SensorsModule.h>

#define SLEEP_INTERVAL 600 // s/10min

// WiFi default parameters
struct WifiDataAndSerialNumber wifiDataAndSerialNum = { "", "", "000000" };

// sensors data
struct SensorsData sensorsData;
boolean isSerialPort = false;
boolean isWifiConnected = false;
int val;

void setup(void)
{ 
  Serial.begin(9600);
  delay(500);

  setupSensors();
}

void loop()
{
  if (!isSerialPort) {
    wifiDataAndSerialNum = getDataFromSdCard();
    delay(2000);

    Serial.println(wifiDataAndSerialNum.login);
    Serial.println(wifiDataAndSerialNum.password);

    isWifiConnected = connectToWifi(wifiDataAndSerialNum.login, wifiDataAndSerialNum.password);  // initialize WiFi connection

    if (!isWifiConnected) {
      wifiDataAndSerialNum = { "", "", "" };
    }
  }

  if (isWifiConnected) {
    String postData = getStringifiedData(readSensors(), wifiDataAndSerialNum.serialNumber); // get data to send in stringified JSON format
    sendRequest(postData, true); // send HTTP POST-request

    sleep(SLEEP_INTERVAL * 1000); // light-sleep for 10min
  } else {
    isSerialPort = true;

    while (Serial.available() > 0) {}
    wifiDataAndSerialNum.login = Serial.readStringUntil('\n');
    Serial.print(wifiDataAndSerialNum.login);
    
    while (Serial.available() > 0) {}
    wifiDataAndSerialNum.password = Serial.readStringUntil('\n');
    Serial.print(wifiDataAndSerialNum.password);
    isSerialPort = false;
  }
}
