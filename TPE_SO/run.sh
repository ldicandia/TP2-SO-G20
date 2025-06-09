#!/bin/bash

sudo make clean

MAKE_COMMAND="sudo make our" 
DEBUG_ARG_PRESENT=false


if [ "$1" = "buddy" ]; then
  MAKE_COMMAND="sudo make buddy"
  if [ "$2" = "-d" ]; then
    DEBUG_ARG_PRESENT=true
  fi
elif [ "$1" = "our" ]; then 
  if [ "$2" = "-d" ]; then
    DEBUG_ARG_PRESENT=true
  fi
elif [ "$1" = "-d" ]; then 
    DEBUG_ARG_PRESENT=true
fi

$MAKE_COMMAND

if [ "$DEBUG_ARG_PRESENT" = true ]; then
  sudo qemu-system-x86_64 -s -S -hda Image/x64BareBonesImage.qcow2 -m 512
else
  sudo qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512
fi