# Boot and LoRa P2P setup

On power-up or reset, the firmware logs that it is initializing. It then configures the RAK3172 for LoRa P2P by sending `AT+NWM=0`, `AT+P2P=915000000:7:125:0:10:14`, and `AT+PRECV=0`, and starts the main loop. The RAK3172's replies are printed to the serial log prefixed with `[RAK3172] `.

## Sub-features

- `boot-log` prints `WATERTANK_APP: Initializing...` after the ESP-IDF boot banner.
- `boot-p2p` has the RAK3172 reply to each of the three setup commands.
- `boot-loop` has the first main-loop line (a level reading or an error) arrive within about 4 s of `Initializing...`, after the 2.3 s of setup delays.

## How to get to it (user POV)

- Power the board on.
- Press the board's EN or RST button.
- Flash new firmware, which hard-resets the board at the end.

## Driving it with verify.sh

Preconditions:

- Hardware tier. `$V doctor <PORT>` is healthy.
- `$V flash <PORT>` reports `flash exit=0`.

- **Reset and record.** Run `$V capture <PORT> 15 --reset`. The output reports `capture lines=N` with N > 0.
- **Boot log.** The log contains `I (...) WATERTANK_APP: Initializing...`.
- **P2P replies.** Within about 3 s after `Initializing...`, the log contains `[RAK3172] ` lines. On RUI3 firmware the expected replies are `OK` for each command. If `AT+NWM=0` changes mode, the module first restarts, so expect its banner. If no `[RAK3172] ` lines appear, report `boot-p2p` failed with "RAK3172 silent".
- **Loop start.** The log has a `Level of Water:` or `Error Measuring Water Level` line within about 4 s of `Initializing...`.
- **Proof.** Quote the `Initializing...` line, every `[RAK3172] ` line, and the first loop line with timestamps from `serial-*.log`.

## Gotchas

- The `[RAK3172] ` prefix and the module's reply are written separately, so a reply can start on the same line as the prefix or on the next line. Match the prefix, then the next non-empty text.
- The exact RAK3172 reply text depends on its firmware version. Record what you saw, and update this file with the observed strings after the first real run.
- The `--reset` pulse needs a board with the usual RTS→EN auto-reset circuit. Without it, press EN by hand right after starting the capture.
- `ESP_LOG` lines and RAK replies share UART0 and can interleave mid-line.
