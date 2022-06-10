#include <HTTPClient.h>
#include <WiFi.h>
#include <HttpClient.h>
#include <Wire.h>
#include <max6675.h>
#include <Adafruit_MCP3008.h>

// Network credentials
const char* ssid    = "yourAP";
const char* password = "yourPassword";

// Domain website/IP address
const char* serverName = "http://192.168.4.2/post-esp-data.php";

// API key value
// Ensure this is the same as the value in post-esp-data.php
String apiKeyValue = "tPmAT5Ab3j7F9";

String sensorName = "AIO Sensor";
String sensorLocation = "Solar Panel 1";

// Thermocouple setup
int thermoDO  = 19; // SO of MAX6675 module to D19
int thermoCS  = 5;  // CS of MAX6675 module to D5
int thermoCLK = 18; // SCK of MAX6675 module to D18
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

// Pyranometer setup (OLD)
//int adcValue;
//float voltValue;
//const int pyra = 34;

// Pyranometer setup (NEW)
Adafruit_MCP3008 adc;
float conv = (3.3 / 1024);
float pcf = 1000;

// Voltage sensor setup
int adcValue1;
float voltValue1;
const int volt = 35;

// Current sensor setup
int adcValue2;
float ampValue;
const int amp = 34;

void setup()  {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting...");
  while(WiFi.status() != WL_CONNECTED)  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP address: ");
  Serial.println(WiFi.localIP());
  
  // For pyranometer
  // Software SPI (specify all, use any available digital)
  //          sck, mosi, miso, cs
  adc.begin(14,13,12,15);  
}

void loop() {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverName);

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    //(OLD) adcValue = analogRead(pyra);
    //(OLD)voltValue = 0.2 + ((adcValue * 3.3) / 4095);
    
    // Voltage sensing
    adcValue1 = analogRead(volt);
    voltValue1 = ((adcValue1 * 3.3) / 4095);
    
    // Current sensing
    adcValue2 = analogRead(amp);
    ampValue = 0.19 + ((adcValue2 * 3.3) / 4095);
    
    String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName + "&location=" + sensorLocation + "&value1=" + String(thermocouple.readCelsius()) + "&value2=" + String(thermocouple.readFahrenheit())  + "&value3=" + String(adc.readADCDifference(1) * conv * pcf) + "&value4=" + String(voltValue1) + "&value5=" + String(ampValue);
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);

    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode>0) {
      Serial.print("HTTP Response Code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  delay(1000);
}
