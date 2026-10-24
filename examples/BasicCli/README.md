# BasicCli

> 日本語版: [README.ja.md](README.ja.md)

Expose named GPIOs and an initialized test filesystem.

## Prepare and run

1. Check the [example guide](../../docs/GUIDE_EXAMPLES.md) for setup and hardware. Use the root README for supported boards and dependencies.
2. Open [BasicCli.ino](BasicCli.ino) and select the actual board/port. Prepare the dependencies used by these includes:

   `<Arduino.h>`, `<Wire.h>`, `<LittleFS.h>`, `<ESP32SerialCtl.h>`

3. Upload and open the Serial monitor at 115200 baud. Check startup messages and input conditions in the source.

## Follow the sketch

These APIs are listed in their first source order. Registering a callback does not execute its body at that point.

1. `Wire.begin(...)` — Initialize this component after its prerequisites; inspect the returned status where provided.
2. `LittleFS.open(...)` — Check that the requested input/file opened and that its type matches the intended operation.
3. `file.close(...)` — End use of the resource and stop using views or handles derived from it.
4. `esp32SerialCtl.setPinAllAccess(...)` — Choose the GPIO access policy before exposing pin-control commands.
5. `esp32SerialCtl.setPinName(...)` — Assign an alias to an actual wired GPIO and review its access permission.
6. `esp32SerialCtl.service()` — Process pending commands regularly; blocking the caller postpones new input.

## API usage and precautions

Begin Serial, open the monitor at the sketch baud rate and send newline-terminated commands. Call `service()` continuously; it parses commands and runs handlers in its calling context, so a slow handler delays later input. Start with `help` and `sys info`, then use only the feature groups initialized by that sketch.

CommandEntry and ConfigEntry tables should remain valid while registered. Argument descriptions do not replace numeric/range checks in a handler. Config values can persist in NVS across resets.

GPIO aliases and access policy must match your actual wiring; filesystem commands operate on the mounted partition, not an isolated demonstration store. Some examples format a filesystem after mount failure, so use test storage and review mount options before uploading.

This sketch writes /test.txt during setup and enables only named GPIOs. The file can be replaced each boot; review the path and mount formatting flag before using a board with existing data.

## Verify and troubleshoot

Expose named GPIOs and an initialized test filesystem and verify the result on its output. First use the supplied input, then change one configuration or data source at a time.

| Symptom | First check |
|---|---|
| No output | Board/dependencies, initialization results, output destination and Serial baud |
| Starts but stops making progress | Wait conditions, buffer space, peer/input readiness and service calls |
| Unexpected values | Lengths/units, format, data lifetime, shared state and partial results |

See the [guide](../../docs/GUIDE_EXAMPLES.md) for shared principles and the [example index](../README.md) for the next sketch.
