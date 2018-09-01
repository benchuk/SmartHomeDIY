# optiboot-nrf24l01-flasher
Flasher program to be used with [optiboot for nRF24L01](https://github.com/balrog-kun/optiboot).

## Introduction
This is a fork of the flasher program provided by @balrog-kun, modified in order to make it compatible with the Arduino IDE.

## Download
You can either download directly one the prebuilt .hex files attached in the [releases page](https://github.com/nicolas-schurando/optiboot-nrf24l01-flasher/releases) and manually upload it to your arduino with avrdude.

Or, you can retrieve the sources from this repository on your computer, open the `optiboot-nrf24l01-flasher.ino` file, and building from the Arduino IDE.

## Required setup
```
+----------+
|          |         +---------+         +-------+
| Computer |  +--->  | Arduino |  +--->  | nRF24 |
|          |   USB   +---------+   SPI   +-------+
+----------+           Flasher
  Compiler
```

This program is to be deployed on an arduino that will serve as gateway between avrdude on your computer and the targetted wireless node.

It expects a nRF24L01 module to be connected to the computer-side arduino as described in the following table. More information on wiring can be found here https://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo, and pin information can be changed in the `optiboot-nrf24l01-flasher.ino` file.

Signal | RF Module PIN | Cable COLOR | Arduino pin
------ | ------------- | ----------- | -----------
GND | 1 | Brown | GND
VCC | 2 | Red | 3.3V
CE | 3 | Orange | 9
CSN | 4 | Yellow | 10
SCK | 5 | Green | 13
MOSI | 6 | Blue | 11
MISO | 7 | Violet | 12
IRQ | 8 | Gray | N/C
