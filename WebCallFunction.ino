#include <ArduinoJson.h>
#include <WiFiNINA.h>
#include "secret.h" 

#define LED_RED 11
#define LED_ORANGE 10
#define LED_GREEN 9
#define LOOP_TIMER 10000

// Setting up WiFi
WiFiSSLClient client;
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS; 

// Firebase Setup
char   HOST_NAME[] = "iot-traffic-light-demo-default-rtdb.firebaseio.com";
String PATH_NAME   = "/Traffic.json";

// JSON Setup
StaticJsonDocument<200> doc;

// Lights Data Setup
bool RedBool = false;
bool OrangeBool = false;
bool GreenBool = false;

void setup() { 
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_ORANGE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  Serial.begin(9600);           // enables Serial Comms
  while (!Serial) {}            // wait for serial to connect
  
  delay(5000);
  // Connect to Wifi Network
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
  Serial.println("You're connected to the network!");
  Serial.println();
}

void loop() {
  GetLightsData();

  if (RedBool != digitalRead(LED_RED)) Toggle("red");
  if (OrangeBool != digitalRead(LED_ORANGE)) Toggle("orange");
  if (GreenBool != digitalRead(LED_GREEN)) Toggle("green");

  delay(LOOP_TIMER);
}

void Toggle(String colour) {
  if(colour == "red") digitalWrite(LED_RED,!digitalRead(LED_RED));
  if(colour == "orange") digitalWrite(LED_ORANGE,!digitalRead(LED_ORANGE));
  if(colour == "green") digitalWrite(LED_GREEN,!digitalRead(LED_GREEN));
}

void GetLightsData() {
  
  // Connecting to firebase server
  while (!Serial);
  if (client.connect(HOST_NAME, 443)) { Serial.println("Connected to Firebase server");}
  else {Serial.println("connection failed");}

  // Set GET request for data
  client.println("GET " + PATH_NAME + " HTTP/1.1");
  client.println("Host: " + String(HOST_NAME));
  client.println("Connection: close");
  client.println(); // end HTTP header

  String response, payload; // Ready response strings

  Serial.print("Saving Response... ");
  while (client.connected()) {
    if (client.available()) {
      response = client.readString(); 
    }

    // Parsing server response for JSON payload
    payload = response.substring(response.indexOf('{'), response.lastIndexOf('}') + 1);
  }
  Serial.print("done.");
  client.stop();
  Serial.println(" disconnected.");

  Serial.print("Payload: "); // Printing final JSON payload
  Serial.println(payload);
  Serial.println();
  
  DeserializationError error = deserializeJson(doc, payload); // Parsing payload for data
  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  // Save data in global variables
  RedBool = doc["RED"];
  OrangeBool = doc["ORANGE"];
  GreenBool = doc["GREEN"];
}
