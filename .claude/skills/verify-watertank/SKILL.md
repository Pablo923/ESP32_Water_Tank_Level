---
name: verify-watertank
description: Prove behavior of the ESP32 Water Tank Level firmware (ESP-IDF 5.1.1, esp32). Builds the image, flashes a connected board, and records UART0 serial output as evidence of boot, RAK3172 LoRa P2P setup, water-level readings, LoRa transmits, and the serial echo task. Use before declaring any firmware change done, or when asked to verify, flash, or check serial output.
---

# Verify the water tank firmware

The user-facing surface is the board itself: the UART0 serial log (115200 baud, shared by `ESP_LOG` output and the app's `SendUART_Message`), the TM1637 4-digit display, and LoRa P2P packets sent by the RAK3172 module. The only surface an agent can observe directly is the serial log. The display and the receiving LoRa node need a human or a second device.

All commands go through `.claude/skills/verify-watertank/scripts/verify.sh` (run from the repo root). Set one `RUN_ID` per verification run so every artifact lands in the same folder:

```sh
export RUN_ID=$(date -u +%Y%m%dT%H%M%SZ)
V=.claude/skills/verify-watertank/scripts/verify.sh
```

## Tiers

- **Build tier** needs only ESP-IDF at `$HOME/esp/esp-idf`. It proves the firmware compiles and fits the partition. It is never proof of runtime behavior.
- **Hardware tier** needs an ESP32 board on USB (`$V ports` lists candidates). It proves runtime behavior from serial output. If no port is listed, report every hardware journey as `skipped: no board connected`. Never report them as verified from the build tier.

## Launch

1. `$V build`. First build takes about 2.5 min and creates `build/` and a local `sdkconfig` (gitignored). Ready means the output shows `build exit=0` and a `binary size ... free` line. Incremental builds take seconds.
2. Hardware tier only: `$V flash <PORT>`. Ready means `flash exit=0` and `Hash of data verified` in the output. If flashing stalls at `Connecting...`, a human must hold **BOOT** on the board. Ask once, then report the journey as blocked.
3. `$V capture <PORT> <SECONDS> --reset` resets the chip through RTS and records from boot. It runs in the foreground and exits on its own, so there is no server to keep alive.

Teardown: nothing stays running. Capture is time-bounded. The board keeps running the flashed image, which is the intended end state.

## Doctor

`$V doctor [PORT]` is read-only. It checks the ESP-IDF path and version (`v5.1.1`), whether the image in `build/` is newer than every source under `main/` and `components/`, whether firmware sources are dirty, and, given a port, that the port exists and no other process holds it. Run it first, and again whenever anything looks off. A stale build means flashing would test old code. Rebuild first.

If the port is held (usually a human's `idf.py monitor`), stop. It is not ours. Ask the user to close it and never kill it.

## Drive

Read [`features/README.md`](features/README.md), then follow the feature file that matches the change. Every journey on the hardware tier is: build → doctor with port → flash → capture → assert on lines in the capture log.

The capture log prefixes every line with host-side elapsed seconds, `[  12.34] ...`, so cadence is checkable. Use `--send "TEXT"` to write to UART0 one second after opening.

## Evidence

Artifacts live in `.verify-artifacts/<RUN_ID>/` (gitignored): `build.log`, `build.diagnostics.txt` (warnings and errors in project sources only), `flash.log`, and `serial-HHMMSS.log`. `$V artifacts` prints the folder.

Proof standards:
- Assert on serial lines the firmware actually printed after a fresh flash of the current HEAD. A capture from an image built before the change proves nothing about the change.
- Capture the action and the result: the reset or send, and the lines that follow it.
- For LoRa, the RAK3172's own response lines (`[RAK3172] ...`) are the side effect. The ESP32 never echoes the AT command it sends, so its absence from the log is normal. A packet on the receiving node is stronger proof. Note in your report whether you had one.
- Display output (digits or `----`) needs a human or photo. Say so rather than inferring it from the log.
- Quote the relevant log lines with their timestamps in your report, and name the artifact path.

## Cleanup

There are no processes to kill: `verify.sh` starts none that outlive the command. Do not run `make clean` (it wipes the 2.5 min build cache) or `make erase-flash` unless the task asks for it. Never delete `.verify-artifacts/`. It is the evidence. A human prunes it.

## Isolation

There is one board, so only one agent at a time may flash or capture. `doctor` refuses a held port, and that is the lock. Two agents building in the same checkout share `build/` and will corrupt each other. Parallel agents each need their own worktree, and only the one holding the board runs hardware journeys.

## Helpers

- `scripts/verify.sh`: run with no arguments for usage.
- `scripts/serial_capture.py`: the capture engine, called by `verify.sh capture` inside the ESP-IDF Python env (it needs `pyserial`, which ESP-IDF ships). It works without a TTY, unlike `idf.py monitor`.
