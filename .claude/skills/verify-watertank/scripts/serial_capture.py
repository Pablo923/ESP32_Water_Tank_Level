"""Record ESP32 serial output for a fixed time, without a TTY.

idf.py monitor needs an interactive terminal; this does not. It writes every
line with a host-side elapsed-seconds prefix so cadence can be checked.

  --reset  pulse EN through RTS (DTR held low so IO0 stays high) to capture boot.
  --send   write TEXT once, 1 s after opening, to exercise the UART0 echo task.
"""
import argparse
import sys
import time

import serial


def set_line(port, name, value):
    try:
        setattr(port, name, value)
    except (OSError, serial.SerialException):
        pass  # ptys and some adapters have no modem lines


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--port", required=True)
    ap.add_argument("--seconds", type=float, required=True)
    ap.add_argument("--out", required=True)
    ap.add_argument("--baud", type=int, default=115200)
    ap.add_argument("--reset", action="store_true")
    ap.add_argument("--send")
    a = ap.parse_args()

    s = serial.Serial()
    s.port, s.baudrate, s.timeout = a.port, a.baud, 0.1
    set_line(s, "dtr", False)
    set_line(s, "rts", False)
    try:
        s.open()
    except serial.SerialException as e:
        sys.exit(f"error: cannot open {a.port}: {e}")

    if a.reset:
        set_line(s, "rts", True)
        time.sleep(0.1)
        set_line(s, "rts", False)

    start = time.monotonic()
    sent = a.send is None
    buf = b""
    lines = 0
    with open(a.out, "w", encoding="utf-8") as f:
        f.write(f"# port={a.port} baud={a.baud} reset={a.reset} send={a.send!r}\n")
        while time.monotonic() - start < a.seconds:
            if not sent and time.monotonic() - start >= 1.0:
                s.write(a.send.encode())
                f.write(f"[{time.monotonic() - start:7.2f}] >>> SENT {a.send!r}\n")
                sent = True
            buf += s.read(512)
            *done, buf = buf.split(b"\n")
            for raw in done:
                f.write(f"[{time.monotonic() - start:7.2f}] {raw.decode('utf-8', 'replace').rstrip()}\n")
                lines += 1
        if buf:
            f.write(f"[{time.monotonic() - start:7.2f}] {buf.decode('utf-8', 'replace').rstrip()} (partial)\n")
            lines += 1
    s.close()
    print(f"capture lines={lines} out={a.out}")
    if lines == 0:
        sys.exit("error: no serial output captured (wrong port, board not running, or baud mismatch)")


if __name__ == "__main__":
    main()
