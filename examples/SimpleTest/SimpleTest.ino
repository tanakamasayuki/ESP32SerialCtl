// en: Run the minimal built-in serial command interface
// ja: 最小構成で組み込みシリアルコマンドを動かす
#include <ESP32SerialCtl.h>

static esp32serialctl::ESP32SerialCtl<> esp32SerialCtl;

void setup()
{
    Serial.begin(115200);
    delay(200);
}

void loop()
{
    // en: Process pending commands regularly; blocking the caller postpones new input.
    // ja: 保留コマンドを定期的に処理します。呼び出し元を止めると次の入力も遅れます。
    esp32SerialCtl.service();
    delay(1);
}
