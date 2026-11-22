# LoRa transmit

After each valid level reading, the firmware sends `AT+PSEND=<hex>` to the RAK3172. The payload is 9 bytes, hex-encoded (18 hex digits): device ID `0x01`, the level as a little-endian `float32`, temperature as `int16` hundredths of °C, and humidity as `uint16` hundredths of %RH (both little-endian, from the DHT11 on GPIO27). A failed DHT11 read sends `0080` for temperature and `FFFF` for humidity. The firmware logs the hex as `LoRa payload: <hex>` before sending. The RAK3172 acknowledges the command and transmits the packet over LoRa P2P to the receiving node.

## Sub-features

- `tx-ack` has the RAK3172 reply to each `AT+PSEND`.
- `tx-done` has the RAK3172 report that the P2P transmission finished.
- `tx-payload` has the receiving node get the same 9 bytes as the logged `LoRa payload:` line.
- `tx-skip-on-error` sends nothing when the reading is out of range.

## How to get to it (user POV)

- Let the board run with the sensor reading a valid level. Packets go out about every 5 s.
- Watch the receiving LoRa node (same P2P parameters, 915 MHz, SF7, BW125) for incoming packets.

## Driving it with verify.sh

Preconditions:

- Hardware tier. The current build is flashed. The RAK3172 is connected and `boot-p2p` passed.
- The sensor gives valid readings (see `water-level.md`).

- **Record transmits.** Run `$V capture <PORT> 25`. Each `Level of Water:` line is followed by `[RAK3172] ` reply text.
- **Ack and done.** On RUI3 the expected replies are `OK`, then `+EVT:TXP2P DONE`. Record the exact strings you see.
- **Payload check.** Decode each `LoRa payload:` line with `struct.unpack('<BfhH', bytes.fromhex(h))` and check it matches the logged level, temperature and humidity (allow `%.2f` rounding on the level). On the receiver, the printed payload must equal the logged hex.
- **No send on error.** On the error path, no `[RAK3172] ` lines follow the `Error Measuring Water Level` line.
- **Proof.** Quote each level line with its RAK3172 replies. Attach the receiver output, or state `tx-payload: needs receiver`.

## Gotchas

- The ESP32 logs the payload hex but not the `AT+PSEND` command itself. The RAK3172 reply is the serial evidence that a transmit happened.
- A `DHT11 read failed` warning means the sensor did not answer or the checksum failed. The packet still goes out with the no-data markers.
- `RAK3172_P2P_CONFIG` must match the receiver exactly, or packets are silently lost while the transmitter still reports done.
