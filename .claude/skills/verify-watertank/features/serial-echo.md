# Serial echo

Text written to the board's USB serial port (UART0) is read by the `uartSerial_receiveTask` task and written back prefixed with `[SERIAL] `. It is the only host→device input path in the firmware.

## Sub-features

- `echo-roundtrip` echoes sent text back after the `[SERIAL] ` prefix.
- `echo-coexist` keeps the main loop's log lines arriving while echo runs.

## How to get to it (user POV)

- Type into a serial terminal (`make monitor PORT=...`) connected to the board.

## Driving it with verify.sh

Preconditions:

- Hardware tier. The current build is flashed. No other process holds the port.

- **Send and record.** Run `$V capture <PORT> 12 --send "ping-$RUN_ID"`. The log shows `>>> SENT 'ping-...'` at about 1 s.
- **Echo.** Within 1 s after the send, the log contains `[SERIAL] ping-<RUN_ID>`.
- **Coexistence.** Loop lines (`Level of Water:` or `Error Measuring Water Level`) keep their 5–6 s cadence before and after the send.
- **Proof.** Quote the SENT line and the echo line with timestamps.

## Gotchas

- Keep sent text well under 1024 bytes. The task reads up to `BUF_SIZE` bytes into a `BUF_SIZE` buffer, then writes a terminator at `data[len]`, so a 1024-byte read writes one byte past the buffer.
- The echo shares UART0 with `ESP_LOG`, so the echoed text can land mid-line with a log line.
- `idf.py monitor` also forwards keystrokes. Never run it alongside a capture on the same port.
