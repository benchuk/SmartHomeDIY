#!/bin/bash 
# osascript -e 'tell app "System Events" to display dialog "Make sure flasher station is connected, then press OK"'
for i; do 
    echo $i 
done 

avrdude -C "C:\Users\Ben\Downloads\avrdude\etc\avrdude.conf" -v -patmega328p -c arduino -PCOM5 -b115200 -D -U flash:w:text.ino.hex


echo "GAME OVER!"