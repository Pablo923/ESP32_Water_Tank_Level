#!/usr/bin/env bash
# Verification harness for the ESP32 Water Tank Level firmware.
# Usage:
#   verify.sh doctor  [PORT]                       read-only health check
#   verify.sh build                                build firmware, log to artifacts
#   verify.sh flash   PORT                         flash the current build
#   verify.sh capture PORT SECONDS [--reset] [--send TEXT]
#                                                  record serial output to artifacts
#   verify.sh ports                                list candidate serial ports
#   verify.sh artifacts                            print this run's artifact dir
# Env:
#   RUN_ID     names the artifact dir (default: UTC timestamp, fixed per shell via export)
#   IDF_PATH   ESP-IDF checkout (default: $HOME/esp/esp-idf)
set -euo pipefail

harness_dir="$(cd "$(dirname "$0")" && pwd)"
repo="$(git -C "$harness_dir" rev-parse --show-toplevel)"
IDF_PATH="${IDF_PATH:-$HOME/esp/esp-idf}"
RUN_ID="${RUN_ID:-$(date -u +%Y%m%dT%H%M%SZ)}"
art="$repo/.verify-artifacts/$RUN_ID"
bin="$repo/build/ESP32_Water_Tank_Level.bin"
expected_idf="v5.1.1"

die() { printf 'error: %s\n' "$*" >&2; exit 1; }
idf_env() { . "$IDF_PATH/export.sh" >/dev/null 2>&1 || die "cannot source $IDF_PATH/export.sh"; }

port_busy() {
	# lsof exits 1 when nobody holds the port; that is the free case, not an error.
	{ lsof "$1" 2>/dev/null || true; } | awk 'NR>1{print $1" pid="$2}'
}

cmd_doctor() {
	local port="${1:-}" ok=1
	[ -f "$IDF_PATH/export.sh" ] && echo "ok   IDF_PATH=$IDF_PATH" || { echo "FAIL no export.sh in $IDF_PATH"; ok=0; }
	local v; v="$(cat "$IDF_PATH/version.txt" 2>/dev/null || git -C "$IDF_PATH" describe --tags 2>/dev/null || echo unknown)"
	[ "$v" = "$expected_idf" ] && echo "ok   ESP-IDF $v" || { echo "WARN ESP-IDF $v (project expects $expected_idf)"; }
	echo "info HEAD $(git -C "$repo" rev-parse --short HEAD) on $(git -C "$repo" branch --show-current)$(git -C "$repo" diff --quiet HEAD -- main components CMakeLists.txt || echo ' (firmware sources dirty)')"
	if [ -f "$bin" ]; then
		local newer
		newer="$(find "$repo/main" "$repo/components" "$repo/CMakeLists.txt" -newer "$bin" \( -name '*.c' -o -name '*.h' -o -name 'CMakeLists.txt' \) 2>/dev/null | head -3)"
		[ -z "$newer" ] && echo "ok   build is current ($(stat -f %Sm "$bin" 2>/dev/null || stat -c %y "$bin"))" \
			|| { echo "FAIL build is stale; newer sources:"; echo "$newer" | sed 's/^/       /'; ok=0; }
	else
		echo "FAIL no firmware image at build/ESP32_Water_Tank_Level.bin (run: verify.sh build)"; ok=0
	fi
	if [ -n "$port" ]; then
		[ -e "$port" ] || { echo "FAIL port $port does not exist (try: verify.sh ports)"; ok=0; }
		if [ -e "$port" ]; then
			local holders; holders="$(port_busy "$port")"
			[ -z "$holders" ] && echo "ok   port $port is free" || { echo "FAIL port $port held by: $holders (not ours: do not kill)"; ok=0; }
		fi
	fi
	[ "$ok" = 1 ] && echo "doctor: healthy" || { echo "doctor: NOT healthy"; return 1; }
}

cmd_build() {
	mkdir -p "$art"
	local log="$art/build.log" rc=0
	( cd "$repo" && make build IDF_PATH="$IDF_PATH" ) >"$log" 2>&1 || rc=$?
	echo "exit=$rc" >>"$log"
	grep -E 'warning:|error:' "$log" | grep -E '/(main|components)/' >"$art/build.diagnostics.txt" || true
	grep -E 'ESP32_Water_Tank_Level.bin binary size' "$log" | tail -1 || true
	echo "build exit=$rc log=$log project-diagnostics=$(wc -l <"$art/build.diagnostics.txt" | tr -d ' ')"
	return "$rc"
}

cmd_flash() {
	local port="${1:?usage: verify.sh flash PORT}"
	[ -z "$(port_busy "$port")" ] || die "port $port is held by another process: $(port_busy "$port")"
	mkdir -p "$art"
	local log="$art/flash.log" rc=0
	( cd "$repo" && make flash PORT="$port" IDF_PATH="$IDF_PATH" ) >"$log" 2>&1 || rc=$?
	echo "exit=$rc" >>"$log"
	grep -E 'Hash of data verified|Hard resetting' "$log" || true
	echo "flash exit=$rc log=$log"
	return "$rc"
}

cmd_capture() {
	local port="${1:?usage: verify.sh capture PORT SECONDS [--reset] [--send TEXT]}" secs="${2:?seconds required}"
	shift 2
	[ -z "$(port_busy "$port")" ] || die "port $port is held by another process: $(port_busy "$port")"
	mkdir -p "$art"
	local out="$art/serial-$(date -u +%H%M%S).log"
	idf_env
	python "$harness_dir/serial_capture.py" --port "$port" --seconds "$secs" --out "$out" "$@"
}

cmd_ports() {
	ls /dev/cu.usbserial* /dev/cu.usbmodem* /dev/cu.SLAB* /dev/cu.wchusbserial* /dev/ttyUSB* /dev/ttyACM* 2>/dev/null || echo "no USB serial ports found (board not connected?)"
}

case "${1:-}" in
	doctor) shift; cmd_doctor "$@" ;;
	build) shift; cmd_build ;;
	flash) shift; cmd_flash "$@" ;;
	capture) shift; cmd_capture "$@" ;;
	ports) cmd_ports ;;
	artifacts) echo "$art" ;;
	*) sed -n '2,/^set -euo/p' "$0" | sed '$d; s/^# \{0,1\}//'; exit 2 ;;
esac
