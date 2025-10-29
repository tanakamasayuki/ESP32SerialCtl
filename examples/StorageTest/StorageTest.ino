#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include <SPIFFS.h>
#include <FFat.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <ESP32SerialCtl.h>

static esp32serialctl::ESP32SerialCtl<> esp32SerialCtl;

static const int PIN_SD_CS = 4;

void setup()
{
    Serial.begin(115200);
    Wire.begin();
    Wire1.begin();
    delay(500);

    // --- SD (SDSPI) ---
    SPI.begin();
    bool ok_sd = SD.begin(PIN_SD_CS, SPI, 15000000);
    Serial.printf("SD mount      : %s\n", ok_sd ? "OK" : "FAIL");

    // --- SPIFFS ---
    bool ok_spiffs = SPIFFS.begin(/*formatOnFail=*/true, /*basePath=*/"/spiffs",
                                  /*maxOpenFiles=*/10, /*partitionLabel=*/"spiffs");
    Serial.printf("SPIFFS mount  : %s\n", ok_spiffs ? "OK" : "FAIL");

    // --- LittleFS ---
    bool ok_lfs = LittleFS.begin(/*formatOnFail=*/true, /*basePath=*/"/littlefs",
                                 /*maxOpenFiles=*/10, /*partitionLabel=*/"littlefs");
    Serial.printf("LittleFS mount: %s\n", ok_lfs ? "OK" : "FAIL");

    // --- FFat (FATFS on flash) ---
    bool ok_ffat = FFat.begin(/*formatOnFail=*/true, /*basePath=*/"/ffat",
                              /*maxOpenFiles=*/10, /*partitionLabel=*/"ffat");
    Serial.printf("FFat mount    : %s\n", ok_ffat ? "OK" : "FAIL");
}

void loop()
{
    esp32SerialCtl.service();
    delay(1);
}
