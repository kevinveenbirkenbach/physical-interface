#!/bin/bash
DEVICE="/dev/ttyUSB0"
BAUT_RATE="9600"
echo "Setting up output device \"$DEVICE\" with baut rate $BAUT_RATE." &&
stty -F "$DEVICE" cs8 "$BAUT_RATE" ignbrk -brkint -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts &&
echo "Start interactive session with screen. You can close this session via \"screen -X -S arduino quit\"." &&
gnome-terminal -- /bin/bash -c 'screen -S "arduino" "'$DEVICE'" "'$BAUT_RATE'";' &&
echo "To terminate screen type enter..." &&
read &&
screen -X -S arduino quit && exit 1
