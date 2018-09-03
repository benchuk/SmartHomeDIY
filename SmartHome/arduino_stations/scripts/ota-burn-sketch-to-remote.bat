@echo off

SET HEX=%1\..\.pioenvs\nanoatmega328\firmware.hex
SET CONF=%2\avr\windows\avrdude\etc\avrdude.conf

REM ECHO %HEX%
REM ECHO %CONF%
REM cd avr\windows\avrdude\bin\
REM copy %HEX% .
REM avrdude -C %CONF% -v -patmega328p -c arduino -PCOM5 -b115200 -D -U flash:w:firmware.hex

cd %1
cd ..
platformio run --target upload

ECHO "GAME OVER!"