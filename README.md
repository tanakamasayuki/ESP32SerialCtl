# ESP32SerialCtl

English | [日本語](README.ja.md)

ESP32SerialCtl is a header-only serial CLI library for Arduino-compatible ESP32 projects. It targets a predictable, lightweight text protocol that works well for both humans and automation. The sources follow the Arduino library layout (`src/ESP32SerialCtl.h`), so you can copy the folder into your `libraries` directory and start using it.

## WebSerial Console

A WebSerial control panel is available on GitHub Pages. Connect from your browser to an ESP32 running ESP32SerialCtl to send commands and manage files.

- URL: https://tanakamasayuki.github.io/ESP32SerialCtl/

## Features
- Header-only: include `ESP32SerialCtl.h` and begin using it
- Template parameters let you size the working buffers so RAM usage stays minimal
- Quoted strings support C-style escapes such as `\"`, `\n`, `\t`, and `\\`
- Options are parsed in both `--name value` and `--name=value` forms
- Helper APIs format responses that follow the `OK`, `ERR`, ` - `, and `| ` conventions
- When both Wi-Fi and Preferences are available, the `wifi` and `ntp` commands persist settings in NVS to control auto-connect and NTP synchronization

## Getting Started

The easiest way to use ESP32SerialCtl is to rely on the built-in command set provided by `esp32serialctl::ESP32SerialCtl`. Declare a global instance, initialize the serial port in `setup()`, and process input inside `loop()`.

```cpp
#include <Arduino.h>
#include <ESP32SerialCtl.h>

static esp32serialctl::ESP32SerialCtl<> esp32SerialCtl;

void setup() {
  Serial.begin(115200);
}

void loop() {
  esp32SerialCtl.service();
}
```

The default configuration ships with the `sys` group (`info`, `uptime`, `mem`, `reset`) and the `help` / `?` commands, so you can retrieve useful information without adding any user code.

The prompt prints automatically on the first call to `service()` and after every response (`OK` / `ERR`).

### Static Command Registration (CommandEntry)

ESP32SerialCtl supports registering commands by passing a statically defined `CommandEntry` array to the constructor. This mirrors how `ConfigEntry` works and keeps names, localized descriptions, and argument specifications together in your sketch.

Key types and constants (defined in `ESP32SerialCtl.h`):

- `struct CmdArgSpec` — argument name, type, required flag, and hint
- `using CmdHandlerFn = int (*)(const char **argv, size_t argc, void *ctx)` — user handler signature
- `struct CommandEntry` — `{ const char *name; LocalizedText descriptions[...] ; CmdArgSpec args[...] ; CmdHandlerFn handler; }`
- `ESP32SERIALCTL_CMD_ARG_MAX` — maximum number of arguments per command (default 8)

Example usage (see `examples/CommandDemo` for a full sketch):

```cpp
static const esp32serialctl::CommandEntry kAppCommands[] = {
  {"ping", {{"en", "Reply with pong"}}, {{"", "", false, ""}}, handle_ping},
  {"rgb",  {{"en", "Set RGB"}},         {/* arg specs */},      handle_rgb},
};

static esp32serialctl::ESP32SerialCtl<> esp32SerialCtl(kAppConfig, kAppCommands);
```

Lifecycle and ownership notes:

- When commands are registered, the library allocates an internal `Command` array that combines the built-in entries with the user-defined entries. The library owns that allocation.
- Re-registering commands destructs and frees the previously allocated array automatically. The built-in static table `kCommands` is never freed.
- The registration API is not thread-safe; avoid calling it from multiple contexts simultaneously.

Refer to `examples/CommandDemo` if you need to inspect a working example.

## Built-in Commands
- `sys info` prints the chip model, revision, CPU clock, flash size, and SDK version.
- `sys uptime` reports uptime in `hh:mm:ss` plus raw milliseconds.
- `sys time` shows the current time in ISO 8601 format.
- `sys timezone` reads or writes the persisted timezone string (also used for NTP).
- `sys mem` lists heap/PSRAM totals, free, minimum, and largest blocks, and stack headroom.
- `sys reset` confirms the request and calls `ESP.restart()`.
- `conf list/get/set/del` stores and retrieves application-defined configuration slots in NVS. Run `conf list --lang ja` to select a specific language. Defaults are registered through `ESP32SerialCtl::setConfigEntries`.
- `storage list/use/status` enumerates linked storage backends such as `SD`, `SPIFFS`, `LittleFS`, or `FFat`, and shows capacity and utilization.
- `fs ls/cat/write/rm/stat/mkdir/mv` lists directories, inspects files, writes text, deletes entries, shows metadata, creates directories, and renames files on the active storage (or one specified with `--storage <name>`).
- `fs b64read/b64write` transfers binary data in Base64-encoded chunks; combine with `--append` for multi-chunk uploads.
- `fs hash` computes file digests (`sha256` by default, optional `md5`) and reports the byte size to verify transfers.
- `gpio mode/read/write/toggle/pins` manages GPIO modes, reads or writes pins, toggles outputs, and prints the access control list.
- `adc read` samples an ADC channel; use the `samples` option to average multiple reads.
- `pwm set/stop` starts or stops LEDC PWM with automatic channel assignment (`pwm set <pin> <freq> <duty>`). The duty cycle uses a fixed 12-bit resolution and accepts raw `0..4095` or percentages.
- `rgb pin/set/stream` drives addressable RGB LEDs via `rgbLedWrite`, including default-pin support.
- `i2c scan/read/write` becomes available when `Wire` is included; on multi-controller boards you can target `Wire`, `Wire1`, etc. with `--bus`.
- `wifi auto/list/add/del/connect/disconnect/status` manages Wi-Fi credentials stored in NVS, controls automatic connection, and displays the current link state.
- `ntp status/set/enable/disable/auto` configures NTP servers, inspects sync status, and toggles automatic start (set `sys timezone` before use).
- `help` / `?` lists registered commands with their descriptions.

