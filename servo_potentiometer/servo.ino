#include <ESP32Servo.h>
Servo myServo;

int const potPin = 4;
int potVal;
int angle;

void setup() {
  myServo.attach(2);
  Serial.begin(9600);
}

void loop() {
  potVal = analogRead(potPin);
  Serial.print("potVal: ");
  Serial.print(potVal);
  angle = map(potVal, 0, 4095, 0, 180);
  Serial.print(", angle: ");
  Serial.println(angle);
  myServo.write(angle);
  delay(100);
}