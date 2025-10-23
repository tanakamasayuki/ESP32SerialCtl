#include <WiFi.h>
#include <ESP32SerialCtl.h>

static esp32serialctl::ESP32SerialCtl<> esp32SerialCtl;

void setup()
{
    Serial.begin(115200);
    delay(500);

    esp32SerialCtl.wifi_begin();

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void loop()
{
    esp32SerialCtl.service();
    delay(1);
}
