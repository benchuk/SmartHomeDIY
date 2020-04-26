

/*
  SD card read/write
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4

 Please note: Also on the bus is the RF24 module

  5V POWER Taken from Vin pin - workaround - because I have run out of 5v pins
  see trick here: http://www.open-electronics.org/the-power-of-arduino-this-unknown/
*/

/*********************
SD CARD
**********************/
#include <Arduino.h>
#include <common.h>

/*********************
RF24
**********************/
#include <Mirf.h>
#include <MirfHardwareSpiDriver.h>
#include <SPI.h>
#include <nRF24L01.h>
// See common.h for pins
//** RF MOSI - pin 11
//** RF MISO - pin 12
//** RF CLK - pin 13
//** RF CS - pin 10
//** RF Ce - pin 9

/****
 * PWM Light
 * **/
int pwm_led_pin = 3;

void enableRF();

void setup(void) {
    watchdogReset();
    configureEEPROMAddressForRFAndOTA("008");

    pinMode(Radio_CSN, OUTPUT);

    watchdogReset();
    Serial.begin(9600);

    //setup SPI devices
    SPI.begin();

    //SPI - rf24 radio
    Serial.println(F("RF INIT"));
    watchdogReset();
    enableRF();
    startRF();
    Serial.println(F("Done RF INIT ok ok"));
    watchdogReset();

    pinMode(pwm_led_pin, OUTPUT);
    analogWrite(pwm_led_pin, 0);  //off
}

/**********************
   Main Arduino Loop
**********************/
void loop(void) {
    enableRF();
    //Serial.println(F("Waiting for remote command..."));
    while (!Mirf.dataReady()) {
        delay(100);
        //blinkReady();
    };

    checkIfOtaRequestOrLoadCommand(cmd);
    //Serial.print(F("Got New command: "));
    //Serial.println(cmd);

    if (strcmp(cmd, "001") == 0) {
        Serial.println(F("Restarting..."));
        // soft_restart();
        return;
    } else if (strcmp(cmd, "999") == 0) {
        Serial.println(F("Hue1"));
        //PWM high
        analogWrite(pwm_led_pin, 255);
        return;
    } else if (strcmp(cmd, "998") == 0) {
        Serial.println(F("Hue2"));
        //PWM mid
        analogWrite(pwm_led_pin, 150);
        return;
    } else if (strcmp(cmd, "997") == 0) {
        Serial.println(F("Hue3"));
        //PWM low
        analogWrite(pwm_led_pin, 100);
        return;
    } else if (strcmp(cmd, "996") == 0) {
        Serial.println(F("Hue4"));
        //PWM very low
        analogWrite(pwm_led_pin, 50);
        return;
    } else if (strcmp(cmd, "995") == 0) {
        Serial.println(F("Hue5"));
        //PWM off
        analogWrite(pwm_led_pin, 0);
        return;
    }
}  //end of loop

void enableRF(void) {
    digitalWrite(Radio_CSN, LOW);  //Enable radio
    delay(10);
}

void disableRF(void) {
    digitalWrite(Radio_CSN, HIGH);  //Disable radio
    delay(10);
}
