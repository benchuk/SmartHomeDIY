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
  //  Serial.println("signalState");
    p.address = 104;
    p.type = TV_STATION;
    p.data = isTVOn;
   // Serial.println("bin state");
    //Serial.println(p.data, BIN);
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
    //Serial.println("Done");
}

long readVcc() 
{
    long result;
    // Read 1.1V reference against AVcc
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    delay(2); // Wait for Vref to settle
    ADCSRA |= _BV(ADSC); // Convert
    while (bit_is_set(ADCSRA,ADSC));
    result = ADCL;
    result |= ADCH<<8;
    result = 1126400L / result; // Back-calculate AVcc in mV
    return result;
}

void setup() {

    configureEEPROMAddressForRFAndOTA("104");

    Serial.begin(9600);

    Serial.println("Init RF");

    startRF();

    Serial.println("INIT OK TV Station");
     signalState();
}

void loop() {

    long vcc = readVcc();
    if(vcc < 3500)
    {
        isTVOn  = 0;
         signalState();
    }

    Serial.println(vcc);
  //  Serial.println("listen....");
    //sleepMinutes(20);
}
