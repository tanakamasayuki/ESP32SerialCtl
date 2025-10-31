# ESP32SerialCtl

English | [日本語](README.ja.md)

ESP32SerialCtl is a header-only serial command line interface helper for
Arduino compatible ESP32 projects. It focuses on a predictable, low-overhead
text protocol that is convenient for both human operators and automation.
Library sources follow the Arduino layout (`src/ESP32SerialCtl.h`) so the
project can be dropped straight into your `libraries` folder.

## WebSerial Console

A WebSerial-powered control surface is available on GitHub Pages. You can open
it in a supported browser, connect to an ESP32 running ESP32SerialCtl, and run
commands or manage the filesystem without installing additional tools.

- URL: https://tanakamasayuki.github.io/ESP32SerialCtl/

## Features
- Header-only: just include `ESP32SerialCtl.h`
- Minimal RAM usage, user-provided buffers with configurable limits
- Parsing for `group action [args]` commands
- Multiple commands per line using `;`, with `#` line comments
- Quoted arguments with C-style escapes (`\"`, `\n`, `\t`, `\\`)
- Options in `--name value` or `--name=value` form
- Helpers for boolean (`on/off/true/false/1/0`) and numeric arguments
  (decimal, hex, units `ms`, `s`, `Hz`, `kHz`, `%`)
- Output helpers for the strict response format (`OK`, `ERR`, body/list rows)
- Optional Wi-Fi credential manager and NTP auto-sync commands when both Wi-Fi and Preferences are available (stored in NVS)

## Getting Started

The simplest setup uses the built-in command set provided by
`esp32serialctl::ESP32SerialCtl`. Create a global instance, start the
port in `setup`, and process input inside `loop`.

```cpp
#include <Arduino.h>
#include <ESP32SerialCtl.h>

static esp32serialctl::ESP32SerialCtl<> esp32SerialCtl;

void setup() {
  Serial.begin(115200);
}

void loop() {
```

### Static command registration (CommandEntry)
  esp32SerialCtl.service();
As an alternative to building a `SerialCtl::Command` table by hand, the
library now accepts user-defined `CommandEntry` arrays that are declared as
static data in your sketch and passed to `ESP32SerialCtl` at construction
time. This mirrors how `ConfigEntry` arrays are registered and is convenient
for keeping command metadata (names, localized descriptions, argument
specs) close to the code that implements the handlers.

Key types and constants (in `ESP32SerialCtl.h`):

- `struct CmdArgSpec` — argument name/type/required/hint
- `using CmdHandlerFn = int (*)(const char **argv, size_t argc, void *ctx)` —
  user handler signature
- `struct CommandEntry` — `{ const char *name; LocalizedText descriptions[...] ; CmdArgSpec args[...] ; CmdHandlerFn handler; }`
- `ESP32SERIALCTL_CMD_ARG_MAX` — max number of args per command (default 8)

Usage example (see `examples/CommandDemo` for a complete demo):

```cpp
static const esp32serialctl::CommandEntry kAppCommands[] = {
  {"ping", {{"en","Reply with pong"}}, {{"", "", false, ""}}, handle_ping},
  {"rgb",  {{"en","Set RGB"}},               {/* arg specs */},   handle_rgb},
};

static esp32serialctl::ESP32SerialCtl<> esp32SerialCtl(kAppConfig, kAppCommands);
```

Lifecycle and ownership notes:

- When you register a `CommandEntry` array (either by passing it to the
  constructor or calling the registration helper), the library allocates an
  internal combined `Command` array that contains the built-in commands plus
  your entries. That allocation is owned by the library.
- If you re-register commands later, the library will destruct and free the
  previously-allocated combined array automatically. The built-in `kCommands`
  (static table shipped with the library) is never freed.
- Registration is not thread-safe; avoid calling the registration API from
  multiple contexts concurrently.

If you need to inspect or debug, consult `examples/CommandDemo` which shows a
minimal working sketch using `CommandEntry` and a pair of example handlers.

}
```

This default configuration ships with the `sys` group (`info`, `uptime`, `mem`,
`reset`) plus `help`/`?` commands, so it responds usefully without any custom
handlers. The prompt prints automatically on the first `service()` call
and after each response (`OK` or `ERR`).

## Custom Commands

When you need bespoke functionality, build your own command table on top of
`SerialCtl` and plug in handlers through the provided context helpers.

```cpp
#include <ESP32SerialCtl.h>

