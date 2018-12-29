Project Description

### Smart Home DIY ###
This project is a DIY home automation, you can have multuple arduino stations controlling anything at you home from your phone (webpage currently)
The Arduino stations should be flashed with the attached custom bootloader to enable remote upload of sketches from your development computer via RF24 (instead of tearing the wall apart each time)

This Project is managed in [trello](https://trello.com/b/8ODwVl67/diy-smart-home)


-------- TBD DOCS LAYOUT EXAMPLES STATIONS---------

For mote info read here:

https://www.2bitornot2bit.com/blog/smart-your-home-up-with-raspberry-pi-nodejs-bluetooth-serial-and-rf24

### Code layout ###

### Flashing remote station from VSCODE script (task) TBD ###

* My [Youtube video](https://youtu.be/8xJqVeZkEw8) to get more info.
* Also read [here](https://www.2bitornot2bit.com/blog/arduino-bootloader-with-ota-over-the-air-support-over-nrf24l01) to get a better understanding 



### How do I get set up? ###

* -------- TBD ---------
* -------- TBD ---------
* -------- TBD ---------
* -------- TBD ---------
* -------- TBD ---------
* -------- TBD ---------



### IR UNIVERSIAL STATION ###
![SmartHomeDIY/SmartHome/Other/Models_Images/IR_STATION/IR_STATION_MODEL1.PNG](/SmartHome/Other/Models_Images/IR_STATION/IR_STATION_MODEL1.PNG)

### PI SERVER WITH NODE AND SERIAL COMMANDS SENT USING BT TO BT TO ANT DISPATCH STATION ###
![PI_STATION_MODEL1.PNG](https://bitbucket.org/repo/86Rb8B/images/3356847782-PI_STATION_MODEL1.PNG)

### BT TO ANT DISPATCH SYSTEM ###
![BT_To_Ant_Dispatch_Station_model_bb.jpg](https://bitbucket.org/repo/86Rb8B/images/3146339184-BT_To_Ant_Dispatch_Station_model_bb.jpg)

### Bootloader modifications for OTA
https://youtu.be/8xJqVeZkEw8


Planned features list:

* RF24
* IR
* Links:
* Control and configure Smart House using nodejs website from mobile.
* Node Running on Raspberry Pi using serial to send commands to other andruino using the Bluetooth HC05 (serial to Bluetooth chip).
* Auto light using light sensors.
* Water system (time and light configuration)
* Shutters and shades conrol.
* House Light control
* Clap to turn off/on lights.
* Security camera and streaming.
* Music remote control.
* Hot water auto and timer control. (turn boiler for x hours - or everyday from hour to hour)
* LCD Screen on main arduino to monitor and status.
* In paraller to manual swithces.
* IR Control for Aircondition - temp sensors. weather forcast and outside temperator and humidity - start aircondition for x hours and auto close - with temp variation plans.
* Remote update the other arduinos from ANT.
* Send notifications when dish washer is done (no power consumption)
