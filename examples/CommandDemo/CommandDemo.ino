#include <Arduino.h>
#include <ESP32SerialCtl.h>

int handle_rgb(const char **argv, size_t argc, void *ctx)
{
    auto ctl = reinterpret_cast<esp32serialctl::ESP32SerialCtl<> *>(ctx);

    if (argc != 3)
    {
        ctl->printError(400, "Expected 3 arguments for r, g, b");
        return -1;
    }

    int r = atoi(argv[0]);
    int g = atoi(argv[1]);
    int b = atoi(argv[2]);

    ctl->printOK("rgb %d %d %d", r, g, b);
    ctl->printBody("rgb handler: r=%d g=%d b=%d", r, g, b);

    return 0;
}

int handle_ping(const char **argv, size_t argc, void *ctx)
{
    auto ctl = reinterpret_cast<esp32serialctl::ESP32SerialCtl<> *>(ctx);

    String value = ctl->configGet("pong");

    ctl->printOK("ping");
    ctl->printBody(value.c_str());

    return 0;
}

static constexpr esp32serialctl::CommandEntry kAppCommands[] = {
    {
        "rgb",
        // descriptions (max locales will be zero-filled if fewer provided)
        {
            {"ja", "RGB色を設定します"},
            {"en", "Set RGB color"},
        },
        // args (fixed-size array inside CommandEntry; unused slots should be zero)
        {
            {"r", "int", true, "0-255"},
            {"g", "int", true, "0-255"},
            {"b", "int", true, "0-255"},
        },
        // handler
        handle_rgb,
    },
    {
        "ping",
        {
            {"ja", "疎通確認"},
            {"en", "Ping command"},
        },
        {
            // no args
        },
        // handler
        handle_ping,
    },
};

static constexpr esp32serialctl::ConfigEntry kAppConfigEntries[] = {
    {
        "pong",
        "Pong!",
        {
            {"ja", "pingに対する応答文字列"},
            {"en", "Response string for ping command"},
        },
    },
};

// Instantiate controller with config entries empty and commands passed in
static esp32serialctl::ESP32SerialCtl<> esp32SerialCtl(kAppConfigEntries, kAppCommands);

void setup()
{
    Serial.begin(115200);
    delay(200);
    Serial.println();
    Serial.println("ESP32SerialCtl Command demo (experimental)");
}

void loop()
{
    esp32SerialCtl.service();
    delay(1);
}
