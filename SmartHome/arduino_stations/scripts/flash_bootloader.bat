@echo off   
echo -----------------------------------                                                                                                                                                                                                                                                          
echo flash bootloader with ota support
echo -----------------------------------

for i; do 
    echo $i 
done 

CONF=%1\avr\windows\avrdude\etc\avrdude.conf
AVB_BIN=%1\avr\windows\avrdude\bin\
ECHO %AVB_BIN%
ECHO %CONF%

cd ..
cd OTA\bootloader_with_ota_support\optiboot-nrf24l01-master\avr\bootloaders\optiboot-nrf24l01\
cp optiboot_atmega328.hex %AVB_BIN%

cd %AVB_BIN%

avrdude -C %CONF% -b 19200 -c usbtiny -p m328p -v -e -U efuse:w:0xFD:m -U hfuse:w:0xDA:m -U lfuse:w:0xFF:m -F
avrdude -C %CONF% -b 19200 -c usbtiny -p m328p -v -e -U flash:w:optiboot_atmega328.hex -U lock:w:0x0F:m

echo "GAME OVER!"