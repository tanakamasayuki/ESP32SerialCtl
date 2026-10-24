// en: Use serial commands to configure and inspect Wi-Fi
// ja: シリアル操作でWi-Fiを設定・確認する
#include <WiFi.h>
#include <ESP32SerialCtl.h>

static esp32serialctl::ESP32SerialCtl<> esp32SerialCtl;

void setup()
{
    Serial.begin(115200);
    delay(500);
}

void loop()
{
    // en: Process pending commands regularly; blocking the caller postpones new input.
    // ja: 保留コマンドを定期的に処理します。呼び出し元を止めると次の入力も遅れます。
    esp32SerialCtl.service();
    delay(1);
}
