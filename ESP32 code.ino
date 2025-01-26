#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <DHT.h>
#include <DHT_U.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "ACS712.h"
#include <BH1750.h>
#include "time.h"
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <Blynk.h>
#include "D:\arduino-1.8.19\libraries\Random_Forest.h"

char auth[] = "n3KDCJEb8CdgBeA0UgWSHcUGa9viduA4";
char ssid[] = "KAVINDU";
char pass[] = "password";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;
const char* openWeatherMapApiKey = "fb73d4644cc4e9bf9de38de50222c6fc";
String city = "Colombo"; // City name
String countryCode = "LK"; // Country code (e.g., US)
unsigned long lastTime = 0;
unsigned long timerDelay = 6000; // Set the timer to 10 minutes (600000 milliseconds)

//char ssid[] = "EN20027360";
//char pass[] = "12345678";

#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6RKIpiE7q"
#define BLYNK_DEVICE_NAME "fyp"
#define DHTPIN 4     
#define DHTTYPE DHT11

WiFiServer server(80);

Adafruit_MPU6050 mpu;
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
ACS712  ACS(25, 3.3, 4095, 185);
const int voltPin = 34;
float ADC_VALUE = 0;
float voltage_value = 0;
BlynkTimer timer;
BH1750 lightMeter(0x23);
float power = 0;
float x,y,z = 0;
String weather;
double pressure;
double visibility;
double wind;
double visible;
String currentDate;
String currentTimeStr;
String header;
double temperature,temp;
double humidity;


void setup(void) {
  Serial.begin(115200);
  Wire.begin();
  while (!Serial)
    delay(10); 

  Blynk.begin(auth, ssid, pass);
  ACS.autoMidPoint();


  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  } else {
    Serial.println(F("Error initialising BH1750"));
  }

  Serial.println("");
  delay(100);


  dht.begin();
  lightMeter.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);

  // Send HTTP GET request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  timeinfo.tm_hour += 5;
  timeinfo.tm_min += 30;

  // Adjust for overflow
  if (timeinfo.tm_min >= 60) {
    timeinfo.tm_min -= 60;
    timeinfo.tm_hour += 1;
  }

  Serial.println(&timeinfo, "%B %d %Y %H:%M");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay,10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
  char formattedTime[50]; // This variable will store the formatted date and time
  strftime(formattedTime, sizeof(formattedTime), "%B %d %Y %H:%M", &timeinfo);
  Serial.println(formattedTime);
  Blynk.virtualWrite(V10, formattedTime); 
}

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients
  Serial.println("");
  
  sensors_event_t event;
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
  }
  float mA = ACS.mA_DC();
  Serial.print("Current:");
  Serial.println(mA);
  
  ADC_VALUE = analogRead(voltPin);
  voltage_value = (ADC_VALUE * 3.3 ) / (4095);
  Serial.print("Voltage = ");
  Serial.print(voltage_value);
  Serial.println("V");

  power = voltage_value*mA;
  Serial.print("Power = ");
  Serial.print(power);
  Serial.println("W");

  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  float irr = (lux*0.0079);
  Serial.print("irradiance: ");
  Serial.print(irr);
  Serial.println(" W/m2");
  float radiation = (irr/375000); //Solar Irradiance (W/m²) = Solar Radiation (Joules) / Area (m²) / Time (seconds)(750mm*500mm)
  Serial.print("Radiation: ");
  Serial.print(radiation);
  Serial.println("W");

  delay(1000);

   // Send an HTTP GET request
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&appid=" + openWeatherMapApiKey;

      String jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);

      JSONVar myObject = JSON.parse(jsonBuffer);

      // Check if JSON parsing was successful
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
      } else {
        Serial.println("JSON object received.");
        Serial.print("Temperature: ");
        Serial.println(myObject["main"]["temp"]);
        Serial.print("Pressure: ");
        Serial.println(myObject["main"]["pressure"]);
        Serial.print("Humidity: ");
        Serial.println(myObject["main"]["humidity"]);
        Serial.print("Wind Speed: ");
        Serial.println(myObject["wind"]["speed"]);
        Serial.print("Visibility: ");
        Serial.println(myObject["visibility"]); // Visibility in meters
        Serial.print("Weather: ");
        Serial.println(myObject["weather"][0]["description"]);
        String weather = (myObject["weather"][0]["description"]);
      //  Serial.println(weather);
        Blynk.virtualWrite(V4, weather); 
        // Convert JSON values to appropriate types and assign to variables
        pressure = myObject["main"]["pressure"];
        visibility = myObject["visibility"];
        visible = visibility/1000;
        wind = myObject["wind"]["speed"];
        temperature = myObject["main"]["temp"];
        humidity = myObject["main"]["humidity"];
        temp = temperature - 273.15;
      }
    } else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
  
  //humidity = event.relative_humidity;
  
  delay(100);

  float feature_values[] = {temp, humidity, radiation,temp, humidity, radiation,temp, humidity, radiation}; // Replace with actual feature values
  float prediction = intercept; // Initialize with the intercept
  float prepower;
  int numCoefficients = sizeof(coefficients) / sizeof(coefficients[0]);

  for (int i = 0; i < numCoefficients; i++) {
    prediction += coefficients[i] * feature_values[i];
  }

  // Print the prediction to the serial monitor
  Serial.print("Prediction: ");
  Serial.println(prediction, 4); // Print with 4 decimal places
  prepower = (prediction*6)/5000;
  // Delay for demonstration purposes (adjust as needed)
  delay(5000);


  Blynk.virtualWrite(V0, temp); 
  Blynk.virtualWrite(V1, humidity); 
  Blynk.virtualWrite(V2, wind); 
  Blynk.virtualWrite(V3, visible); 
  Blynk.virtualWrite(V11, prepower); 
  Blynk.virtualWrite(V5, voltage_value); 
  Blynk.virtualWrite(V6, mA); 
  Blynk.virtualWrite(V7, power); 
  Blynk.virtualWrite(V8, irr); 
  Blynk.virtualWrite(V9, pressure); 
  
  Blynk.run();
  timer.run();
  printLocalTime();

}
