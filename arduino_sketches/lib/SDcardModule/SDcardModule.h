#ifndef __SDCARDMODULE_H
#define __SDCARDMODULE_H
#include <SD.h>

struct WifiDataAndSerialNumber {
  String login;
  String password;
  String serialNumber;
};

void initializeSdCard();
void writeDataToSdCard(File myFile, String postData);
struct WifiDataAndSerialNumber getDataFromSdCard(File myFile)

#endif