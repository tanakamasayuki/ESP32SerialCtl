#include <Arduino.h>
#include <ESP32SerialCtl.h>

static constexpr esp32serialctl::ConfigEntry kAppConfigEntries[] = {
    {
        "api_key",
        "",
        {
            {"ja", "外部APIと連携するための認証キー"},
            {"en", "Authentication token for external API integration"},
            // 残り6要素は自動で {nullptr, nullptr}
        },
    },
    {
        "ai_persona",
        "friendly",
        {
            {"ja", "AI応答のキャラクタ（例: 優しい / まじめ）"},
            {"en", "AI response persona (e.g., friendly, serious)"},
        },
    },
    {
        "retry_limit",
        "3",
        {
            {"ja", "APIリトライ上限（数値）"},
            {"en", "API retry limit (numeric)"},
        },
    },
    // 設定の上限は配列のサイズから自動的に判断
};

static esp32serialctl::ESP32SerialCtl<> esp32SerialCtl(kAppConfigEntries);

void setup()
{
    Serial.begin(115200);
    delay(200);

    Serial.println();
    Serial.println("ESP32SerialCtl Config demo");

    String value = esp32SerialCtl.configGet("api_key");
    Serial.printf("config[api_key] = %s\n", value.c_str());

    value = esp32SerialCtl.configGet("ai_persona");
    Serial.printf("config[ai_persona] = %s\n", value.c_str());

    value = esp32SerialCtl.configGet("retry_limit");
    Serial.printf("config[retry_limit] = %s\n", value.c_str());

    Serial.println();
}

void loop()
{
    esp32SerialCtl.service();
    delay(1);
}
