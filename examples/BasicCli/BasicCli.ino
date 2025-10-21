#include <Arduino.h>
#include <Wire.h>
#include <ESP32SerialCtl.h>

static esp32serialctl::ESP32SerialCtl<> esp32SerialCtl;

void setup()
{
  Serial.begin(115200);
  Wire.begin(45, 0);
  Wire1.begin();
  delay(50);
}

void loop()
{
  esp32SerialCtl.service();
}
