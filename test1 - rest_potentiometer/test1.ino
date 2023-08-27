#include <WiFi.h>
#include <HTTPClient.h>
#include "credentials.h"

HTTPClient http;
char data[256];
int sensorValue;

const char* ssid = ssidKey;
const char* password = passKey;
char apiUrl[] = "http://192.168.1.106:8080/api/events"; 

void setup() {
  Serial.begin(115200);
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
  sensorValue = analogRead(32);

  sprintf(data, "{\"timestamp\":%d, \"value\":%d}", millis(), sensorValue);
  Serial.println(data);
  
  http.begin(apiUrl);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(data);
 
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println(httpResponseCode);
    Serial.println(response);
  }
  else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }
   
  http.end();
  delay(500);
}