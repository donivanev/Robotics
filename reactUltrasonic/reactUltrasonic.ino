#include <WiFi.h>
#include <HTTPClient.h>
#include "credentials.h"

#define SOUND_SPEED 0.034

HTTPClient http;
char data[256];
long duration;
float distance;
const int trigPin = 5;
const int echoPin = 18;

const char* ssid = ssidKey;
const char* password = passKey;
char apiUrl[] = "http://192.168.1.101:8080/api/events"; 

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.SSID());
  Serial.println(WiFi.RSSI());
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration * SOUND_SPEED / 2;

  sprintf(data, "{\"time\":%d, \"distance\":%d}", millis(), distance);
  Serial.println(data);
  
  // http.begin(apiUrl);
  // http.addHeader("Content-Type", "application/json");

  // int httpResponseCode = http.POST(data);
 
  // if (httpResponseCode > 0) {
  //   String response = http.getString();
  //   Serial.println(httpResponseCode);
  //   Serial.println(response);
  // }
  // else {
  //   Serial.print("Error on sending POST: ");
  //   Serial.println(httpResponseCode);
  // }
   
  // http.end();
  delay(500);
}