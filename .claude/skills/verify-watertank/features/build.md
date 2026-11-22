# Firmware build

The firmware builds for `esp32` with ESP-IDF 5.1.1 into `build/ESP32_Water_Tank_Level.bin`. The image must fit the smallest app partition, which is 0x100000 bytes.

## Sub-features

- `build-compile` compiles and links without errors.
- `build-size` reports the image size and the free space left in the app partition.
- `build-warnings` reports compiler warnings in `main/` and `components/` only.

## How to get to it (user POV)

- Run `make build` from the repo root.
- Press `Cmd+Shift+B` in VS Code, which runs the same make target.
- Run `idf.py build` after sourcing `export.sh`.

## Driving it with verify.sh

Preconditions:

- `$V doctor` reports `ok   IDF_PATH=...`.

- **Build.** Run `$V build`. The output ends with `build exit=0 ...` and a `binary size 0x... bytes. ... (NN%) free.` line.
- **Check diagnostics.** Read `.verify-artifacts/$RUN_ID/build.diagnostics.txt`. Compare its count with the base branch. A change must not add warnings in `main/` or `components/`.
- **Confirm current.** Run `$V doctor`. It shows `ok   build is current`.
- **Proof.** Keep `build.log` and `build.diagnostics.txt`, and quote the size line in the report.

## Gotchas

- The first build after a fresh clone or `make clean` takes about 2.5 min. A timeout under 5 min can kill it mid-link.
- `sdkconfig` is gitignored and generated on first build. If the target looks wrong, run `make set-target` once. That regenerates `sdkconfig` and forces a full rebuild.
- A green build proves nothing about runtime behavior. Never report hardware journeys as verified from it.
