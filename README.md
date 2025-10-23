# ESP32SerialCtl

English | [日本語](README.ja.md)

ESP32SerialCtl is a header-only serial command line interface helper for
Arduino compatible ESP32 projects. It focuses on a predictable, low-overhead
text protocol that is convenient for both human operators and automation.
Library sources follow the Arduino layout (`src/ESP32SerialCtl.h`) so the
project can be dropped straight into your `libraries` folder.

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
  esp32SerialCtl.service();
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
- `sys mem` lists heap/PSRAM totals, free, minimum, and largest blocks plus stack watermarks.
- `sys reset` confirms the request, flushes output, then calls `ESP.restart()`.
- `storage list/use/status` exposes whichever storage backends have been linked in (`SD`, `SPIFFS`, `LittleFS`, `FFat`). Use `storage use <name>` to select the active target and `storage status [name]` to inspect capacity and usage.
- `fs ls/cat/write/rm/stat/mkdir/mv` manipulate the filesystem on the active storage (or one specified via `--storage <name>`), covering directory listings, file inspection, text writes, deletions, and renames.
- `fs b64read/b64write` move arbitrary binary blobs by streaming Base64-encoded chunks; pair with `--append` for multi-chunk uploads.
- `fs hash` computes file digests (`sha256` default, `md5` optional) and reports byte size to confirm transfers.
- `gpio mode/read/write/toggle` manipulate pins with familiar semantics.
- `adc read` samples an ADC channel with optional averaging.
- `pwm set/stop` manage LEDC PWM (`pwm set <pin> <freq> <duty>`, 12-bit resolution; duty accepts raw 0..4095 or percent).
- `rgb pin/set/stream` control addressable RGB LEDs via `rgbLedWrite`, with configurable defaults and streaming support.
- `i2c scan/read/write` becomes available automatically when `Wire` is included; use `--bus` to target `Wire`, `Wire1`, etc. on multi-controller boards.
- `help` / `?` enumerates registered commands with their descriptions.

Library users can also configure the default RGB pin from code via
`esp32serialctl::ESP32SerialCtl<>::setDefaultRgbPin(pin);`. When the platform
defines `RGB_BUILTIN`, that pin is picked up automatically.

## Examples

`examples/BasicCli/BasicCli.ino` mirrors the minimal setup shown above so you
can upload and interact with the CLI immediately after adding the library.

## License

ESP32SerialCtl is distributed under the terms of the MIT license. See
`LICENSE` for details.
