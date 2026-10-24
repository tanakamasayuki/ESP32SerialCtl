// en: Expose named GPIOs and an initialized test filesystem
// ja: 名前付きGPIOと初期化した試験用FSを公開する
#include <Arduino.h>
#include <Wire.h>
#include <LittleFS.h>
#include <ESP32SerialCtl.h>

static esp32serialctl::ESP32SerialCtl<> esp32SerialCtl;

void setup()
{
  Serial.begin(115200);
  // en: Initialize this component after its prerequisites; inspect the returned status where provided.
  // ja: 前提となる機器を準備してから初期化し、結果を返すAPIでは成否を確認します。
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
  // en: Process pending commands regularly; blocking the caller postpones new input.
  // ja: 保留コマンドを定期的に処理します。呼び出し元を止めると次の入力も遅れます。
  esp32SerialCtl.service();
}
