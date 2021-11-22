#ifndef __SDCARDMODULE_H
#define __SDCARDMODULE_H
#include <SD.h>

void writeDataToSdCard(File, String);
struct WifiDataAndSerialNumber getDataFromSdCard(File);

#endif