#ifndef __SENSORSMODULE_H
#define __SENSORSMODULE_H

struct SensorsData {
  float humidity;
  float dhtTemp;
  float dallasTemp;
  float bmpTemp;
  float pressure;
  float voltage;
};

void setupSensors();
struct SensorsData readSensors();

#endif