#include <Arduino.h>
#include <PinChangeInt.h>
#include <common.h>

#include <Mirf.h>
#include <MirfHardwareSpiDriver.h>
#include <SPI.h>
#include <dht.h>
#include <nRF24L01.h>

#define dht_apin A0
dht DHT;


Payload p;

void signalState(Status_Type type, uint8_t data) {
    Serial.println("signalState");

    p.address = 101;
    p.type = type;
    p.data = data;
    //Serial.println(p.data, BIN);
    Mirf.send((byte*)&p);
    while (Mirf.isSending())
        ;
    Serial.println("Done");
}

void setup() {

    configureEEPROMAddressForRFAndOTA("101");

    Serial.begin(9600);

    Serial.println("Init RF");

    startRF();

    Serial.println("INIT OK Temp sensor station");
}

void loop() {

    
    Serial.println("listen....");
    //  delay(100);
    // return;
    // while (!Mirf.dataReady()) {
    //     watchdogReset();
    //     Serial.print(".");
    // };

    DHT.read11(dht_apin);

    Serial.print("Current humidity = ");
    Serial.print((uint8_t) DHT.humidity);
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print((uint8_t) DHT.temperature);
    Serial.println("C  ");

    signalState(TEMP_STATION, (uint8_t) DHT.temperature);
    delay(5000);
    signalState(HUMIDITY_STATION, (uint8_t) DHT.humidity);
    sleepMinutes(10);
}
