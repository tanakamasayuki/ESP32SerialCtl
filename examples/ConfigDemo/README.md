# ConfigDemo

> 日本語版: [README.ja.md](README.ja.md)

Read application settings and expose configuration commands.

## Prepare and run

1. Check the [example guide](../../docs/GUIDE_EXAMPLES.md) for setup and hardware. Use the root README for supported boards and dependencies.
2. Open [ConfigDemo.ino](ConfigDemo.ino) and select the actual board/port. Prepare the dependencies used by these includes:

   `<Arduino.h>`, `<ESP32SerialCtl.h>`

3. Upload and open the Serial monitor at 115200 baud. Check startup messages and input conditions in the source.

## Follow the sketch

These APIs are listed in their first source order. Registering a callback does not execute its body at that point.

1. `esp32SerialCtl.configGet(...)` — Read the effective configured string; a stored override can differ from the source default.
2. `esp32SerialCtl.service()` — Process pending commands regularly; blocking the caller postpones new input.

## API usage and precautions

Begin Serial, open the monitor at the sketch baud rate and send newline-terminated commands. Call `service()` continuously; it parses commands and runs handlers in its calling context, so a slow handler delays later input. Start with `help` and `sys info`, then use only the feature groups initialized by that sketch.

CommandEntry and ConfigEntry tables should remain valid while registered. Argument descriptions do not replace numeric/range checks in a handler. Config values can persist in NVS across resets.

GPIO aliases and access policy must match your actual wiring; filesystem commands operate on the mounted partition, not an isolated demonstration store. Some examples format a filesystem after mount failure, so use test storage and review mount options before uploading.

## Inputs and observable output

These are selected output formats from the source. printf specifiers are replaced by runtime values; inspect the surrounding conditions rather than assuming these lines always occur in this order.

```text
ESP32SerialCtl Config demo
config[api_key] = %s
config[ai_persona] = %s
```

## Verify and troubleshoot

Read application settings and expose configuration commands and verify the result on its output. First use the supplied input, then change one configuration or data source at a time.

| Symptom | First check |
|---|---|
| No output | Board/dependencies, initialization results, output destination and Serial baud |
| Starts but stops making progress | Wait conditions, buffer space, peer/input readiness and service calls |
| Unexpected values | Lengths/units, format, data lifetime, shared state and partial results |

See the [guide](../../docs/GUIDE_EXAMPLES.md) for shared principles and the [example index](../README.md) for the next sketch.
