#include <Arduino.h>
#include <ESP32SerialCtl.h>

int handle_rgb(const char **argv, size_t argc, void *ctx)
{
    auto ctl = reinterpret_cast<esp32serialctl::ESP32SerialCtl<> *>(ctx);
    if (!ctl)
    {
        Serial.println("ERR 500 No context");
        return -1;
    }

    int r = argc > 0 ? atoi(argv[0]) : 0;
    int g = argc > 1 ? atoi(argv[1]) : 0;
    int b = argc > 2 ? atoi(argv[2]) : 0;

    // (Perform any device-side action here, e.g. set PWM for an RGB LED)
    char buf[64];
    snprintf(buf, sizeof(buf), "rgb handler: r=%d g=%d b=%d", r, g, b);
    ctl->printBody(buf);

    snprintf(buf, sizeof(buf), "rgb %d %d %d", r, g, b);
    ctl->printOK(buf);

    return 0;
}

int handle_ping(const char **argv, size_t argc, void *ctx)
{
    // Cast ctx to the concrete controller type and require it.
    auto ctl = reinterpret_cast<esp32serialctl::ESP32SerialCtl<> *>(ctx);
    if (!ctl)
    {
        Serial.println("ERR 500 No context");
        return -1;
    }
    (void)argv;
    (void)argc;

    String value = ctl->configGet("pong");
    // Return the stored response, then an OK marker for success.
    ctl->printBody(value.c_str());
    ctl->printOK("ping");
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
