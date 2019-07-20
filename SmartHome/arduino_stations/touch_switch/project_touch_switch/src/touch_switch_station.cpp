#include <Arduino.h>
#include <PinChangeInt.h>
#include <common.h>

/*********************
      ██████╗ ███████╗██████╗ ██╗  ██╗              ███╗   ███╗███████╗███╗
███╗██████╗ ███████╗██████╗ ███████╗ ██╔══██╗██╔════╝╚════██╗██║  ██║ ████╗
████║██╔════╝████╗ ████║██╔══██╗██╔════╝██╔══██╗██╔════╝ ██████╔╝█████╗
█████╔╝███████║    █████╗    ██╔████╔██║█████╗  ██╔████╔██║██████╔╝█████╗
██████╔╝███████╗ ██╔══██╗██╔══╝  ██╔═══╝ ╚════██║    ╚════╝    ██║╚██╔╝██║██╔══╝
██║╚██╔╝██║██╔══██╗██╔══╝  ██╔══██╗╚════██║ ██║  ██║██║     ███████╗     ██║ ██║
╚═╝ ██║███████╗██║ ╚═╝ ██║██████╔╝███████╗██║  ██║███████║ ╚═╝  ╚═╝╚═╝ ╚══════╝
╚═╝              ╚═╝     ╚═╝╚══════╝╚═╝     ╚═╝╚═════╝ ╚══════╝╚═╝  ╚═╝╚══════╝

**********************/

#include <Mirf.h>
#include <MirfHardwareSpiDriver.h>
#include <SPI.h>
#include <nRF24L01.h>

// #define rfCE 9
// #define rfCS 10
// #define rfMISO 12
// #define rfMOSI 11
// #define rfCLK 13

// pin change interrupts groups are:
// A0 to A5
// D0 to D7
// D8 to D13
// So using 2 groups pin 7 on one group and pin A3 on the other group
// This way I do not have to ready pin state to make sure which one was changed.
#define SWITCH3_PIN_CHANGE_INTERRUPT 8
#define SWITCH4_PIN_CHANGE_INTERRUPT A3
// interrupts from touch button
#define interruptTouch1Pin 2
#define interruptTouch2Pin 3

#define STATUS_LED_PIN A2
// out to switch relay
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
// only needed for pin change switches - cannot set rais or fall only change
volatile int s3Counter = 0;
volatile int s4Counter = 0;

volatile boolean enabled = false;
boolean once = true;
unsigned long timestamp = 0;

enum Status_Type { Relay_4_Way = 1, Relay_2_Way = 2 };

typedef struct PayloadData {
    uint8_t address;
    uint8_t type;
    uint8_t data;
} Payload;

Payload p;

void signalState() {
    Serial.println("signalState");
    p.address = 7;
    p.type = Relay_4_Way;
    p.data = s1 | (s2 << 1) | (s3 << 2) | (s4 << 3);
    Serial.println("bin state");
    Serial.println(p.data, BIN);

    // Mirf.send((byte*)&p);
    // Mirf.send((byte*)"111");
    // wait for prev send to finish
    while (Mirf.isSending())
        ;
    Mirf.send((uint8_t*)(&p));
    while (Mirf.isSending())
        ;
    Serial.println("Done");
}

void toggles3() {
    if (millis() - timestamp < 500) {
        return;
    }
    timestamp = millis();
    s3 = !s3;
    digitalWrite(light3Pin, s3);
}

void touch3() {
    // Serial.println("TOUCH3 ?");
    if (!enabled) {
        return;
    }
    if (millis() - timestamp < 500) {
        return;
    }
    s3Counter++;
    if (s3Counter == 1) {
        toggles3();
        delay(50);
        signalState();
        Serial.println("TOUCH3");
    }
    if (s3Counter == 2) {
        s3Counter = 0;
    }
}

void toggles4() {
    if (millis() - timestamp < 500) {
        return;
    }
    timestamp = millis();
    s4 = !s4;
    digitalWrite(light4Pin, s4);
}

void touch4() {
    if (!enabled) {
        return;
    }
    if (millis() - timestamp < 500) {
        return;
    }
    s4Counter++;
    if (s4Counter == 1) {
        toggles4();
        delay(50);
        signalState();
        Serial.println("TOUCH4");
    }
    if (s4Counter == 2) {
        s4Counter = 0;
    }
}

void toggles1() {
    if (millis() - timestamp < 500) {
        return;
    }
    timestamp = millis();
    s1 = !s1;
    digitalWrite(light1Pin, s1);
}

void touch1() {
    if (!enabled) {
        return;
    }
    if (millis() - timestamp < 500) {
        return;
    }
    toggles1();
    delay(50);
    signalState();
    Serial.println("TOUCH1");
}
void toggles2() {
    if (millis() - timestamp < 500) {
        return;
    }
    timestamp = millis();
    s2 = !s2;
    digitalWrite(light2Pin, s2);
}

