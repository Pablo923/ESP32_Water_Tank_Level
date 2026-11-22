# Water tank firmware verification map

This directory is the maintained source for verifying the user-facing behavior of the ESP32 Water Tank Level firmware. Read this index before driving the board, then use the matching feature file as the recipe.

## Baseline preconditions

- Run from the repo root with `V=.claude/skills/verify-watertank/scripts/verify.sh` and one exported `RUN_ID`.
- `$V doctor` reports `doctor: healthy`. For the hardware tier, run `$V doctor <PORT>`.
- Hardware tier: an ESP32 on USB with the RAK3172 on UART2 (TX GPIO17, RX GPIO16), the HC-SR04 on TRIG GPIO26 and ECHO GPIO25, and the TM1637 on CLK GPIO18 and DIO GPIO19.
- Never flash or capture on a port another process holds.

## Driving conventions

- Every hardware journey starts from a fresh `$V flash <PORT>` of the current build.
- Use `$V capture <PORT> <SECONDS> --reset` to observe from boot. Use it without `--reset` to observe steady state.
- Match log lines by their stable text: the `WATERTANK_APP:` tag and message, and the `[RAK3172] ` and `[SERIAL] ` prefixes.
- The main loop period is about 5 s plus measurement time (up to about 1 s when no echo arrives). Capture at least 20 s to see several cycles.

## Proof and skip reporting

- Build proof: `build exit=0`, the `binary size` line, and `build.diagnostics.txt`.
- Serial proof: the capture log path plus quoted lines with timestamps.
- A journey that needs hardware with no board present is reported as `skipped: no board connected`.
- A journey that needs a peripheral that is absent (sensor, RAK3172, LoRa receiver) is reported as failed or partial, with the missing peripheral named. Never weaken the assertion to pass.
- Display and receiving-node checks are reported as `needs human` unless a human or second device confirmed them.

## Feature entry contract

Each feature file has an H1 title, one paragraph of user-visible behavior, then exactly these H2s in order: `Sub-features`, `How to get to it (user POV)`, `Driving it with verify.sh`, `Gotchas`.

## Features

- [Firmware build](./build.md) checks that the image compiles and fits, with no new project warnings.
- [Boot and LoRa P2P setup](./boot-p2p-init.md) checks the startup log and the RAK3172 accepting P2P configuration.
- [Water level measurement](./water-level.md) checks the periodic level reading or error report, and the display.
- [LoRa transmit](./lora-transmit.md) checks that each valid reading is sent as `AT+PSEND` and acknowledged by the RAK3172.
- [Serial echo](./serial-echo.md) checks that text sent to UART0 comes back prefixed with `[SERIAL] `.
