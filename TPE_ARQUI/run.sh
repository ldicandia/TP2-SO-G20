#!/bin/bash

#  sudo make clean

#  sudo make buddy

# if [ "$1" = "-d" ]; then
# sudo qemu-system-x86_64 -s -S -hda Image/x64BareBonesImage.qcow2 -m 512
# else
# sudo qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512
# fi


#!/bin/bash

sudo make clean

MAKE_COMMAND="sudo make our" # Default to 'make our'
DEBUG_ARG_PRESENT=false

# Determine the make command and identify the debug flag position
if [ "$1" = "buddy" ]; then
  MAKE_COMMAND="sudo make buddy"
  if [ "$2" = "-d" ]; then
    DEBUG_ARG_PRESENT=true
  fi
elif [ "$1" = "our" ]; then # Explicitly handle 'our' as first argument
  # MAKE_COMMAND is already "sudo make our"
  if [ "$2" = "-d" ]; then
    DEBUG_ARG_PRESENT=true
  fi
elif [ "$1" = "-d" ]; then # If '-d' is the first argument, 'make our' is implied
    DEBUG_ARG_PRESENT=true
fi

# Execute the determined make command
$MAKE_COMMAND

# Run QEMU
if [ "$DEBUG_ARG_PRESENT" = true ]; then
  sudo qemu-system-x86_64 -s -S -hda Image/x64BareBonesImage.qcow2 -m 512
else
  sudo qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512
fi