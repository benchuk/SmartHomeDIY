//#define USE_RF_NANO_BOARD
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <common.h>

SoftwareSerial BTSerial(6, 7);  // RX | TX

bool firstPart = true;
char add[4];
int ttt = 0;
int i = 0;
bool newCommand = false;
char response[6];
// void configureEEPROMAddressForRFAndOTA(void)
// {
//   //my address "001"
//   EEPROM.write(0, 48);
//   EEPROM.write(1, 48);
//   EEPROM.write(2, 49);

//   //OTA station address (always "000")
//   EEPROM.write(3, 48);
//   EEPROM.write(4, 48);
//   EEPROM.write(5, 48);
// }

void setup() {
    // STRING NULL TERMINATION
    add[3] = 0;
    add[0] = 'x';
    add[1] = 'x';
    add[2] = 'x';

    Serial.begin(115200);
    Serial.println("setup...");
    configureEEPROMAddressForRFAndOTA("001");
    startRF();
    // BTSerial.begin(38400);  // HC-05 default speed in AT command more
    BTSerial.begin(9600);  // custom
    Serial.println("Dispatch station ready.");
}
// char otaCmd[Mirf.payload + 1];
char otaCmd[4];
void loop() {
    watchdogReset();

    if (checkIfOtaRequestOrLoadCommand(otaCmd)) {
        // NEED TO TEST THIS WITH DATA FROM REAL ENDPOINT LIGHTS STATION
        Serial.println("got state ");
        Serial.println((uint8_t)otaCmd[0]);
        Serial.println((uint8_t)otaCmd[1]);
        Serial.println((uint8_t)otaCmd[2]);
        // while (BTSerial.availableForWrite()) {
        //     delay(5);
        // }
        BTSerial.write(otaCmd, 3);
        // BTSerial.write(otaCmd[0]);
        // BTSerial.write(otaCmd[1]);
        // BTSerial.write(otaCmd[2]);
        BTSerial.flush();
        // while (BTSerial.availableForWrite()) {
        //     delay(5);
        // }
    }
    // else
    // {
    //    Serial.print("NO DATA");
    // }

    // THIS TEST WORKS
    // if (true) {
    //     // checkIfOtaRequestOrLoadCommand((uint8_t *)"111");
    //     BTSerial.write("111");
    //     Serial.print("sent: ");
    //     delay(1000);
    //     Serial.print("continue");
    // };
    // digitalWrite(Radio_CSN, LOW); // ENABLE radio
    // Keep reading from HC-05 and send to Arduino Serial Monitor
    if (BTSerial.available()) {
        char command = BTSerial.read();
        Serial.print("new command char: ");
        Serial.println(command);

        //TBD: Handle none number or ':' chars and restart
        if (command == ':') {
            Serial.println("middle");
            i = 0;
            firstPart = false;
            return;
        }
        if (firstPart) {
            Serial.println("firstPart");
            Serial.println(command);
            Serial.println(i);
            add[i] = command;
            Serial.println("========");
            Serial.println(add[i]);
            Serial.println("========");
            i++;
        } else {
            Serial.println("end");
            cmd[i] = command;
            i++;
            if (i == 3) {
                firstPart = true;
                i = 0;
                newCommand = true;
            }
        }

        // delay(1000);
        // add code to check what was requested from serial server (PI station)

        if (newCommand) {
            Serial.println("sending: ");
            Serial.print("address: ");
            Serial.println(add);
            Mirf.setTADDR((byte *)add);
            Serial.print("command: ");
            Serial.println(cmd);
            Mirf.send((uint8_t *)cmd);
            newCommand = false;
            while (Mirf.isSending()) {
                Serial.print(".-");
            }
            // ;
        }
    }
}
