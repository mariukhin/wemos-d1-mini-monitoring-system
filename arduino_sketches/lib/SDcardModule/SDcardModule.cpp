#include <SD.h>
#include <Arduino.h>
#include "SDcardModule.h"
#include "ServiceModule.h"

const int chipSelect = D8;
char separator = ':';
String wifiInfo;

struct WifiDataAndSerialNumber {
  String login;
  String password;
  String serialNumber;
};

void initializeSdCard()
{
  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("initialization done.");
}

void writeDataToSdCard(File myFile, String postData)
{
  myFile = SD.open("data.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to data.txt...");
    myFile.println(postData);
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening wifi.txt");
  }
}

struct WifiDataAndSerialNumber getDataFromSdCard(File myFile)
{
  initializeSdCard();

  struct WifiDataAndSerialNumber wifiData;

  // re-open the file for reading:
  myFile = SD.open("wifi.txt");
  if (myFile) {
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      char data = myFile.read();
      wifiInfo.concat(data);
    }
    // close the file:
    myFile.close();
    String login = getValue(wifiInfo, separator, 1);
    login.trim();
    String logVal = getValue(login, '\n', 0);
    logVal.trim();
    wifiData.login = logVal;
    String pass = getValue(wifiInfo, separator, 2);
    pass.trim();
    String passVal = getValue(pass, '\n', 0);
    passVal.trim();
    wifiData.password = passVal;
    String ser = getValue(wifiInfo, separator, 3);
    ser.trim();
    wifiData.serialNumber = ser;
    return wifiData;
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening wifi.txt");
    return wifiData;
  }
}