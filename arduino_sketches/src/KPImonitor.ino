// Импортируем библиотеку поддержки ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h> 
#include <DHT.h> // For temperature and humidity sensor 
#include <DHT_U.h>  // For temperature and humidity sensor 
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_BMP085.h>
#include <Arduino_JSON.h>
#include <LightSleepModule.h>
#include <ServiceModule.h>
#include <SDcardModule.h>

#define DHTPIN D4     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11
#define ONE_WIRE_BUS D2
#define DEEP_SLEEP_INTERVAL 600 // s/10min

WiFiClient client;

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
float dallasTemp;

Adafruit_BMP085 bmp;
float bmpTemp;
int bmpPressure;

File myFile;

//Analog input
int sensorValue;
float voltage;

// DH11
DHT dht(DHTPIN, DHTTYPE);
float h, t, prev_h = 0, prev_t = 0;

struct WifiDataAndSerialNumber wifiData;

// Hosting data
const char* host = "192.168.0.200:1880"; // computer IP
String url = "/update-sensors"; 
const int httpPort = 80;

void send_request(String data);
void sendParsedDataToNodeRed()
{
  String parsedData;

  // re-open the file for reading:
  myFile = SD.open("data.txt");
  if (myFile) {
    while (myFile.available()) {
      char data = myFile.read();
      parsedData.concat(data);
    }
    // close the file:
    myFile.close();

    for (int i = 0; i < parsedData.length(); i++)
    {
      String item = getValue(parsedData, '\n', i);
      if (item.equals("")) {
        break;
      } else {
        send_request(item);
        delay(5000);
      }
    }
    SD.remove("data.txt");
  } else {
    Serial.println("error opening data.txt");
  }
}

void connect_to_Wifi()
{
  wifiData = getDataFromSdCard(myFile);
  delay(2000);

  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiData.login, wifiData.password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println();
  }
}

void read_sensors()
{
  // read the input on analog pin 0:
    sensorValue = analogRead(A0);
    // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 3.2V):
    voltage = sensorValue * (3.2 / 1023.0)*((100.0+9.697)/9.697)*1.07;
    Serial.print("ADC: ");
    Serial.println(sensorValue);
    
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    t = dht.readTemperature();
  
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      h = prev_h;
      t = prev_t;
    } else {
      prev_h = h;
      prev_t = t;
    }

    // Reading DallasTemperature
    sensors.requestTemperatures(); // Send the command to get temperatures
    dallasTemp = sensors.getTempCByIndex(0); // Temperature for the device 1 (index 0)
    if(dallasTemp != DEVICE_DISCONNECTED_C) 
    {
      Serial.print("Temperature for the device 1 (index 0) is: ");
      Serial.println(dallasTemp);
    } 
    else
    {
      Serial.println("Error: Could not read DallasTemperature data");
    }
    dallasTemp = (round(dallasTemp*10))/10.0;


    // Reading bmp temp and pressure
    bmpTemp = bmp.readTemperature();
    Serial.print("Bmp temperature is: ");
    Serial.print(bmpTemp);
    Serial.println(" *C\t");

    bmpPressure = bmp.readPressure();
    Serial.print("Bmp pressure is: ");
    Serial.print(bmpPressure);
    Serial.println(" Pa\t");
    
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" *C\t");
}

void send_request(String data = "0")
{
  String postData;
  Serial.print("Requesting URL: "); 
  Serial.println(url); //Post Data
  String time = getTime(client);

  if (data != "0") {
    postData = data;
  } else {
    JSONVar jsonData;
    jsonData["humidity"] = (int)h;
    jsonData["temp"] = dallasTemp;
    jsonData["temp_dht"] = t;
    jsonData["temp_bpm"] = bmpTemp;
    jsonData["pressure"] = bmpPressure;
    jsonData["voltage"] = voltage;
    jsonData["time"] = time;
    jsonData["device_number"] = wifiData.serialNumber;
    postData = JSON.stringify(jsonData);
  }
  
  String address = host + url; 
  Serial.println(address);
  Serial.println(postData);
  HTTPClient http; 
  String fullUrl = "http://" + address;
  http.begin(client, fullUrl); 
  http.addHeader("Content-Type", "application/json");
  auto httpCode = http.POST(postData); 
  
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      Serial.println("Data was successfully sended!");

      if(data == "0"){
        sendParsedDataToNodeRed();
      }
    } else {
      Serial.println("Failed to send data");
      writeDataToSdCard(myFile, postData);
    }
  } else {
    Serial.println("Failed to connect to Node-Red");
    writeDataToSdCard(myFile, postData);
    return;
  }

  http.end(); //Close connection 
  Serial.println("closing connection");
}

void setup(void)
{ 
  // Инициализация последовательного порта
  Serial.begin(9600);
  delay(500);

  sensors.begin();
  dht.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED on

  if (!bmp.begin()) {
    Serial.println("Could not find BMP180 or BMP085 sensor at 0x77");
    while (1) {}
  }
}

void loop(){
  connect_to_Wifi();   // Инициализация соединения WiFi

  read_sensors();
  send_request();

  sleep(DEEP_SLEEP_INTERVAL * 1000);
}
