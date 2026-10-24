# ESP32SerialCtl — Example workflow

> 日本語版: [GUIDE_EXAMPLES.ja.md](GUIDE_EXAMPLES.ja.md)

Build and extend a serial command interface.

Check installation and platform requirements in the [library README](../README.md), then choose a sketch from the [example index](../examples/README.md). Version/support tables and internal capacity defaults stay in their original reference rather than being duplicated here.

## From setup to a verified result

1. Choose the smallest relevant sketch and check its board, hardware, dependencies and bundled files.
2. Match pins, peers and input files to your actual setup. Each README identifies important placeholders.
3. Open the .ino, select board/port and upload. Match Serial baud where used.
4. Locate the last completed stage: initialization, input, processing or output. Compilation does not test hardware I/O.
5. Reproduce the supplied example before changing one setting at a time.

## API and data handling

Begin Serial, open the monitor at the sketch baud rate and send newline-terminated commands. Call `service()` continuously; it parses commands and runs handlers in its calling context, so a slow handler delays later input. Start with `help` and `sys info`, then use only the feature groups initialized by that sketch.

CommandEntry and ConfigEntry tables should remain valid while registered. Argument descriptions do not replace numeric/range checks in a handler. Config values can persist in NVS across resets.

GPIO aliases and access policy must match your actual wiring; filesystem commands operate on the mounted partition, not an isolated demonstration store. Some examples format a filesystem after mount failure, so use test storage and review mount options before uploading.

## When adapting it

- Distinguish bool status, processed counts and handles; do not use stale output after failure.
- Registered callbacks run later from their documented context. Decide synchronization and retained-data lifetime.
- Keep bytes, samples, frames and pixels distinct; capacity is different from valid length.
- Retain views/pointers/buffers for the required period. Copying a pointer does not copy its payload.
- Add application-specific failure handling, retry and cleanup that a minimal example may omit.

## Read the individual README

Use Prepare and run for hardware and steps, Follow the sketch for the actual APIs and precautions for feature-specific constraints. If output is missing, check initialization first, continued service calls next and input/peer readiness after that.