using CLI = esp32serialctl::SerialCtl<192, 16>;

static void handlePing(CLI::Context &ctx) {
  ctx.printBody("pong");
  ctx.printOK("ping");
}

static const CLI::Command COMMANDS[] = {
    {nullptr, "ping", handlePing, "Reply with pong"},
};

static CLI cli(Serial, COMMANDS, sizeof(COMMANDS) / sizeof(COMMANDS[0]));

void setup() {
  Serial.begin(115200);
}

void loop() {
  cli.service();
}
```

### Parsing arguments
- `ctx.arg(index).toBool(value)` parses boolean tokens such as `on` or `false`.
- `ctx.arg(index).toNumber(parsed)` handles decimal and hexadecimal numbers,
  applying unit scaling to milliseconds, hertz, or percent.
- Options are available through `ctx.findOption("name")`, with support for the
  same helper methods (`toBool`, `toNumber`, `valueEquals`, etc.).

### Emitting responses
- `ctx.printOK("message")` -> `OK message`
- `ctx.printError(code, "reason")` -> `ERR code reason`
- `ctx.printList("text")` -> ` - text`
- `ctx.printBody("text")` -> `| text`

### Error codes used by the parser
- `ERR 404 Unknown command`
- `ERR 431 Too many options`
- `ERR 413 Too many tokens`
- `ERR 500 No handler`
- `ERR 508 Missing closing quote`
- `ERR 510 Line too long`

These are emitted automatically before the command handler is called.

## Built-in Commands
- `sys info` prints chip model, revision, clock, flash, SDK/IDF, build info.
- `sys uptime` reports uptime in `hh:mm:ss` and raw milliseconds.
- `sys time` returns the current local time in ISO 8601 format.
- `sys timezone` gets/sets the persisted TZ string (also used by NTP).
- `sys mem` lists heap/PSRAM totals, free, minimum, and largest blocks plus stack watermarks.
- `sys reset` confirms the request, flushes output, then calls `ESP.restart()`.
- `conf list/get/set/del` expose application-defined configuration slots backed by NVS (`conf list --lang ja` shows localized descriptions). Defaults come from `ESP32SerialCtl::setConfigEntries`.
- `storage list/use/status` exposes whichever storage backends have been linked in (`SD`, `SPIFFS`, `LittleFS`, `FFat`). Use `storage use <name>` to select the active target and `storage status [name]` to inspect capacity and usage.
- `fs ls/cat/write/rm/stat/mkdir/mv` manipulate the filesystem on the active storage (or one specified via `--storage <name>`), covering directory listings, file inspection, text writes, deletions, and renames.
- `fs b64read/b64write` move arbitrary binary blobs by streaming Base64-encoded chunks; pair with `--append` for multi-chunk uploads.
- `fs hash` computes file digests (`sha256` default, `md5` optional) and reports byte size to confirm transfers.
- `gpio mode/read/write/toggle/pins` manipulate pins, toggle outputs, and list the access control table.
- `adc read` samples an ADC channel with optional averaging.
- `pwm set/stop` manage LEDC PWM (`pwm set <pin> <freq> <duty>`, 12-bit resolution; duty accepts raw 0..4095 or percent).
- `rgb pin/set/stream` control addressable RGB LEDs via `rgbLedWrite`, with configurable defaults and streaming support.
- `i2c scan/read/write` becomes available automatically when `Wire` is included; use `--bus` to target `Wire`, `Wire1`, etc. on multi-controller boards.
- `wifi auto/list/add/del/connect/disconnect/status` manage Wi-Fi credentials stored in NVS, control automatic connection on boot, and inspect the current link.
- `ntp status/set/enable/disable/auto` configure SNTP servers, enable/disable synchronization, and toggle automatic start (set `sys timezone` before enabling).
- `help` / `?` enumerates registered commands with their descriptions.

Library users can also configure the default RGB pin from code via
`esp32serialctl::ESP32SerialCtl<>::setDefaultRgbPin(pin);`. When the platform
defines `RGB_BUILTIN`, that pin is picked up automatically.

Application settings can be exposed to the CLI by registering a `ConfigEntry`
array at startup:

```cpp
static constexpr esp32serialctl::ConfigEntry kAppConfig[] = {
    {"api_key", "", {{"en", "External API key"}, {"ja", "外部APIキー"}}},
};

