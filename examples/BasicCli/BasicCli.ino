#include <Arduino.h>
#include <Wire.h>
#include <SD.h>
#include <SPIFFS.h>
#include <LittleFS.h>
#include <FFat.h>
#include <ESP32SerialCtl.h>

static esp32serialctl::ESP32SerialCtl<> esp32SerialCtl;

#define TO_STRING(x) #x
#define USE_FS LittleFS // Choose one: SD, SPIFFS, LittleFS, FFat

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  Wire1.begin();
  delay(500);

  if (!USE_FS.begin(true))
  {
    Serial.print(TO_STRING(USE_FS) " Mount Failed");
  }
  else
  {
    File file = USE_FS.open("/test.txt", "w");
    file.printf("test string\n");
    file.close();
  }
}

void loop()
{
  esp32SerialCtl.service();
}
