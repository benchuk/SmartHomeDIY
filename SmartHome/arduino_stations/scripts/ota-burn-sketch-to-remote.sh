#!/bin/bash 
set echo off  

# osascript -e 'tell app "System Events" to display dialog "Make sure flasher station is connected, then press OK"'
for i; do 
    echo $i 
done 

HEX=$1/../.pioenvs/nanoatmega328/firmware.hex
CONF=$2/avr/windows/avrdude/etc/avrdude.conf
AVB_BIN="$2/avr/windows/avrdude/bin/"

# ECHO $HEX
# ECHO $CONF
# ECHO $AVB_BIN

# cd $AVB_BIN
# copy $HEX .
# avrdude -C $CONF -v -patmega328p -c arduino -PCOM5 -b115200 -D -U flash:w:firmware.hex

cd $1
cd ..
platformio run --target upload

echo "GAME OVER!"