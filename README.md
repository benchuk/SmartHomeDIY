
# Welcome to SmartHomeDIY

This project is a DIY home automation framework.
With this system you will be able to control your home lights, Air conditioning, TV, water heater any many more devices (IR, wall sockets, and light switches are only a few).
The system is Arduino and Raspberry Pi based.
You will be able to control your house from the internal WiFi network as well as from the Internet.</br>
For internet access you will have to use a DNS service such as [no-ip](https://www.noip.com/free?gclid=Cj0KCQiAj4biBRC-ARIsAA4WaFiQeMklZ1TYbty1hthazzI8TCtS24gkB4rJyCSRdwulj-RzcK-DRw8aAsyBEALw_wcB&utm_campaign=free-dynamic-dns&utm_medium=cpc&utm_source=adwords&utm_term=free-dynamic-dns)

You can have multiple Arduino stations controlling anything at you home.</br>
Control is done via your phone, iPad or Desktop(currently using a web page only).

# SmartHomeDIY, Control everything about your automated home

Please see our full documentation [here](https://benchuk.github.io/SmartHomeDIY_Docs/)

This Project is managed in [trello](https://trello.com/b/8ODwVl67/diy-smart-home)

For some more info please checkout my [blog](https://www.2bitornot2bit.com/blog/smart-your-home-up-with-raspberry-pi-nodejs-bluetooth-serial-and-rf24)

## Flashing remote station from 'Visual Studio Code' script (task) TBD

* Connect Flasher to computer
* Ctrl+P and select Set remote Address this will set remote address to the Flasher.
* Upload sketch (this will forward the sketch to the remote station with the specified address)
* My [Youtube video](https://youtu.be/8xJqVeZkEw8) to get more info.
* Also read [here](https://www.2bitornot2bit.com/blog/arduino-bootloader-with-ota-over-the-air-support-over-nrf24l01) to get a better understanding


### Bootloader modifications for OTA

<a href="http://www.youtube.com/watch?feature=player_embedded&v=8xJqVeZkEw8
" target="_blank"><img src="http://img.youtube.com/vi/8xJqVeZkEw8/0.jpg" 
alt="Arduino OTA" width="340" height="280" border="20" /></a>



## Planned features list:

- [x]  IR - <a href="http://www.ibroadlink.com/" target="_blank">Broadlink</a> or  <a href="https://www.logitech.com/en-us/product/harmony-hub" target="_blank">Harmony hub</a> like - records IR commands and send them on request.
- [x]  Control and configure Smart House using nodejs website from mobile.
- [x]  Node Running on Raspberry Pi using serial to send commands to other Arduino using the Bluetooth HC05 (serial to Bluetooth chip).
- [ ]  Auto light using light sensors.
- [ ]  Plants Water system (time and light configuration)
- [x]  Shutters control.
- [x]  Shades control.
- [x]  House Light control
- [ ]  Clap to turn off/on lights.
- [ ]  Security camera and streaming.
- [ ]  Music remote control.
- [ ]  Hot water auto and timer control. (turn boiler for x hours - or everyday from hour to hour)
- [ ]  LCD Screen on main arduino to monitor and status.
- [x]  On wall Touch switch in parallel to digital control.
- [ ]  IR Control for Air Condition - temp sensors. weather forecast and outside temperature and humidity - start Air Condition for x hours and auto close - with temp variation plans.
- [x]  Remote update the other Arduino station from RF24.
- [ ]  Send notifications when dish washer is done (no power consumption)
- [ ]  Central logging station
- [ ]  Easy updates to station (download firmware from github and flash to specific address)
- [ ]  Dynamic remote controls recording and UI setup
- [ ]  Configuration server
