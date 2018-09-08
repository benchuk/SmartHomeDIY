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
cd OTA\bootloader_with_ota_support\optiboot-nrf24l01-master\avr\bootloaders\optiboot-nrf24l01\
copy %HEX_FILE% %AVB_BIN%

cd %AVB_BIN%

echo -----------------------------------                                                                                                                                                                                                                                                          
echo flashing  %HEX_FILE%
echo -----------------------------------

avrdude -C %CONF% -b 19200 -c usbtiny -p m328p -v -e -U efuse:w:0xFD:m -U hfuse:w:0xDA:m -U lfuse:w:0xFF:m -F
avrdude -C %CONF% -b 19200 -c usbtiny -p m328p -v -e -U flash:w:%HEX_FILE% -U lock:w:0x0F:m

echo "GAME OVER!"