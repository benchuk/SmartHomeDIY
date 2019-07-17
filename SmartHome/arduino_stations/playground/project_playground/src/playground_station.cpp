#include <Arduino.h>
#include <PinChangeInt.h>
#include <common.h>

#include <Mirf.h>
#include <MirfHardwareSpiDriver.h>
#include <SPI.h>
#include <nRF24L01.h>

#define light1Pin 4
#define light2Pin 5
#define light3Pin 6
#define light4Pin 7

// switch state for lights
// RELAY (mechanical ones) are active low - set high so default is lights off
volatile byte s1 = HIGH;
volatile byte s2 = HIGH;
volatile byte s3 = HIGH;
volatile byte s4 = HIGH;

char* myAddress = "999";
uint8_t address = 9;

enum Status_Type { Relay_4_Way = 1, Relay_2_Way = 2 };

typedef struct PayloadData {
    uint8_t address;
    uint8_t type;
    uint8_t data;
} Payload;
  Payload p;
void signalState() {
    Serial.println("signalState");
  
    // p.address = address;
    // p.type = Relay_4_Way;
    // p.data = s1 | (s2 << 1)  |  (s3 <<2)  |   (s4<<3);
    // Serial.println("bin state");
    // Serial.println(p.data, BIN);
    Mirf.send((uint8_t*)"111");
}

void setup() {

    configureEEPROMAddressForRFAndOTA("555");

    Serial.begin(9600);

    pinMode(light1Pin, OUTPUT);
    pinMode(light2Pin, OUTPUT);
    pinMode(light3Pin, OUTPUT);
    pinMode(light4Pin, OUTPUT);

    digitalWrite(light1Pin, s1);
    digitalWrite(light2Pin, s2);
    digitalWrite(light3Pin, s3);
    digitalWrite(light4Pin, s4);

    Serial.println("Init RF");

    startRF();

    Serial.println("INIT OK playground");
}

void loop() {

    signalState();

    Serial.println("listen....");
    //  delay(100);
    // return;
    while (!Mirf.dataReady()) {
        watchdogReset();
        Serial.print(".");
    };

    Serial.println(F("ok"));
    s1 = !s1;
    delay(1000);
}
