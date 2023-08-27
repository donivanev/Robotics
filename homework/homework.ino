#include <Arduino.h>
#include <Arduino_JSON.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "arduino_secrets.h"
#include <ESP32Servo.h>

Servo myServo1;
Servo myServo2;
HTTPClient http;
char data[256];
int servoPin1 = 18;
int servoPin2 = 19;
int pulseWidth;
unsigned long MOVING_TIME = 3000; // moving time is 3 seconds
unsigned long moveStartTime;
int startAngle = 30; // 30°
int stopAngle  = 90; // 90°

#define CLOCKWISE 1
#define COUNTER_CLOCKWISE 2

#define ENCODER_L 13
#define ENCODER_R 18
#define SPEED_CALC_PERIOD 50
#define DEBOUNCETIME 1

volatile int interruptCountL = 0;
volatile int interruptCountR = 0;
uint32_t debounceTimeoutL = 0;
uint32_t debounceTimeoutR = 0;
uint32_t debounceTimeoutOldL = 0;
uint32_t debounceTimeoutOldR = 0;
long encoderCountL = 0;
long encoderCountR = 0;
long encoderCountOldL = 0;
long encoderCountOldR = 0;
// Mutexes for synchronizing counter access between ISRs and main loop
portMUX_TYPE muxL = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE muxR = portMUX_INITIALIZER_UNLOCKED;

unsigned long startTime = 0;
unsigned long endTime = 0;
unsigned long currentTime = 0;
float rotL = 0;
float rotR = 0;
float rpsL = 0;
float rpsR = 0;

const char* ssid = ssidKey;
const char* password = passKey;
char apiUrl[] = "http://192.168.1.102:8080/api/events/slider";

// void resetEncoderCounters() {
//   encoderCountL = 0;
//   encoderCountR = 0;
//   encoderCountOldL = 0;
//   encoderCountOldR = 0;
// }

// void setMotorDirection(int motor, int direction) {
//   
// }

// void setMotorSpeed(int motor, int speed) {
//   
// }

// void updateEncoderData() {
//   if (interruptCountL > 0) {
//     portENTER_CRITICAL(&muxL);
//     encoderCountL += interruptCountL;
//     interruptCountL = 0;
//     portEXIT_CRITICAL(&muxL);
//     Serial.printf("An interruptL has occurred. Total: %d\n", encoderCountL);
//   }

//   if (interruptCountR > 0) {
//     portENTER_CRITICAL(&muxR);
//     encoderCountR += interruptCountR;
//     interruptCountR = 0;
//     portEXIT_CRITICAL(&muxR);
//     Serial.printf("An interruptR has occurred. Total: %d\n", encoderCountR);
//   }

//   // Calculating the speed
//   currentTime = millis();
//   if (currentTime > endTime) {
//     rotL = (encoderCountL - encoderCountOldL) / 40.0;   // 1 rotation = 2 x 20 impulses;
//     encoderCountOldL = encoderCountL;
//     rotR = (encoderCountR - encoderCountOldR) / 40.0;   // 1 rotation = 2 x 20 impulses;
//     encoderCountOldR = encoderCountR;
//     rpsL = (rotL * 1000) / (currentTime - startTime); // rotations per second
//     rpsR = (rotR * 1000) / (currentTime - startTime); // rotations per second
//     Serial.printf("RotationsL: %8.2f, RotationsR: %8.2f | SpeedL: %8.2f rps, SpeedR: %8.2f rps\n", rotL, rotR, rpsL, rpsR);
//     startTime = millis();
//     endTime = startTime + SPEED_CALC_PERIOD;
//   }
// }

// void moveForward(int speed, int timeMs) {
void moveForward1() {
  myServo1.attach(18);
  for(int posDegrees = 0; posDegrees <= 180; posDegrees++) {
    myServo1.write(posDegrees);
    delay(20);
  }
}

// void moveBackward(int speed, int timeMs) {
void moveBackward1() {
  myServo1.attach(18);
  for(int posDegrees = 180; posDegrees >= 0; posDegrees--) {
    myServo1.write(posDegrees);
    delay(20);
  }
}

void stopServo1() {
  myServo1.detach();
}

void moveForward2() {
  myServo2.attach(19);
  for(int posDegrees = 0; posDegrees <= 180; posDegrees++) {
    myServo2.write(posDegrees);
    delay(20);
  }
}

// void moveBackward(int speed, int timeMs) {
void moveBackward2() {
  myServo2.attach(19);
  for(int posDegrees = 180; posDegrees >= 0; posDegrees--) {
    myServo2.write(posDegrees);
    delay(20);
  }
}

void stopServo2() {
  myServo2.detach();
}

void setup() {
  myServo1.attach(servoPin1);
  myServo2.attach(servoPin2);
  moveStartTime = millis();
  //pinMode(servoPin, OUTPUT);
  // pinMode(ENCODER_L, INPUT);
  // pinMode(ENCODER_R, INPUT);
  // attachInterrupt(digitalPinToInterrupt(ENCODER_L), isrSpeedL, CHANGE ); // attach iterrupt to speed sensor SpeedL pin, detects every change high->low and low->high
  // attachInterrupt(digitalPinToInterrupt(ENCODER_R), isrSpeedR, CHANGE ); // attach iterrupt to speed sensor SpeedR pin, detects every change high->low and low->high
  Serial.begin(115200);
  // startTime = millis();
  // endTime = startTime + SPEED_CALC_PERIOD;

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

void servoPulse(int servoPin, int angle) {
  pulseWidth = (angle * 10) + 600;  // opredelya zabavyaneto
  digitalWrite(servoPin, HIGH);         // vklyuchva servoto
  delayMicroseconds(pulseWidth);        // izchakva mikrosekundi
  digitalWrite(servoPin, LOW);          // izklyuchva servoto
}

unsigned long start_time = 0;
unsigned long end_time = 0;
int steps=0;
float steps_old=0;
float temp = 0;

void loop() {
  // sprintf(data, "{\"motorLeftSpeed\":%d, \"motorLeftDirection\":%d, \"motorRightSpeed\":%d, \"motorRightDirection\":%d}", 
  //         1, 2, 3, 4);
  // Serial.println(data);

  http.begin(apiUrl);
  //http.addHeader("Content-Type", "application/json");

  //int httpResponseCode = http.POST(data);
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println(httpResponseCode);
    Serial.println(response);

    JSONVar jsonResponse = JSON.parse(response);
    JSONVar keys = jsonResponse.keys();

    int leftAngle = jsonResponse["leftAngle"];
    int rightAngle = jsonResponse["rightAngle"];

    Serial.println(leftAngle);
    Serial.println(rightAngle);

    myServo1.write(leftAngle);
    myServo2.write(rightAngle);

    //(int)jsonResponse[keys[1]] == 1 ? myServo1.write((int)jsonResponse[keys[2]]) : myServo2.write((int)jsonResponse[keys[2]]);

    if ((bool)jsonResponse[keys[2]] == true)
    {
      (int)jsonResponse[keys[1]] == 1 ? moveForward1() : moveForward2();
    }
    if ((bool)jsonResponse[keys[3]] == true)
    {
      (int)jsonResponse[keys[1]] == 1 ? moveBackward1() : moveBackward2();
    }
    if ((bool)jsonResponse[keys[4]] == true) {
      (int)jsonResponse[keys[1]] == 1 ? stopServo1() : stopServo2();
    }
  }
  else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }
   
  http.end();
  delay(500);
}