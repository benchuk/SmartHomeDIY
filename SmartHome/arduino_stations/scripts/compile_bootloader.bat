
cd ..
cd OTA\bootloader_with_ota_support\optiboot-nrf24l01-master\avr\bootloaders\optiboot-nrf24l01

SET MAKE_APP=%1\make\bin\make.exe
REM setx path "%path%;%MAKE_HOME_DIR%"

%MAKE_APP% atmega328 LED_START_FLASHES=0 RADIO_UART=1 FORCE_WATCHDOG=1 SUPPORT_EEPROM=1
