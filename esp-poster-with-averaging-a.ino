/* 	All-In-One (AIO) Photovoltaic (PV) Sensor A
 	This module is used for controlling the ESP32 MCU onboard the sensing device on panel A.
 	Module's functionalities include:
 		(1) collecting data from each sensing module onboard the AIO PV Sensor,
 		(2) converting that data when necessary (unless converted through HW), and 
 		(3) averaging collected data, and transmitting data to a PHP file stored on the collector node.
*/

#include <HTTPClient.h>
#include <WiFi.h>
#include <max6675.h>
#include <Adafruit_MCP3008.h>

// Number of sample readings to average
#define SAMPLES     10

// Sensor pins
#define THERMO_DO   19
#define THERMO_CS   5
#define THERMO_CLK  18
#define PYRA_SCK    14
#define PYRA_MOSI   13
#define PYRA_MISO   12
#define PYRA_CS     15
#define VOLT        35
#define AMP         34

// Network credentials
String ap_ssid     = "OUC_AIOPV_AP";
String ap_password = "solarpi314";

// Domain website/IP address

int ipFix = 0;	// Counter variable for last digit of IP address (LAN website)
// PHP file where collected data is posted to
String serverNameFix = "http://192.168.4.0/post-esp-data-a.php"; /* Last digit of IP 
																 	address starts at 0
																 	as a base case 
																 	(until it reaches the
																 	correct IP address)
																 */ 

// API key value used to ensure only this device can post data to the database
// acts as an embedded password
String apiKeyValue = "0yNlVEhIJY"; 

// Sensor meta data
String sensorName = "AIO PV Sensor A";
String sensorLocation = "Solar Panel A";

// Thermocouple
MAX6675 thermocouple(THERMO_CLK, THERMO_CS, THERMO_DO); /* 	Instantiating MAX6675 (ADC)
															object with the ESP32 pin
															numbers associated with
															thermocouple's CLK, CS, and 
															DO pins
														*/
int rawCelsiusTemperature[SAMPLES] = {}; // Buffer array used for averaging Celsius temperature values
int celsiusTemperatureSum = 0; // Accumulator variable used for averaging Celsius temperature values
float averageRawCelsiusTemperature = 0; // Average Celsius temperature value
float outputCelsiusTemperature = 0; // Same as averageRawCelsiusTemperature
int rawFahrenheitTemperature[SAMPLES] = {}; // Buffer array used for averaging Fahrenheit temperature values
int fahrenheitTemperatureSum = 0; // Accumulator variable used for averaging Fahrenheit temperature values
float averageRawFahrenheitTemperature = 0; // Average Fahrenheit temperature value
float outputFahrenheitTemperature = 0; // Same as averageRawFahrenheitTemperature

// Pyranometer
Adafruit_MCP3008 adc; // Instantiating MCP3008 object 
float conv = (3.3 / 1024); // Conversion factor for 10-bit ADC (MCP3008)
float pcf = 5000; // Conversion factors of 1000 for V to mV and 5 for mV to W/m^2
int rawIrradiance[SAMPLES] = {}; // Buffer array used for averaging irradiance values
int irradianceSum = 0; // Accumulator variable used for averaging irradiance values
float averageRawIrradiance = 0; // Average irradiance value
float outputIrradiance = 0; // Same as above

// Voltage sensor
int analogVolt[SAMPLES] = {}; // Buffer array used for averaging analog voltage values
int voltSum = 0; // Accumulator variable used for averaging analog voltage values
float averageAnalogVolt = 0; // Average analog voltage value
float digitalVolt = 0; // Average digital voltage value

// Current sensor
int analogAmp[SAMPLES] = {}; // Buffer array used for averaging analog current values
int ampSum = 0; // Accumulator variable used for averaging analog current values
float averageAnalogAmp = 0; // Average analog current value
float digitalAmp = 0; // Average digital current value

