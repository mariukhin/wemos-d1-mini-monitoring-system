#include <WiFiModule.h>
#include <LightSleepModule.h>
#include <ServiceModule.h>
#include <SDcardModule.h>
#include <SensorsModule.h>

#define SLEEP_INTERVAL 600 // s/10min

// WiFi default parameters
struct WifiDataAndSerialNumber wifiDataAndSerialNum = { "atep", "", "000000" };

// sensors data
struct SensorsData sensorsData;

void setup(void)
{ 
  Serial.begin(9600);
  delay(500);

  setupSensors();
}

void loop()
{
  wifiDataAndSerialNum = getDataFromSdCard();
  delay(2000);

  connectToWifi(wifiDataAndSerialNum.login, wifiDataAndSerialNum.password);  // initialize WiFi connection

  String postData = getStringifiedData(readSensors(), wifiDataAndSerialNum.serialNumber); // get data to send in stringified JSON format
  sendRequest(postData, true); // send HTTP POST-request

  sleep(SLEEP_INTERVAL * 1000); // light-sleep for 10min
}
