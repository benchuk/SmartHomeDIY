@echo off
echo -----------------------------------                                                                                                                                                                                                                                                          
echo flash bootloader with ota support
echo -----------------------------------

SET HEX_FILE=%2
SET CONF=%1\avr\windows\avrdude\etc\avrdude.conf
SET AVB_BIN=%1\avr\windows\avrdude\bin\
ECHO %AVB_BIN%
ECHO %CONF%

cd ..
REM cd OTA\bootloader_with_ota_support\optiboot-nrf24l01-master\avr\bootloaders\optiboot-nrf24l01\

cd scripts\hex\

copy %HEX_FILE% %AVB_BIN%

cd %AVB_BIN%

echo -----------------------------------                                                                                                                                                                                                                                                        
echo flashing  %HEX_FILE%
echo -----------------------------------
dir
avrdude -C %CONF% -b 19200 -c usbtiny -p atmega32 -v -e -U efuse:w:0xFD:m -U hfuse:w:0xDA:m -U lfuse:w:0xFF:m -F
REM avrdude -C %CONF% -b 19200 -c usbtiny -p m328p  -e -Ulock:w:0x3F:m -Uefuse:w:0xF8:m -Uhfuse:w:0xdd:m -Ulfuse:w:0xff:m -F
REM avrdude -C %CONF% -b 19200 -c usbtiny -p m328p -e -v -u -U efuse:w:0xFD:m -U hfuse:w:0xD6:m -U lfuse:w:0xFF:m
REM avrdude -C %CONF% -b 19200 -c usbtiny -p m328p -e -v -U flash:w:%HEX_FILE% -U lock:w:0x0F:m -F -V
REM avrdude -C %CONF% -v -v -v -v -patmega8 -c usbtiny -p m328 -b19200 -B8 -e -Ulock:w:0x3F:m -Uhfuse:w:0xc4:m -Ulfuse:w:0xe4:m 
avrdude -F -C %CONF% -b 19200 -c usbtiny -p m328p -v -e -U flash:w:%HEX_FILE% -U lock:w:0x0F:m

echo "GAME OVER!"