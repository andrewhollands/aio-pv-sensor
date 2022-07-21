#include <HTTPClient.h>
#include <WiFi.h>
//#include <HttpClient.h>
//#include <Wire.h>
#include <max6675.h>
#include <Adafruit_MCP3008.h>

// Number of sample readings to average
#define SAMPLES		10

// Sensor pins
#define THERMO_DO	19
#define THERMO_CS	5
#define THERMO_CLK	18
#define PYRA_SCK	14
#define PYRA_MOSI	13
#define PYRA_MISO	12
#define PYRA_CS 	15
#define VOLT 		35
#define AMP 		34

// Network credentials
const char* ssid     = "yourAP";
const char* password = "yourPassword";

// Domain website/IP address
const char* serverName = "http://192.168.4.3/post-esp-data.php";

// API key value
// Ensure this is the same as the value in post-esp-data.php
String apiKeyValue = "tPmAT5Ab3j7F9";

// Sensor metadata
String sensorName = "AIO Sensor";
String sensorLocation = "Solar Panel 1";

// Thermocouple
MAX6675 thermocouple(THERMO_CLK, THERMO_CS, THERMO_DO);
int rawCelsiusTemperature[SAMPLES] = {};
int celsiusTemperatureSum = 0;
float averageRawCelsiusTemperature = 0;
float outputCelsiusTemperature = 0;
int rawFahrenheitTemperature[SAMPLES] = {};
int fahrenheitTemperatureSum = 0;
float averageRawFahrenheitTemperature = 0;
float outputFahrenheitTemperature = 0;

// Pyranometer
Adafruit_MCP3008 adc;
float conv = (3.3 / 1024);
float pcf = 5000; // 1000 for V to mV, 5 for mV to W/m^2
// output = adc.readADCDifference(1) * conv * pcf
int rawIrradiance[SAMPLES] = {};
int irradianceSum = 0;
float averageRawIrradiance = 0;
float outputIrradiance = 0;

// Voltage sensor
int analogVolt[SAMPLES] = {};
int voltSum = 0;
float averageAnalogVolt = 0;
float digitalVolt = 0;

// Current sensor
int analogAmp[SAMPLES] = {};
int ampSum = 0;
float averageAnalogAmp = 0;
float digitalAmp = 0;

void setup()
{
	Serial.begin(115200);
  	WiFi.begin(ssid, password);
  	Serial.println("Connecting...");
  	while(WiFi.status() != WL_CONNECTED)
  	{
    	delay(500);
    	Serial.print(".");
  	}
	Serial.println("");
  	Serial.print("Connected to WiFi network with IP address: ");
  	Serial.println(WiFi.localIP());  	
  	adc.begin(PYRA_SCK, PYRA_MOSI, PYRA_MISO, PYRA_CS);  
}

void loop()
{
	if(WiFi.status() == WL_CONNECTED)
	{
	    HTTPClient http;

	    http.begin(serverName);

	    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
	    
	    // Thermocouple sensing
	    for (int i; i < SAMPLES; i++)
	    {
	    	rawCelsiusTemperature[i] = thermocouple.readCelsius();
	    	celsiusTemperatureSum += rawCelsiusTemperature[i];
	    }
	    averageRawCelsiusTemperature = celsiusTemperatureSum / SAMPLES;
	    outputCelsiusTemperature = averageRawCelsiusTemperature;

	    for (int i; i < SAMPLES; i++)
	    {
	    	rawFahrenheitTemperature[i] = thermocouple.readFahrenheit();
	    	fahrenheitTemperatureSum += rawFahrenheitTemperature[i];
	    }
	    averageRawFahrenheitTemperature = fahrenheitTemperatureSum / SAMPLES;
	    outputFahrenheitTemperature = averageRawFahrenheitTemperature;

	    // Pyranometer sensing
	    for (int i = 0; i < SAMPLES; i++)
    	{
    		rawIrradiance[i] = adc.readADCDifference(1) * conv * pcf;
    		irradianceSum += rawIrradiance[i];
    	}
    	averageRawIrradiance = irradianceSum / SAMPLES;
    	outputIrradiance = averageRawIrradiance;

	    // Voltage sensing
	    for (int i = 0; i < SAMPLES; i++)
    	{
	        analogVolt[i] = analogRead(VOLT);
	        voltSum += analogVolt[i];
    	}
	    averageAnalogVolt = voltSum / SAMPLES;
	    digitalVolt = 22.05 * (0.16 + (averageAnalogVolt * 3.3 / 4095));

	    // Current sensing
		for (int i = 0; i < SAMPLES; i++)
    	{
	        analogAmp[i] = analogRead(AMP);
	        ampSum += analogAmp[i];
    	}
	    averageAnalogAmp = ampSum / SAMPLES;
	    digitalAmp = ((averageAnalogAmp * 3.3) / 4095);
	    digitalAmp = ((digitalAmp + 0.102) / 0.173);

	    String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName + "&location=" + sensorLocation + "&value1=" + String(outputCelsiusTemperature) + "&value2=" + String(outputFahrenheitTemperature)  + "&value3=" + String(outputIrradiance) + "&value4=" + String(digitalVolt) + "&value5=" + String(digitalAmp);
	    Serial.print("httpRequestData: ");
	    Serial.println(httpRequestData);

	    // Send HTTP POST request
	    int httpResponseCode = http.POST(httpRequestData);

	    if (httpResponseCode>0)
	    {
	      Serial.print("HTTP Response Code: ");
	      Serial.println(httpResponseCode);
	    }
	    else
	    {
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
