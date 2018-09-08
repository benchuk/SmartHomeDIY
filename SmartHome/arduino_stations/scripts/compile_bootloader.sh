
#!/bin/bash
set echo off   
echo -----------------------------------                                                                                                                                                                                                                                                          
echo compile bootloader
echo -----------------------------------

cd ..
cd OTA/bootloader_with_ota_support/optiboot-nrf24l01-master/avr/bootloaders/optiboot-nrf24l01/

make atmega328 LED_START_FLASHES=0 RADIO_UART=1 SUPPORT_EEPROM=1
