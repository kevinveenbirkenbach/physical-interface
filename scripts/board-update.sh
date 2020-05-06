#!/bin/bash
#
# This script updates or creates the support for the ESP8266 board
# @author Kevin Veen-Birkenbach [kevin@veen.world]
# @see https://arduino-esp8266.readthedocs.io/en/latest/installing.html#using-git-version

# set variables
ARDUINO_HARDWARE_PATH="$HOME/Arduino/hardware";
ESP8266_PATH="$ARDUINO_HARDWARE_PATH/esp8266com";
if [ -d "$ESP8266_PATH" ]
  then
        echo "Updates esp8266com..."
        cd "$ESP8266_PATH/esp8266" || exit 1
        git status
        git pull
  else
    echo "Creates support for ESP8266..."
    mkdir -p "$ESP8266_PATH" && echo "Arduino hardware folder created" && cd "$ESP8266_PATH" || exit 2;
    git clone https://github.com/esp8266/Arduino.git esp8266
    echo "Initialize the submodules..."
    cd esp8266 || exit 3
    git submodule update --init
    cd tools || exit 4
    python3 get.py
    echo "esp8266com path looks now the follow:"
    tree -dL 2 "$ESP8266_PATH"
fi
echo "Finished."
