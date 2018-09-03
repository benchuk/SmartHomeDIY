
@ECHO on
ECHO %1
cd avr\windows\avrdude\bin\
avrdude.exe -C "..\etc\avrdude.conf" -v -patmega328p -c arduino -PCOM5 -b115200 -D -U flash:%1
ECHO "GAME OVER!"