// Setup Wi-Fi connection and pyranometer's ADC
void setup()
{
  Serial.begin(115200); // Open a serial port at 115200 baud rate
  WiFi.begin(ap_ssid, ap_password); // Connect to a Wi-Fi network with provided credentials
  Serial.println("Connecting..."); // Print 'Connecting...' status to serial port 
  
  // Print '.' every 500 milliseconds until connected to desired Wi-Fi network
  while(WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(""); // Newline
  
  // Print IP address of Wi-Fi network to serial port
  Serial.print("Connected to WiFi network with IP address: ");
  Serial.println(WiFi.localIP());
  
  // Initiate pyranometer's ADC with the ESP32 pin numbers associated with pyranometer's SCK, MOSI, MISO, and CS pins
  adc.begin(PYRA_SCK, PYRA_MOSI, PYRA_MISO, PYRA_CS);
}

void loop()
{
  // If connected to Wi-Fi network
  if(WiFi.status() == WL_CONNECTED)
  {
    // Instantiating HTTP client object and beginning connection to LAN website
    HTTPClient http;
    http.begin(serverNameFix);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Thermocouple sensing
    // Outputs the average Celsius temperature value collected over SAMPLES samples
    for (int i; i < SAMPLES; i++)
    {
      rawCelsiusTemperature[i] = thermocouple.readCelsius();
      celsiusTemperatureSum += rawCelsiusTemperature[i];
    }
    averageRawCelsiusTemperature = celsiusTemperatureSum / SAMPLES;
    outputCelsiusTemperature = averageRawCelsiusTemperature;
	
	// Outputs the average Fahrenheit temperature value collected over SAMPLES samples
    for (int i; i < SAMPLES; i++)
    {
      rawFahrenheitTemperature[i] = thermocouple.readFahrenheit();
      fahrenheitTemperatureSum += rawFahrenheitTemperature[i];
    }
    averageRawFahrenheitTemperature = fahrenheitTemperatureSum / SAMPLES;
    outputFahrenheitTemperature = averageRawFahrenheitTemperature;

    // Pyranometer sensing
    // Outputs the average irradiance value collected over SAMPLES samples
    for (int i = 0; i < SAMPLES; i++)
    {
      rawIrradiance[i] = adc.readADCDifference(1) * conv * pcf;
      irradianceSum += rawIrradiance[i];
    }
    averageRawIrradiance = irradianceSum / SAMPLES;
    outputIrradiance = averageRawIrradiance;

    // Voltage sensing
    // Outputs the average voltage value collected over SAMPLES samples
    for (int i = 0; i < SAMPLES; i++)
    {
      analogVolt[i] = analogRead(VOLT);
      voltSum += analogVolt[i];
    }
    averageAnalogVolt = voltSum / SAMPLES;
    digitalVolt = 22.05 * (0.16 + (averageAnalogVolt * 3.3 / 4095));

    // Current sensing
    // Outputs the average current value collected over SAMPLES samples
    for (int i = 0; i < SAMPLES; i++)
    {
      analogAmp[i] = analogRead(AMP);
      ampSum += analogAmp[i];
    }
    averageAnalogAmp = ampSum / SAMPLES;
    digitalAmp = ((((averageAnalogAmp * 3.3) / 4095) + 0.102) / 0.173);

    // Concatenate all collected values into a string to be passed to web server via HTTP 
    
    // If thermocouple is disconnected
    if (outputCelsiusTemperature < 0)
    {
      String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName + "&location=" + sensorLocation + "&value1=" + String("NaN") + "&value2=" + String("NaN")  + "&value3=" + String(outputIrradiance) + "&value4=" + String(digitalVolt) + "&value5=" + String(digitalAmp);
    }
    // If thermocouple is connected
    else
    {
      String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName + "&location=" + sensorLocation + "&value1=" + String(outputCelsiusTemperature) + "&value2=" + String(outputFahrenheitTemperature)  + "&value3=" + String(outputIrradiance) + "&value4=" + String(digitalVolt) + "&value5=" + String(digitalAmp);
    }
    
    // Print HTTP post request to serial port
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);

    int httpResponseCode = http.POST(httpRequestData); // HTTP response code
    
	// Print valid HTTP response code to serial port
    if (httpResponseCode>0)
    {
      Serial.print("HTTP Response Code: ");
      Serial.println(httpResponseCode); // Will be 200 if valid
    }
    
    // Print invalid HTTP response code to serial port
    // Increment the counter variable for the last digit of the IP address and update the website URL
    else
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      Serial.print("IP address fix count: ");
      Serial.println(ipFix);
      ipFix += 1;
      serverNameFix = "http://192.168.4." + String(ipFix) + "/post-esp-data-a.php";
      
      // Reset ipFix variable to 0 if it reaches an IP address digit's physical maximum of 255
      if (ipFix > 255)
      {
        ipFix = 0;
      }
    }
    
    // Stop HTTP client
    http.end();
    
    // Reset variables used for averaging 
    celsiusTemperatureSum = 0;
    averageRawCelsiusTemperature = 0;
    outputCelsiusTemperature = 0;
    fahrenheitTemperatureSum = 0;
    averageRawFahrenheitTemperature = 0;
    outputFahrenheitTemperature = 0;
    irradianceSum = 0;
    averageRawIrradiance = 0;
    outputIrradiance = 0;
    voltSum = 0;
    averageAnalogVolt = 0;
    digitalVolt = 0;
    ampSum = 0;
    averageAnalogAmp = 0;
    digitalAmp = 0;
  }
  
  // Print Wi-Fi connection status if not connected to Wi-Fi network
  else
  {
    Serial.println("Wi-Fi Disconnected");
  }
  // Wait 1 second
  delay(1000);
}
