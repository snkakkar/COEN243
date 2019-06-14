# --- Kevin Velcich ---
#
# CLI for compiling & uploading programs onto esp32 using the arduino CLI.
# 
# This is set up for macOS. For other OSs only the SERIAL variable should
# need to be changed.
# 
# Also this uses a custom platform.txt to compile into the project directory,
# it might work by adding BUILD flag to end of compile instruction. See this
# github issue for details: https://github.com/arduino/arduino-cli/issues/163

BAUD		= 115200
BOARD		= esp32:esp32:nodemcu-32s-max
CLI		= arduino-cli
DIR		= $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
FQBN		= --fqbn $(BOARD)
BUILD		= --build-path $(DIR)
PORT		= -p $(SERIAL)
SERIAL		= /dev/cu.SLAB_USBtoUART

compile:;	$(CLI) compile $(FQBN) $(DIR)

upload:;	$(CLI) upload $(PORT) $(FQBN) $(DIR)

monitor:;	screen $(SERIAL) $(BAUD)

clean:;		$(RM) *.bin *elf

