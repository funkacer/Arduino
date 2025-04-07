/*
  Repeating WiFi Web Client

 This sketch connects to a a web server and makes a request
 using a WiFi equipped Arduino board.

 created 23 April 2012
 modified 31 May 2012
 by Tom Igoe
 modified 13 Jan 2014
 by Federico Vanzati

 This code is in the public domain.

  Find the full UNO R4 WiFi Network documentation here:
  https://docs.arduino.cc/tutorials/uno-r4-wifi/wifi-examples#wi-fi-web-client-repeating
 */

#include "WiFiS3.h"
#include <ArduinoJson.h>

#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;

// Initialize the WiFi client library
WiFiClient client;

// server address:
//char server[] = "example.org";
char server[] = "www.funkacer.cz";
//IPAddress server(64,131,82,241);

unsigned long lastConnectionTime1 = 0;            // last time you connected to the server, in milliseconds
unsigned long lastConnectionTime2 = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval1 = 55L * 1000L; // delay between updates, in milliseconds
const unsigned long postingInterval2 = 50L * 1000L; // delay between updates, in milliseconds
String last1 = "";
String last2 = "";

bool ido = true;

/* -------------------------------------------------------------------------- */
void setup() {
/* -------------------------------------------------------------------------- */  
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    //delay(10000);
  }
  // you're connected now, so print out the status:
  printWifiStatus();
}

/* just wrap the received data up to 80 columns in the serial print*/
/* -------------------------------------------------------------------------- */
String read_request() {
/* -------------------------------------------------------------------------- */
  uint32_t received_data_num = 0;
  String last = "";
  //char* last = "";
  int read = 0;

  while (client.available()) {
    /* actual data reception */
    char c = client.read();
    /* print data to serial port */
    //Serial.print(c);
    if (c == '[') {
      read = 1;
    }
    if (read == 1) {
      last += c;
    }
    /* wrap data to 80 columns*/
    received_data_num++;
    if(received_data_num % 160 == 0) { 
      //Serial.println();
    }
  }

  return last;
  
}

/* -------------------------------------------------------------------------- */
void loop() {
/* -------------------------------------------------------------------------- */  
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  
  String last = read_request();
  if (last != "") {
    Serial.println(last);

    JsonDocument doc;
    deserializeJson(doc, last);

    
    if ((String)doc[0]["temp_min"] != "null") {
      Serial.println((String)doc[0]["datum"]);
      Serial.println((String)doc[1]["den"]);
      Serial.println((String)doc[2]["temp_min"]);
      Serial.println((String)doc[4]["temp_max"]);
      Serial.println((String)doc[5]["w_main"]);
      Serial.println((String)doc[6]["datum"]);
      
    }
    

    // This loop will look through the JSON response and find the lines
    // that contain the temperature and weather codes,
    // which will then be placed in arrays so they can be easily accessed
    /*
    for (int i = 0; i < numOfForecasts; i++) {
      temperatureMax[i] = doc["daily"]["temperature_2m_max"][i];
      temperatureMin[i] = doc["daily"]["temperature_2m_min"][i];
      weather[i] = doc["daily"]["weather_code"][i];
      Serial.println(weather[i]);
    }
    */
    
  }

  if (ido) {
    ido = !ido;
    httpRequest1();
  }

  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime1 > postingInterval1) {
    httpRequest1();
  }

  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime2 > postingInterval2) {
    httpRequest2();
  }

  delay(100);

}

// this method makes a HTTP connection to the server:
/* -------------------------------------------------------------------------- */
void httpRequest1() {
/* -------------------------------------------------------------------------- */  
  // close any connection before send a new request.
  // This will free the socket on the NINA module
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
  //if (client.connect(server, 443)) {
    Serial.println("connecting1...");
    // send the HTTP GET request:
    //client.println("GET /status/status HTTP/1.1");
    client.println("GET /weather/getall HTTP/1.1");
    client.println("Host: funkacer.cz");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
    // note the time that the connection was made:
    lastConnectionTime1 = millis();
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}


// this method makes a HTTP connection to the server:
/* -------------------------------------------------------------------------- */
void httpRequest2() {
/* -------------------------------------------------------------------------- */  
  // close any connection before send a new request.
  // This will free the socket on the NINA module
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
  //if (client.connect(server, 443)) {
    Serial.println("connecting2...");
    // send the HTTP GET request:
    //client.println("GET /status/status HTTP/1.1");
    client.println("GET /weather/getweather HTTP/1.1");
    client.println("Host: funkacer.cz");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
    // note the time that the connection was made:
    lastConnectionTime2 = millis();
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}

/* -------------------------------------------------------------------------- */
void printWifiStatus() {
/* -------------------------------------------------------------------------- */  
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


