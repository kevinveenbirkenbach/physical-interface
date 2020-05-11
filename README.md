# Physical Interface

[![License: AGPL v3](https://img.shields.io/badge/License-AGPL%20v3-blue.svg)](https://www.gnu.org/licenses/agpl-3.0)

Software to build up an REST-API on an Arduino based microcontroller, to interact with hardware and the physical environment.

## Setup

First update the necessary software via typing in:

```bash
bash ./scripts/library-update.sh
bash ./scripts/board-update.sh
```

## Maintaining

### Links
The following links can be helpful for maintaining this code and understanding the domain:

#### Hardware
- https://smarthome-blogger.de/wp-content/uploads/2018/03/nodemcu-esp8266-pinout.png

##### Transistor
- https://www.elektronik-kompendium.de/sites/bau/0201291.htm
- https://components101.com/transistors/s8050-transistor-pinout-equivalent-datasheet
- https://cdn-reichelt.de/documents/datenblatt/A100/PN2222A-FSC.pdf

##### IR-Reciever
- https://www.elecrow.com/download/Infrared%20receiver%20vs1838b.pdf

#### Software

#### IR
- https://github.com/crankyoldgit/IRremoteESP8266
- https://github.com/z3t0/Arduino-IRremote/blob/master/examples/IRrecvDumpV2/IRrecvDumpV2.ino
- https://daniel-ziegler.com/arduino/esp/mikrocontroller/2017/07/28/arduino-universalfernbedienung/
- https://www.instructables.com/id/Universal-Remote-Using-ESP8266Wifi-Controlled/

#### Serial Communication
- https://playground.arduino.cc/Interfacing/LinuxTTY/

## License
The ["GNU AFFERO GENERAL PUBLIC LICENSE"](./LICENSE.txt) applies to this project.
