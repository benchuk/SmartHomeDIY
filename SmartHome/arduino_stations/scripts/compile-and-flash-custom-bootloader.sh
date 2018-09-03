

                                                                                                                                                                                                                                                                   
echo Burn bootloader                                                                                                                                                                                                                                                             


osascript -e 'tell app "System Events" to display dialog "Make sure the SPI burnner is connected via CISP pin and ready"'
                                                                                                                       
# cd C:\Users\Ben\Downloads\avrdude\bin>avrdude
# copy the optiboot_atmega328.hex from C:\Users\Ben\Documents\arduinodiy\Arduino Porjects\Projects\SmartHome\OTA or download from https://github.com/nicolas-schurando/optiboot-nrf24l01/tree/master/avr/bootloaders/optiboot-nrf24l01
# run the following 2 commands:
# !!!note the DA:
# !!!!!!!!!!!!!!!!!!  cd C:\Users\Ben\Downloads\avrdude\bin
# !!!!!!!!!!!!!!!!!!	avrdude -C "C:\Users\Ben\Downloads\avrdude\etc\avrdude.conf" -b 19200 -c usbtiny -p m328p -v -e -U efuse:w:0xFD:m -U hfuse:w:0xDA:m -U lfuse:w:0xFF:m -F
# !!!!!!!!!!!!!!!!!!  avrdude -C "C:\Users\Ben\Downloads\avrdude\etc\avrdude.conf" -b 19200 -c usbtiny -p m328p -v -e -U flash:w:optiboot_atmega328_without_watchdog.hex -U lock:w:0x0F:m
# avrdude -C "C:\Users\Ben\Downloads\avrdude\etc\avrdude.conf" -b 19200 -c usbtiny -p m328p -v -e -U flash:w:optiboot_atmega328_with_watchdog_auto_reset_every_4_sec.hex -U lock:w:0x0F:m
