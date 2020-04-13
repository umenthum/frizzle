#!/bin/bash

# Settings
DEFAULT_TARGET=$PWD/mem.hex
OBJCOPY=$RISCV/bin/riscv64-unknown-elf-objcopy
OBJDUMP=$RISCV/bin/riscv64-unknown-elf-objdump

# Command line parameters
ELF_FILE=$1
TARGET_FILE=${2:-$DEFAULT_TARGET}


# Print usage
if [[ "$#" -lt 1 ]]; then
    echo "Compile a RISC-V assembly file and write a memory file for simulation."
    echo "Usage: $0 <asm-file> [memory-file]"
    exit 0
fi

# Remove temporary directory on exit
cleanup()
{
    rm -rf -- "$WORK_DIR"
    # echo "$WORK_DIR"
}
trap cleanup exit

# Create temporary directory
WORK_DIR="$(mktemp -d)"

# Copy files to temporary directory
cp "$ELF_FILE" "$WORK_DIR"

# Fail if objcopy cannot be found
if [ ! -x "$OBJCOPY" ]; then
    echo "Cannot execute objcopy at $OBJCOPY" >&2
    echo "Make sure it exists and is executable." >&2
    exit 1
fi

# Fail if the target directory doesn't exist
if [[ ! -d "$(dirname "$TARGET_FILE")" ]]; then
    echo "Directory $(dirname "$TARGET_FILE") does not exist." >&2
    echo "Did you specify the correct target path? Aborting." >&2
    exit 4
fi

# Ask if user wants to overwrite target
if [ -e "$TARGET_FILE" ]; then
    echo "Target file $TARGET_FILE exists."
    read -p "Overwrite? [y/N] " CONF
    shopt -s nocasematch
    if [[ "${CONF}" != "y" && "${CONF}" != "yes" ]]; then
        echo "Aborting." >&2
        exit 0
    fi
    shopt -u nocasematch
fi

#"$OBJDUMP" -D "$ELF_FILE"
"$OBJCOPY" -O binary "$ELF_FILE" "$TARGET_FILE"
#hexdump $TARGET_FILE

# Fail if binary file doesn't exist or has no memory content
if [[ ! -e "$TARGET_FILE" || "$(cat "$TARGET_FILE" | wc -c)" -le "1" ]]; then
    echo "Error binarizing $ELF_FILE, not generating memory file" >&2
    exit 3
fi

echo "Assembled $ELF_FILE and wrote memory contents to $TARGET_FILE"
