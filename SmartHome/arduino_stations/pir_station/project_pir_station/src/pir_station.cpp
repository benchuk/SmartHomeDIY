#include <Arduino.h>
#include <Mirf.h>
#include <MirfHardwareSpiDriver.h>
#include <PinChangeInt.h>
#include <SPI.h>
#include <common.h>
#include <nRF24L01.h>

int pirSensor = 5;
int pirState = LOW;
int val = 0;

// enum Status_Type {
//     Relay_4_Way = 1,
//     Relay_2_Way = 2,
//     TEMP_STATION = 3,
//     HUMIDITY_STATION = 4,
//     PIR_STATION = 5
// };

// typedef struct PayloadData {
//     uint8_t address;
//     uint8_t type;
//     uint8_t data;
// } Payload;

Payload p;

uint8_t isMotionDetected = 0;
int prevPirState = 0;
void signalState() {
    Serial.println("signalState");
    p.address = 102;
    p.type = PIR_STATION;
    p.data = isMotionDetected;
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
    configureEEPROMAddressForRFAndOTA("102");

    Serial.begin(9600);

    Serial.println("Init RF");

    startRF();

    pinMode(pirSensor, INPUT);

    Serial.println("INIT OK PIR Station");
}

void loop() {
    // signalState();

    //Serial.println("listen....");

    int sensorValue = digitalRead(pirSensor);
    if (prevPirState != sensorValue) {
        if (sensorValue == HIGH) {
            isMotionDetected = 1;
            Serial.println("High");
            signalState();
        } else {
            isMotionDetected = 0;
            Serial.println("Low");
            signalState();
        }
    }
    prevPirState = sensorValue;
    delay(500);
}
