@echo off
echo -----------------------------------                                                                                                                                                                                                                                                          
echo compile bootloader debug
echo -----------------------------------

SET AVR8_GNU_TOOLCHAIN_PATH=%1\avr\windows\avr8-gnu-toolchain\bin
REM should be installed http://gnuwin32.sourceforge.net/install.html or http://fab.cba.mit.edu/classes/863.16/doc/projects/ftsmin/windows_avr.html or http://fab.cba.mit.edu/classes/863.16/doc/projects/ftsmin/make-3.81.exe
SET GnuWin32_PATH=C:\Program Files (x86)\GnuWin32
SET AVR_DUDE_PATH=%1\avr\windows\avrdude\bin
SET PATH=%PATH%;%GnuWin32_PATH%;%AVR_DUDE_PATH%;%AVR8_GNU_TOOLCHAIN_PATH%

echo -----------------------------------
echo %AVR8_GNU_TOOLCHAIN_PATH%
echo -----------------------------------
echo %GnuWin32_PATH%
echo -----------------------------------
echo %AVR_DUDE_PATH%
echo -----------------------------------
echo %PATH%
echo -----------------------------------
echo ***********************************
cd ..
cd OTA\bootloader_with_ota_support\for_debug\bootloader_with_debug_leds\optiboot-nrf24l01-master-debug\avr\bootloaders\optiboot-nrf24l01\
REM ENV install here  TDB make auto: http://fab.cba.mit.edu/classes/863.16/doc/projects/ftsmin/windows_avr.html
REM http://www.microchip.com/mplab/avr-support/avr-and-arm-toolchains-(c-compilers)
REM https://github.com/eerimoq/avr-toolchain-windows
REM http://blog.zakkemble.net/download/avr-gcc-8.2.0-x64-mingw.zip - this is the version I used. from here http://blog.zakkemble.net/avr-gcc-builds/
REM SET MAKE_APP=%1\make\bin\make.exe
REM SET MAKE_APP=mingw32-gcc.exe
REM SET MAKE_HOME_DIR=C:\MinGW\bin\
REM setx path "%path%;%MAKE_HOME_DIR%"

make atmega328 LED_START_FLASHES=10 RADIO_UART=1 SUPPORT_EEPROM=1
REM make atmega328 RADIO_UART=1 SUPPORT_EEPROM=1
REM %MAKE_APP% atmega328 LED_START_FLASHES=0 RADIO_UART=1 SUPPORT_EEPROM=1 FORCE_WATCHDOG=1
start .

