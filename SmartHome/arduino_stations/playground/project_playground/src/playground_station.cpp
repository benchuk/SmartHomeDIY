
//https://forum.arduino.cc/index.php?topic=102430.0
//https://gist.github.com/chendy/69454f6ec77b54f9a710
#include <Arduino.h>
/* Code to pulse pin 3 with a modulated signal
* Can be used to drive an IR LED to keep a TSOP IR reciever happy
* This allows you to use a modulated reciever and a continious beam detector
* By Mike Cook Nov 2011 - Released under the Open Source licence
*/
volatile byte pulse = 0;

ISR(TIMER2_COMPB_vect) {  // Interrupt service routine to pulse the modulated pin 3
    pulse++;
    if (pulse >= 8) {  // change number for number of modulation cycles in a pulse
        pulse = 0;
        //TCCR2A ^= _BV(COM2B1);  // toggle pin 3 enable, turning the pin on and off
    }
}

void setIrModOutput() {  // sets pin 3 going at the IR modulation rate
    pinMode(3, OUTPUT);
    TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);  // Just enable output on Pin 3 and disable it on Pin 11
    TCCR2B = _BV(WGM22) | _BV(CS22);
    OCR2A = 51;                          // defines the frequency 51 = 38.4 KHz, 54 = 36.2 KHz, 58 = 34 KHz, 62 = 32 KHz
    OCR2B = 26;                          // deines the duty cycle - Half the OCR2A value for 50%
    TCCR2B = TCCR2B & 0b00111000 | 0x2;  // select a prescale value of 8:1 of the system clock
}

void setup() {
    setIrModOutput();
    TIMSK2 = _BV(OCIE2B);  // Output Compare Match B Interrupt Enable
}

void loop() {
    // do something here
}
// #include <Arduino.h>
// #include <Mirf.h>
// #include <MirfHardwareSpiDriver.h>
// #include <PinChangeInt.h>
// #include <SPI.h>
// #include <common.h>
// #include <nRF24L01.h>

// // switch state for lights
// // RELAY (mechanical ones) are active low - set high so default is lights off
// volatile byte s1 = HIGH;
// volatile byte s2 = HIGH;
// volatile byte s3 = HIGH;
// volatile byte s4 = HIGH;

// // char* myAddress = "999";
// // uint8_t address = 9;

// enum Status_Type { Relay_4_Way = 1,
//                    Relay_2_Way = 2 };

// typedef struct PayloadData {
//     uint8_t address;
//     uint8_t type;
//     uint8_t data;
// } Payload;

// Payload p;

// uint8_t counter = 0;
// void signalState() {
//     Serial.println("signalState");

//     p.address = 9;
//     p.type = Relay_2_Way;
//     p.data = (uint8_t)counter;  // s1 | (s2 << 1)  |  (s3 <<2)  |   (s4<<3);
//     // p.data =  s1 | (s2 << 1)  |  (s3 <<2)  |   (s4<<3);
//     Serial.println("bin state");
//     Serial.println(p.data, BIN);
//     Mirf.send((byte*)&p);
//     while (Mirf.isSending())
//         ;
//     // Mirf.send((byte*)"111");
//     Serial.println("Done");
//     counter++;
//     if (counter == 16) {
//         counter = 0;
//     }
// }

// void setup() {
//     configureEEPROMAddressForRFAndOTA("555");

//     Serial.begin(9600);

//     Serial.println("Init RF");

//     startRF();

//     Serial.println("INIT OK playground");
// }

// void loop() {
//     signalState();

//     Serial.println("listen....");
//     //  delay(100);
//     // return;
//     // while (!Mirf.dataReady()) {
//     //     watchdogReset();
//     //     Serial.print(".");
//     // };

//     Serial.println(F("ok"));
//     s1 = !s1;
//     delay(3000);
// }
