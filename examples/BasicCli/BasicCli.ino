#include <Arduino.h>
#include <Wire.h>
#include <LittleFS.h>
#include <ESP32SerialCtl.h>

static esp32serialctl::ESP32SerialCtl<> esp32SerialCtl;

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  Wire1.begin();
  delay(500);

  if (!LittleFS.begin(true))
  {
    Serial.print("LittleFS Mount Failed");
  }
  else
  {
    File file = LittleFS.open("/test.txt", "w");
    file.printf("test string\n");
    file.close();
  }

  esp32SerialCtl.setPinAllAccess(false);
  esp32SerialCtl.setPinName(GPIO_NUM_2, "LED");
  esp32SerialCtl.setPinName(GPIO_NUM_16, "RGB LED");
  esp32SerialCtl.setDefaultRgbPin(GPIO_NUM_16);
}

void loop()
{
  esp32SerialCtl.service();
}