static esp32serialctl::ESP32SerialCtl<> esp32SerialCtl(kAppConfig, "my_app_config");
```

The `conf` command group lets users list entries (`conf list` shows all
localized descriptions unless `--lang ja` narrows the output), inspect values
by name (`conf get <name> [--lang ja]`), update them (`conf set <name> "value"`),
and delete stored overrides (`conf del <name>`). Only `conf list` prints the
localized descriptions; other commands report status and values. Code can read
the effective value at runtime with `esp32SerialCtl.configGet("api_key")`. If
the entries or namespace need to be chosen dynamically (for example, after
reading a manifest from storage), the static helpers
`ESP32SerialCtl<>::setConfigNamespace(...)` and
`ESP32SerialCtl<>::setConfigEntries(...)` remain available.
If no `ConfigEntry` array is registered, the `conf` commands are disabled and
remain hidden from the CLI/help output.

## GPIO Access Control

GPIO commands now respect an allow-list that you configure at runtime. By
default every pin is accessible. Calling `esp32SerialCtl.setPinAllAccess(false)`
switches to restricted mode, where only pins explicitly marked as allowed can
be used from `gpio`, `adc`, `pwm`, or `rgb` commands. Use
`setPinName(GPIO_NUM_2, "LED")` (or the `int` overload) to assign a human
readable alias and implicitly allow that pin, and call `setPinAllowed(pin, true)`
or `false` to adjust access without changing the alias. Once an alias is
registered you can refer to it directly in CLI commands, e.g. `gpio read LED`.

Run `gpio pins` to dump the current mode, the allow-list, and any aliases you
have defined. In unrestricted mode the command shows which pins were explicitly
whitelisted or named.

## Time and Network Helpers

### Timekeeping (`sys`)
- `sys time [datetime]` prints the current local time when called without arguments. Passing an ISO 8601 timestamp such as `2024-04-05T12:34:56` updates the device clock immediately.
- `sys timezone [tz]` reads or writes the TZ environment string stored in NVS (namespace `serial_ctl`). The saved value is applied on startup and before any NTP reconfiguration. Define `ESP32SERIALCTL_DEFAULT_TIMEZONE` at compile time to change the fallback (defaults to an empty string).
- Timezone changes propagate to both manual `sys time` adjustments and SNTP sessions triggered through the `ntp` helpers.

### Wi-Fi command group
- `wifi auto <on/off>` stores whether the library should connect automatically during the first `service()` call. Automatic connection is enabled by default and governed by the `wifi_auto` preference.
- `wifi list` enumerates stored access points, including the backing slot index inside NVS.
- `wifi add <ssid> <key>` persists credentials in the first free slot (keys `wifi{N}_ssid`, `wifi{N}_key`) up to `ESP32SERIALCTL_WIFI_MAX_NETWORKS` entries (default 8). The command returns the logical list index so it can be paired with `wifi del`.
- `wifi del <index>` removes an entry by the list index printed by `wifi list`.
- `wifi connect [ssid] [key]` connects using the stored list through `WiFiMulti`. Optional arguments add a temporary network without storing it. The operation uses the timeout defined by `ESP32SERIALCTL_WIFI_CONNECT_TIMEOUT_MS` (default 10 s) and refreshes NTP once the link comes up.
- `wifi disconnect` and `wifi status` disconnect from the current network or show the link state, MAC, IP, RSSI, channel, and auto-connect preference.

### NTP command group
- `ntp status` reports auto/enable flags, the active timezone, configured servers, and the latest synchronization status.
- `ntp set <server> [server2] [server3]` replaces the stored server list (up to `ESP32SERIALCTL_NTP_MAX_SERVERS`, default 3). `ESP32SERIALCTL_DEFAULT_NTP_SERVER` overrides the factory default `pool.ntp.org`.
- `ntp enable` starts SNTP after validating that a timezone has been set. The command waits up to 30 s for an initial sync and records the timestamp when it completes.
- `ntp disable` stops SNTP and clears the enabled preference.
- `ntp auto <on/off>` toggles whether SNTP should start automatically once Wi-Fi is connected and a timezone is present.

## Examples

`examples/BasicCli/BasicCli.ino` mirrors the minimal setup shown above so you
can upload and interact with the CLI immediately after adding the library.

## License

ESP32SerialCtl is distributed under the terms of the MIT license. See
`LICENSE` for details.
