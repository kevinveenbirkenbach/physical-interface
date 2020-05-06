#!/bin/bash
# @author Kevin Veen-Birkenbach [kevin@veen.world]
# This script compiles the program

# set variables
ARDUINO_PATH="/bin/arduino";

# test preconditions
if [ ! -f "$ARDUINO_PATH" ]
then
  echo "This program expects to find the Arduino IDE in $ARDUINO_PATH, but the file doesn't exist."
  echo "Feel free to optimize this script or install Arduino how described under the following url:"
  echo "https://askubuntu.com/questions/1025753/how-to-install-latest-arduino-ide"
  exit 1;
fi
bash "$ARDUINO_PATH" --upload ../main/main.ino
