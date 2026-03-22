#!/bin/bash

set -e

# Contiki path relative to repo root
CONTIKI_PATH="../contiki-ng"

if [ ! -d "$CONTIKI_PATH" ]; then
  echo "Error: contiki-ng directory not found at $CONTIKI_PATH"
  echo "Make sure it exists in the root of the repository"
  exit 1
fi

echo "Installing IDS and firmware files into: $CONTIKI_PATH"
echo "------------------------------------------------------"

# Function to copy files safely
copy_file() {
  SRC="$1"
  DST="$2"

  if [ ! -f "$SRC" ]; then
    echo "Error: source file not found: $SRC"
    exit 1
  fi

  mkdir -p "$(dirname "$DST")"
  cp "$SRC" "$DST"

  echo "✔ Copied: $SRC -> $DST"
}

# IDS module
copy_file "../firmware/ids/ids_dis.c" \
          "$CONTIKI_PATH/os/services/ids/ids_dis.c"

copy_file "../firmware/ids/ids_dis.h" \
          "$CONTIKI_PATH/os/services/ids/ids_dis.h"

# RPL modification
copy_file "../firmware/rpl-icmp6.c" \
          "$CONTIKI_PATH/os/net/routing/rpl-lite/rpl-icmp6.c"

# Firmware Makefile
copy_file "../firmware/Makefile" \
          "$CONTIKI_PATH/examples/rpl-udp/Makefile"

# DIS attack firmware
copy_file "../firmware/dis-attack/Makefile" \
          "$CONTIKI_PATH/examples/dis-attack/Makefile"

copy_file "../firmware/dis-attack/dis-attacker.c" \
          "$CONTIKI_PATH/examples/dis-attack/dis-attacker.c"

echo "------------------------------------------------------"
echo "Installation completed successfully."