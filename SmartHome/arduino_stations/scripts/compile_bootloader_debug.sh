
#!/bin/bash
set echo off   
echo -----------------------------------                                                                                                                                                                                                                                                          
echo compile bootloader debug
echo -----------------------------------

cd ..
cd OTA/bootloader_with_ota_support/for_debug/bootloader_with_debug_leds/optiboot-nrf24l01-master-debug/avr/bootloaders/optiboot-nrf24l01/

#make atmega328 LED_START_FLASHES=0 RADIO_UART=1 SUPPORT_EEPROM=1 
#make atmega328 LED_START_FLASHES=0 RADIO_UART=1 SUPPORT_EEPROM=1 FORCE_WATCHDOG=1
make atmega328 LED_START_FLASHES=10 RADIO_UART=1 SUPPORT_EEPROM=1
open .
