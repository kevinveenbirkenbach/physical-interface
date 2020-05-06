#!/bin/bash
#
# This script updates the dependencies
# @author Kevin Veen-Birkenbach [kevin@veen.world]

# set variables
ARDUINO_LIBRARIES_PATH="$HOME/Arduino/libraries";

# test preconditions
if [ ! -d "$ARDUINO_LIBRARIES_PATH" ]
then
  echo "This program expects to find the Arduino libraries in $ARDUINO_LIBRARIES_PATH, but the directory doesn't exist."
  echo "Feel free to optimize this script for your individual purpose."
  exit 1;
fi

# Pulls or clones a repository
# @param $1 Folder name
# @param $2 Git repository
pullOrClone(){
  local FOLDER_PATH="$ARDUINO_LIBRARIES_PATH/$1";
  echo "Trying to update $FOLDER_PATH..."
  if [ -d "$FOLDER_PATH" ]
    then
      cd "$FOLDER_PATH" || exit 1;
        if [ -d ".git" ]
          then
            git pull;
          else
            echo "Update not possible, because library hadn't been installed via git."
        fi
    else
      cd "$ARDUINO_LIBRARIES_PATH" && git clone "$2";
  fi
}
(
  pullOrClone "Adafruit_Sensor" "https://github.com/adafruit/Adafruit_Sensor";
  pullOrClone "DHT-sensor-library" "https://github.com/adafruit/DHT-sensor-library";
  pullOrClone "NewRemoteSwitch" "https://github.com/1technophile/NewRemoteSwitch";
)