void touch2() {
    if (!enabled) {
        return;
    }
    if (millis() - timestamp < 500) {
        return;
    }
    toggles2();
    delay(50);
    signalState();
    Serial.println("TOUCH2");
}

void blinkReady() {
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(50);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(50);
}

void setup() {

    configureEEPROMAddressForRFAndOTA("007");

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

    Serial.println("INIT OK");
}

void loop() {
    if (!enabled) {
        digitalWrite(STATUS_LED_PIN, HIGH);
        delay(1000);
        digitalWrite(STATUS_LED_PIN, LOW);
        delay(1000);
        digitalWrite(STATUS_LED_PIN, HIGH);
        delay(1000);
        digitalWrite(STATUS_LED_PIN, LOW);
        delay(1000);
        digitalWrite(STATUS_LED_PIN, HIGH);
        delay(1000);
        digitalWrite(STATUS_LED_PIN, LOW);

        Serial.println("INIT PINS and INTERRUPTS");
        noInterrupts();

        pinMode(interruptTouch1Pin, INPUT_PULLUP);
        pinMode(interruptTouch2Pin, INPUT_PULLUP);
        pinMode(SWITCH3_PIN_CHANGE_INTERRUPT, INPUT);
        pinMode(SWITCH4_PIN_CHANGE_INTERRUPT, INPUT);

        attachInterrupt(digitalPinToInterrupt(interruptTouch1Pin), touch1,
                        RISING);
        attachInterrupt(digitalPinToInterrupt(interruptTouch2Pin), touch2,
                        RISING);
        attachPinChangeInterrupt(SWITCH3_PIN_CHANGE_INTERRUPT, touch3, CHANGE);
        attachPinChangeInterrupt(SWITCH4_PIN_CHANGE_INTERRUPT, touch4, CHANGE);
        Serial.println("Done init interrupts");
        enabled = true;
        interrupts();
    }
    //  watchdogReset();
    // Serial.print("TICK");

    if (once) {
        once = false;
        signalState();
    }

    while (millis() - timestamp < 500) {
        Serial.print(".");
    }

    Serial.println("listen....");
    //  delay(100);
    // return;
    while (!Mirf.dataReady()) {
        // Serial.print(".");
        watchdogReset();
        blinkReady();
    };

    Serial.println(F("ok"));
    // Serial.print("TICK");
    checkIfOtaRequestOrLoadCommand(cmd);
    Serial.print(F("New command: "));
    Serial.println(cmd);
    if (strcmp(cmd, "001") == 0) {
        Serial.println("remtoe restart");
        timestamp = millis();
        soft_restart();
        return;
    }
    // s1
    else if (strcmp(cmd, "002") == 0) {
        timestamp = millis();
        Serial.println("s1 off");
        s1 = LOW;
        digitalWrite(light1Pin, s1);
        return;
    } else if (strcmp(cmd, "003") == 0) {
        timestamp = millis();
        Serial.println("s1 on");
        s1 = HIGH;
        digitalWrite(light1Pin, s1);
        return;
    } else if (strcmp(cmd, "004") == 0) {
        Serial.println("toggle s1");
        toggles1();
        return;
    }
    // s2
    else if (strcmp(cmd, "005") == 0) {
        timestamp = millis();
        Serial.println("s2 off");
        s2 = LOW;
        digitalWrite(light2Pin, s2);
        return;
    } else if (strcmp(cmd, "006") == 0) {
        timestamp = millis();
        Serial.println("s2 on");
        s2 = HIGH;
        digitalWrite(light2Pin, s2);
        return;
    } else if (strcmp(cmd, "007") == 0) {
        Serial.println("toggle s2");
        toggles2();
        return;
    }
    // s3
    else if (strcmp(cmd, "008") == 0) {
        if (millis() - timestamp < 500) {
            return;
        }
        timestamp = millis();
        Serial.println("s3 off");
        s3 = LOW;
        digitalWrite(light3Pin, s3);
        return;
    } else if (strcmp(cmd, "009") == 0) {
        if (millis() - timestamp < 500) {
            return;
        }
        timestamp = millis();
        Serial.println("s3 on");
        s3 = HIGH;
        digitalWrite(light3Pin, s3);
        return;
    } else if (strcmp(cmd, "010") == 0) {
        Serial.println("toggle s3");
        toggles3();
        return;
    }
    // s4
    else if (strcmp(cmd, "011") == 0) {
        timestamp = millis();
        Serial.println("s4 off");
        s4 = LOW;
        digitalWrite(light4Pin, s4);
        return;
    } else if (strcmp(cmd, "012") == 0) {
        timestamp = millis();
        Serial.println("s4 on");
        s4 = HIGH;
        digitalWrite(light4Pin, s4);
        return;
    } else if (strcmp(cmd, "013") == 0) {
        Serial.println("toggle s4");
        toggles4();
        return;
    }
}
