#include <HTTPClient.h>
#include <WiFi.h>
#include <HttpClient.h>
#include <Wire.h>
#include <max6675.h>
#include <Adafruit_MCP3008.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

// Network credentials
const char* ssid    = "yourAP";
const char* password = "yourPassword";

// Domain website/IP address
const char* serverName = "http://192.168.4.2/post-esp-data.php";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

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
// Software SPI (specify all, use any available digital)
//          sck, mosi, miso, cs
adc.begin(14,13,12,15);

// Voltage sensor setup
float adcValue1;
float voltValue1;
const int volt = 35;

// Current sensor setup
float adcValue2;
float ampValue1;
const int amp = 32;

adcValue1 = analogRead(volt);
voltValue1 = 0.19 + ((adcValue1 * 3.3) / 4095);
adcValue2 = analogRead(amps);
ampValue1 = 0.19 + ((adcValue2 * 3.3) / 4095);

String readTemperatureC() {
  float C = thermocouple.readCelsius();
  return C;
}

String readTemperatureF() {
  float F = thermocouple.readFahrenheit();
  return F;
}

String readIrradiance() {
  float I = adc.readADCDifference(1) * conv * pcf;
  return I;
}

String readVoltage()  {
  adcValue1 = analogRead(volt);
  voltValue1 = 0.19 + ((adcValue1 * 3.3) / 4095);
  return voltValue1;
}

String readCurrent()  {
  adcValue2 = analogRead(amps);
  ampValue1 = 0.19 + ((adcValue2 * 3.3) / 4095);
  return ampValue1;
}

void setup()  {
  Serial.begin(115200);
  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting...");
  while(WiFi.status() != WL_CONNECTED)  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP address: ");
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
  server.on("/temperature_celsius", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readTemperatureC().c_str());
  });
  server.on("/temperature_fahrenheit", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readTemperatureF().c_str());
  });
  server.on("/irradiance", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readIrradiance().c_str());
  });
  server.on("/voltage", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readVoltage().c_str());
  });
  server.on("/current", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readCurrent().c_str());
  });
}

void loop() {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverName);

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    //(OLD) adcValue = analogRead(pyra);
    //(OLD)voltValue = 0.2 + ((adcValue * 3.3) / 4095);
    //adcValue1 = analogRead(volt);
    //voltValue1 = 0.19 + ((adcValue1 * 3.3) / 4095);
    ///ampValue1 = 0.19 + ((adcValue2 * 3.3) / 4095);
    /*  value1 = String(thermocouple.readCelsius())
        value2 = String(thermocouple.readFahrenheit())
        value3 = String(adc.readADCDifference(1) * conv * pcf)
        value4 = String(voltValue1)
        value5 = String(ampValue1)
    */ 
    
    String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName + "&location=" + sensorLocation + "&value1=" + String(thermocouple.readCelsius()) + "&value2=" + String(thermocouple.readFahrenheit())  + "&value3=" + String(adc.readADCDifference(1) * conv * pcf) + "&value4=" + String(voltValue1) + "&value5=" + String(ampValue1);
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