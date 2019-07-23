#include <Arduino.h>
#include <PinChangeInt.h>
#include <common.h>

#include <Mirf.h>
#include <MirfHardwareSpiDriver.h>
#include <SPI.h>
#include <nRF24L01.h>

Payload p;
uint8_t isTVOn = 1;

void signalState() {
    Serial.println("signalState");
    p.address = 104;
    p.type = TV_STATION;
    p.data = isTVOn;
    Serial.println("bin state");
    Serial.println(p.data, BIN);
    // wait for prev send to finish
    while (Mirf.isSending())
        ;
    Mirf.send((uint8_t*)(&p));
    while (Mirf.isSending())
        ;
    delay(150);
    Mirf.send((uint8_t*)(&p));
    while (Mirf.isSending())
        ;
    Serial.println("Done");
}

void setup() {

    configureEEPROMAddressForRFAndOTA("104");

    Serial.begin(9600);

    Serial.println("Init RF");

    startRF();

    Serial.println("INIT OK TV Station");
}

void loop() {

   signalState();

 Serial.println("listen....");

  sleepMinutes(20);
}
