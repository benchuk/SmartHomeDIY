#include <Arduino.h>
#include <Mirf.h>
#include <MirfHardwareSpiDriver.h>
#include <PinChangeInt.h>
#include <SPI.h>
#include <common.h>
#include <nRF24L01.h>

// switch state for lights
// RELAY (mechanical ones) are active low - set high so default is lights off
volatile byte s1 = HIGH;
volatile byte s2 = HIGH;
volatile byte s3 = HIGH;
volatile byte s4 = HIGH;

// char* myAddress = "999";
// uint8_t address = 9;

enum Status_Type { Relay_4_Way = 1,
                   Relay_2_Way = 2 };

typedef struct PayloadData {
    uint8_t address;
    uint8_t type;
    uint8_t data;
} Payload;

Payload p;

uint8_t counter = 0;
void signalState() {
    Serial.println("signalState");

    p.address = 9;
    p.type = Relay_2_Way;
    p.data = (uint8_t)counter;  // s1 | (s2 << 1)  |  (s3 <<2)  |   (s4<<3);
    // p.data =  s1 | (s2 << 1)  |  (s3 <<2)  |   (s4<<3);
    Serial.println("bin state");
    Serial.println(p.data, BIN);
    Mirf.send((byte*)&p);
    while (Mirf.isSending())
        ;
    // Mirf.send((byte*)"111");
    Serial.println("Done");
    counter++;
    if (counter == 16) {
        counter = 0;
    }
}

void setup() {
    configureEEPROMAddressForRFAndOTA("555");

    Serial.begin(9600);

    Serial.println("Init RF");

    startRF();

    Serial.println("INIT OK playground");
}

void loop() {
    signalState();

    Serial.println("listen....");
    //  delay(100);
    // return;
    // while (!Mirf.dataReady()) {
    //     watchdogReset();
    //     Serial.print(".");
    // };

    Serial.println(F("ok"));
    s1 = !s1;
    delay(3000);
}
