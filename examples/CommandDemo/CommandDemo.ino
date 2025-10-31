#include <Arduino.h>
#include <ESP32SerialCtl.h>

int handle_rgb(const char **argv, size_t argc, void *ctx);
int handle_ping(const char **argv, size_t argc, void *ctx);

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

// Simple handler signatures expected by the library (adapt if your handler
// type differs). Here we implement a minimal handler that prints parsed args.
int handle_rgb(const char **argv, size_t argc, void *ctx)
{
    // Example of using the ctx pointer if the framework provides one.
    // If ctx is non-null it may point to the controller instance; otherwise
    // fall back to the global instance. This keeps the handler flexible.
    auto ctl = ctx ? reinterpret_cast<decltype(esp32SerialCtl) *>(ctx) : &esp32SerialCtl;

    int r = argc > 0 ? atoi(argv[0]) : 0;
    int g = argc > 1 ? atoi(argv[1]) : 0;
    int b = argc > 2 ? atoi(argv[2]) : 0;

    // (Perform any device-side action here, e.g. set PWM for an RGB LED)
    Serial.printf("rgb handler: r=%d g=%d b=%d\n", r, g, b);

    // Return an OK-style response so the UI/parser can detect success.
    Serial.printf("OK rgb %d %d %d\n", r, g, b);
    (void)ctl; // suppress unused-warning if not used further
    return 0;
}

int handle_ping(const char **argv, size_t argc, void *ctx)
{
    // Demonstrate reading config via ctx (if provided) or global controller.
    auto ctl = ctx ? reinterpret_cast<decltype(esp32SerialCtl) *>(ctx) : &esp32SerialCtl;
    (void)argv;
    (void)argc;

    String value = ctl->configGet("pong");
    // Return the stored response, then an OK marker for success.
    Serial.println(value);
    Serial.println("OK ping");
    return 0;
}

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
