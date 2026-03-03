#!/usr/bin/env bash
# flash.sh
# Usage:
#   ./flash.sh [device] [addr]
# Examples:
#   ./flash.sh            -> uses /dev/ttyACM0 check and FLASH_ADDR from Makefile
#   ./flash.sh /dev/ttyACM0 0x08000000

set -euo pipefail

DEVICE="${1:-/dev/ttyACM0}"
FLASH_ADDR="${2:-0x08000000}"
BIN="./build/firmware.bin"

if [ ! -f "$BIN" ]; then
  echo "Binary not found: $BIN"
  echo "Run: make"
  exit 1
fi

# try to free the device (if in use)
if lsof "$DEVICE" >/dev/null 2>&1; then
  echo "Device $DEVICE is in use. Listing PIDs:"
  lsof "$DEVICE"
  echo
  echo "Attempting to kill processes using $DEVICE..."
  PIDS=$(lsof -t "$DEVICE" || true)
  if [ -n "$PIDS" ]; then
    echo "Killing: $PIDS"
    kill $PIDS || echo "Failed to kill some processes. You may need sudo."
    sleep 0.5
  fi
fi

echo "Flashing $BIN to $FLASH_ADDR using st-flash..."
st-flash write "$BIN" "$FLASH_ADDR"
echo "Done."