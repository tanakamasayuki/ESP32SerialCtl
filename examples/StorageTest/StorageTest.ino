// en: Inspect mount results for several storage backends
// ja: 複数のストレージのmount結果を調べる
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
    // en: Initialize this component after its prerequisites; inspect the returned status where provided.
    // ja: 前提となる機器を準備してから初期化し、結果を返すAPIでは成否を確認します。
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
    // en: Process pending commands regularly; blocking the caller postpones new input.
    // ja: 保留コマンドを定期的に処理します。呼び出し元を止めると次の入力も遅れます。
    esp32SerialCtl.service();
    delay(1);
}
