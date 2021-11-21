#ifndef __SDCARDMODULE_H
#define __SDCARDMODULE_H
#include <SD.h>

void initializeSdCard();
void writeDataToSdCard(File myFile, String postData);
struct WifiDataAndSerialNumber getDataFromSdCard(File myFile);

#endif