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

#define DHTPIN D4     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11
#define ONE_WIRE_BUS D2
//#define LONG_INTERVAL 30000
//#define SHORT_INTERVAL 10000
#define LONG_INTERVAL 600000
#define SHORT_INTERVAL 60000

long interval = LONG_INTERVAL;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
unsigned long previousMillisReadSensors = 0;

String Area = "alias"; // alias for searching device in Database
WiFiClient client;


// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
float dallasTemp;

Adafruit_BMP085 bmp;
float bmpTemp;
int bmpPressure;

const int chipSelect = D8;
File myFile;

//Analog input
int sensorValue;
float voltage;

// DH11
DHT dht(DHTPIN, DHTTYPE);
float h, t, prev_h = 0, prev_t = 0;

// Параметры вашей сети WiFi
String ssid = "admin";
String password = "admin";
char separator = ':';
String wifiInfo;

// Hosting data
const char* host = "192.168.0.197:1880"; // computer IP
String url = "/update-sensors"; 
const int httpPort = 80;

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void getDataFromSdCard()
{
  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
//  myFile = SD.open("wifi.txt", FILE_WRITE);
//
//  // if the file opened okay, write to it:
//  if (myFile) {
//    Serial.print("Writing to test.txt...");
//    myFile.println("testing 1, 2, 3.");
//    // close the file:
//    myFile.close();
//    Serial.println("done.");
//  } else {
//    // if the file didn't open, print an error:
//    Serial.println("error opening wifi.txt");
//  }

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
    ssid = logVal;
    String pass = getValue(wifiInfo, separator, 2);
    pass.trim();  
    password = pass;
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening wifi.txt");
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

void send_request()
{
  Serial.print("Requesting URL: "); 
  Serial.println(url); //Post Data 
  String postData = "{\"humidity\":";
  postData += (int)h;
  postData += ",\"temp\":";
  postData += dallasTemp;
  postData += ",\"temp_dht\":";
  postData += t;
  postData += ",\"temp_bpm\":";
  postData += bmpTemp;
  postData += ",\"pressure\":";
  postData += bmpPressure;
  postData += ",\"voltage\":";
  postData += voltage;
  postData += ",\"device_alias\":";
  postData += "\"" + Area + "\"";
  postData += ",\"pw\":";
  postData += "\"atepmonitor\"";
  postData += "}";
  
  String address = host + url; 
  Serial.println(address);
  Serial.println(postData);
  HTTPClient http; 
  String fullUrl = "http://" + address;
  http.begin(client, fullUrl); 
  http.addHeader("Content-Type", "application/json");
  auto httpCode = http.POST(postData); 
  Serial.println(httpCode); //Print HTTP return code 
  String response = http.getString(); 
  Serial.println(response); //Print request response payload 
  http.end(); //Close connection Serial.println(); 
  Serial.println("closing connection"); 
}

void setup(void)
{ 
  sensors.begin();
  dht.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off (HIGH is the voltage level)
  if (!bmp.begin()) {
    Serial.println("Could not find BMP180 or BMP085 sensor at 0x77");
    while (1) {}
  }
  
  // Инициализация последовательного порта
  Serial.begin(9600);
  delay(500);
  
  getDataFromSdCard();
  delay(2000);

  // Инициализация соединения WiFi
  Serial.println("Try to connect");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  // Вывод IP адреса платы в терминал
  Serial.println(WiFi.localIP());

  read_sensors();
  send_request();
  //ESP.deepSleep(600000000); // на 10 минут
}

void loop() {

  currentMillis = millis();
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  
  if (currentMillis - previousMillis >= LONG_INTERVAL) 
  {
    previousMillis = currentMillis;
    read_sensors();
    send_request();   
  }

  if (currentMillis - previousMillisReadSensors >= SHORT_INTERVAL)
  {
    previousMillisReadSensors = currentMillis;
    read_sensors();
  }
  
  if (previousMillis > currentMillis)
  {
    // prevent overload unsigned long
    previousMillis = currentMillis;
    previousMillisReadSensors = currentMillis;
  }
}
