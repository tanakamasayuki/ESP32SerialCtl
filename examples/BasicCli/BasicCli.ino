#include <Arduino.h>
#include <ESP32SerialCtl.h>

static esp32serialctl::ESP32SerialCtl<> esp32SerialCtl;

void setup()
{
  Serial.begin(115200);
  delay(50);
}

void loop()
{
  esp32SerialCtl.service();
}
