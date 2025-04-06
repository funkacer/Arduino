#include <WiFiS3.h>
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>
#include "thingProperties.h"
#include <Arduino_LED_Matrix.h>
#include <Arduino_CloudConnectionFeedback.h>

ArduinoLEDMatrix matrix;

// Number that sets how many days of forecasts that are collected
const int numOfForecasts = 7;

double temperatureMax[numOfForecasts];
double temperatureMin[numOfForecasts];
int weather[numOfForecasts];


void setup() {
  Serial.begin(9600);
  delay(1500);

  matrix.begin();

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  waitForArduinoCloudConnection(matrix);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  int status = WL_IDLE_STATUS;
  WiFi.begin(SSID, PASS);
  delay(1000);
  status = WiFi.status();

  // Set location of weather forecasts
  //and display location on the cloud dashboard
  weatherLocation = Location(latitude, longitude);

  getWeatherData();
}

void loop() {
  ArduinoCloud.update();
}

void getWeatherData() {
  int httpPort = 80;
  char serverAddress[] = "api.open-meteo.com"; // server address
  WiFiClient wifi;
  HttpClient client = HttpClient(wifi, serverAddress, httpPort);

  // This loop will look through the JSON response and find the lines
  // that contain the temperature and weather codes,
  // which will then be placed in arrays so they can be easily accessed
  client.get("/v1/forecast?latitude=" + String(latitude) + "&longitude=" + String(longitude) + "&current=temperature_2m,weather_code&daily=weather_code,temperature_2m_max,temperature_2m_min&timezone=Europe%2FBerlin");
  Serial.println("/v1/forecast?latitude=" + String(latitude) + "&longitude=" + String(longitude) + "&current=temperature_2m,weather_code&daily=weather_code,temperature_2m_max,temperature_2m_min&timezone=Europe%2FBerlin");

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      client.stop();
      return;
    }
  }

  int statusCode = client.responseStatusCode();
  // The entire response from the API in JSON is then stored in "result"
  String result = client.responseBody();
  // To only get the information that is needed, the JSON response will be parsed using a parsing function
  parseWeatherData(result);
}

void parseWeatherData(const String& json) {
  StaticJsonDocument<7000> doc;
  deserializeJson(doc, json);

  // This loop will look through the JSON response and find the lines
  // that contain the temperature and weather codes,
  // which will then be placed in arrays so they can be easily accessed
  for (int i = 0; i < numOfForecasts; i++) {
    temperatureMax[i] = doc["daily"]["temperature_2m_max"][i];
    temperatureMin[i] = doc["daily"]["temperature_2m_min"][i];
    weather[i] = doc["daily"]["weather_code"][i];
  }
}

void onLongitudeChange(){
  getWeatherData();
  weatherLocation = Location(latitude, longitude);
}

void onLatitudeChange(){
  getWeatherData();
  weatherLocation = Location(latitude, longitude);
}

void onDayChange() {
  
}

void onForecastScheduleChange() {
  
}
