#ifndef __SDCARDMODULE_H
#define __SDCARDMODULE_H

struct WifiDataAndSerialNumber {
  String login;
  String password;
  String serialNumber;
};

void writeDataToSdCard(String postData);
void sendParsedDataToNodeRed();
struct WifiDataAndSerialNumber getDataFromSdCard();

#endif