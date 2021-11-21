#include <DHT.h> // For temperature and humidity sensor 
#include <DHT_U.h>  // For temperature and humidity sensor 
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_BMP085.h>
#include <Arduino.h>
#include "SensorsModule.h"

#define DHTPIN D4     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11
#define ONE_WIRE_BUS D2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
Adafruit_BMP085 bmp;

//Analog input
int sensorValue;

// DH11
DHT dht(DHTPIN, DHTTYPE);
float prev_h = 0, prev_t = 0;

struct SensorsData {
  float humidity;
  float dhtTemp;
  float dallasTemp;
  float bmpTemp;
  float pressure;
  float voltage;
};

void setupSensors()
{
  sensors.begin();
  dht.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED on

  if (!bmp.begin()) {
    Serial.println("Could not find BMP180 or BMP085 sensor at 0x77");
    while (1) {}
  }
}

struct SensorsData read_sensors()
{
    struct SensorsData resultData;
    
    // read the input on analog pin 0:
    sensorValue = analogRead(A0);
    // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 3.2V):
    resultData.voltage = sensorValue * (3.2 / 1023.0)*((100.0+9.697)/9.697)*1.07;
    
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    resultData.humidity = dht.readHumidity();
    // Read temperature as Celsius (the default)
    resultData.dhtTemp = dht.readTemperature();
  
    // Check if any reads failed and exit early (to try again).
    if (isnan(resultData.humidity) || isnan(resultData.dhtTemp)) {
      Serial.println("Failed to read from DHT sensor!");
      resultData.humidity = prev_h;
      resultData.dhtTemp = prev_t;
    } else {
      prev_h = resultData.humidity;
      prev_t = resultData.dhtTemp;
    }

    // Reading DallasTemperature
    sensors.requestTemperatures(); // Send the command to get temperatures
    resultData.dallasTemp = sensors.getTempCByIndex(0); // Temperature for the device 1 (index 0)
    if(resultData.dallasTemp != DEVICE_DISCONNECTED_C) 
    {
      Serial.print("Temperature for the device 1 (index 0) is: ");
      Serial.println(resultData.dallasTemp);
    } 
    else
    {
      Serial.println("Error: Could not read DallasTemperature data");
    }
    resultData.dallasTemp = (round(resultData.dallasTemp*10))/10.0;


    // Reading bmp temp and pressure
    resultData.bmpTemp = bmp.readTemperature();
    Serial.print("Bmp temperature is: ");
    Serial.print(resultData.bmpTemp);
    Serial.println(" *C\t");

    resultData.pressure = bmp.readPressure();
    Serial.print("Bmp pressure is: ");
    Serial.print(resultData.pressure);
    Serial.println(" Pa\t");
    
    Serial.print("Humidity: ");
    Serial.print(resultData.humidity);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(resultData.dhtTemp);
    Serial.println(" *C\t");

    return resultData;
}