Set the default RGB pin from your sketch with `esp32serialctl::ESP32SerialCtl<>::setDefaultRgbPin(pin);`.
When the platform defines `RGB_BUILTIN`, that value is picked up automatically.

Expose application configuration to the CLI by registering a `ConfigEntry` array at startup:

```cpp
static constexpr esp32serialctl::ConfigEntry kAppConfig[] = {
    {"api_key", "", {{"ja", "外部API連携用のトークン"}, {"en", "External API key"}}},
};

static esp32serialctl::ESP32SerialCtl<> esp32SerialCtl(kAppConfig, "my_app_config");
```

`conf list` prints all localized descriptions by default; use `--lang ja` (or another language tag) to narrow the output. Fetch and update values by name with `conf get <name> [--lang ja]`, `conf set <name> "value"`, and `conf del <name>`. These commands omit descriptions and focus on status and values. In code, call `esp32SerialCtl.configGet("api_key")` to read the effective value. If you need to change the namespace or entries at runtime, the static helpers `ESP32SerialCtl<>::setConfigNamespace(...)` and `ESP32SerialCtl<>::setConfigEntries(...)` remain available. When no `ConfigEntry` array is registered, the `conf` commands are disabled and hidden from help.

## GPIO Access Control

GPIO commands default to an "allow all" mode. Calling `esp32SerialCtl.setPinAllAccess(false);` switches to a whitelist mode where only explicitly allowed pins can be used from the `gpio`, `adc`, `pwm`, or `rgb` groups. Registering an alias such as `setPinName(GPIO_NUM_2, "LED");` both names the pin and allows it, letting you write CLI commands like `gpio read LED`. To adjust permissions without changing the alias, use `setPinAllowed(pin, true/false);`.

Check the current mode, allow-list, and registered aliases with `gpio pins`. In unrestricted mode the output shows pins that were explicitly named or allowed; in restricted mode it lists every pin with its allow/deny state and alias.

## Time and Network Helpers

### Timekeeping (`sys`)
- `sys time [datetime]` prints the current local time when called without arguments. Passing an ISO 8601 timestamp such as `2024-04-05T12:34:56` updates the device clock immediately.
- `sys timezone [tz]` reads or writes the TZ string stored in NVS (namespace `serial_ctl`). The saved value is applied on startup and before any NTP reconfiguration. Define `ESP32SERIALCTL_DEFAULT_TIMEZONE` at compile time to change the default (empty string by default).
- Updating the timezone also affects manual `sys time` settings and SNTP sessions triggered through the `ntp` commands.

### Wi-Fi Commands
- `wifi auto <on/off>` stores whether the library should connect automatically during the first `service()` call. Automatic connection is enabled by default and controlled by the `wifi_auto` preference.
- `wifi list` enumerates saved access points along with the slot index in NVS.
- `wifi add <ssid> <key>` saves credentials in the first free slot (`wifi{N}_ssid`, `wifi{N}_key`). Up to `ESP32SERIALCTL_WIFI_MAX_NETWORKS` entries are stored (default 8), and the returned index can be used with `wifi del`.
- `wifi del <index>` deletes the entry referenced by the index shown in `wifi list`.
- `wifi connect [ssid] [key]` attempts to connect using the saved list via `WiFiMulti`. Optional arguments add a temporary network without persisting it. The timeout is controlled by `ESP32SERIALCTL_WIFI_CONNECT_TIMEOUT_MS` (default 10 seconds), and NTP is refreshed after a successful connection.
- `wifi disconnect` and `wifi status` drop the current link or show link state, MAC, IP, RSSI, channel, and the auto-connect setting.

### NTP Commands
- `ntp status` reports auto/enable flags, the current timezone, configured servers, and the latest synchronization status.
- `ntp set <server> [server2] [server3]` saves up to `ESP32SERIALCTL_NTP_MAX_SERVERS` entries (default 3). Override `ESP32SERIALCTL_DEFAULT_NTP_SERVER` to change the factory default (`pool.ntp.org`).
- `ntp enable` starts SNTP after verifying that a timezone has been set. It waits up to 30 seconds for the initial sync and prints the timestamp on success.
- `ntp disable` stops SNTP and clears the enabled flag.
- `ntp auto <on/off>` toggles whether SNTP should start automatically once Wi-Fi is connected and a timezone is present.

## Examples

`examples/BasicCli/BasicCli.ino` reproduces the minimal setup above, so you can experiment with the CLI immediately after adding the library.

## License

ESP32SerialCtl is released under the MIT license. See `LICENSE` for details.

[Japanese version](README.ja.md)
