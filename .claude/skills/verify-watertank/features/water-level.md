# Water level measurement

About every 5 s, the firmware measures distance with the HC-SR04 and converts it to liters (`level = distance_cm * -31.65 + 5893.98`). If the level is in `[0, 5400]`, it shows the whole liters on the TM1637 and logs the reading. Otherwise it logs an error and shows dashes on the display.

## Sub-features

- `level-valid` logs `Level of Water: <x.xx> lts` and shows the integer on the display.
- `level-error` logs `Error Measuring Water Level` and shows the dash pattern on the display.
- `level-cadence` keeps successive readings about 5–6 s apart.

## How to get to it (user POV)

- Watch the display or the serial log while the board runs.
- Change the water height, or move a flat target in front of the sensor, and watch the reading follow.

## Driving it with verify.sh

Preconditions:

- Hardware tier. The current build is flashed.
- For `level-valid`, the sensor is connected and the target is between about 16 cm and 186 cm away. That distance range maps to 0–5400 L.

- **Record steady state.** Run `$V capture <PORT> 25`. The log has at least 4 loop lines.
- **Valid reading.** With the sensor aimed at a target, each loop line reads `WATERTANK_APP: Level of Water: <x.xx> lts`, with the value in `[0, 5400]`.
- **Error path.** With the sensor disconnected or aimed at nothing, each loop line reads `WATERTANK_APP: Error Measuring Water Level`.
- **Cadence.** Consecutive loop lines are 5.0–6.2 s apart by the host timestamps.
- **Response to change.** Move the target closer during a second capture. The logged liters rise, because closer means fuller.
- **Display.** Needs human: the TM1637 shows the same integer as the log, or dashes on error.
- **Proof.** Quote 3 or more consecutive loop lines with timestamps, and say which path (valid or error) you exercised and how.

## Gotchas

- With no sensor attached, one of the echo waits times out after about 1 s. If ECHO floats low, the distance comes out near 0, which converts to about 5894 L, above 5400. If it floats high, the distance is about 17150 cm, which converts to a negative level. Either way the result is the error path, not a bogus valid reading. A valid reading with nothing attached would itself be a bug.
- The error log ends with `\n` inside `ESP_LOGI`, so expect a blank line after each error line.
- The calibration constants are specific to one tank. Do not "fix" a reading by changing them unless the task asks for it.
