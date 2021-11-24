#ifndef __SDCARDMODULE_H
#define __SDCARDMODULE_H
#include <SD.h>

struct WifiDataAndSerialNumber {
  String login;
  String password;
  String serialNumber;
};

void writeDataToSdCard(File, String);
struct WifiDataAndSerialNumber getDataFromSdCard(File);

#endif