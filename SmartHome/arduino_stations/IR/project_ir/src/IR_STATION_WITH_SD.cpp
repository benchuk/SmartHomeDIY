/************************
  https://learn.adafruit.com/trinket-gemma-ir-remote-control/using-ir-codes-1
  https://www.instructables.com/id/Clone-a-Remote-with-Arduino/
   http://www.instructables.com/id/Clone-a-Remote-with-Arduino/
   https://www.arduino.cc/en/Tutorial/ReadWrite
   check this for sd and rf on the same spi
   https://forum.arduino.cc/index.php?topic=360718.0
   other sd card interesting stuff: https://www.arduino.cc/en/Tutorial/DumpFile
   http://forum.arduino.cc/index.php?topic=104109.0

**********************/

/*********************
   SPI init for IR and SDCARD on same BUS
   Record IR command to SDCARD
   Wait for dispatcher station to get command from PI server and send it here
   Send this command by loading from SDCARD to mem and then send via IR
**********************/

/* Raw IR decoder sketch!

  This sketch/program uses the Arduino and a PNA4602 to
  decode IR received. This can be used to make a IR receiver
  (by looking for a particular code)
  or transmitter (by pulsing an IR LED at ~38KHz for the
  durations detected

  Code is public domain, check out www.ladyada.net and adafruit.com
  for more tutorials!
*/

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
#include <SD.h>
#include <common.h>
File myFile;
#define SelectSD 4

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

/*********************
IR
**********************/
/*****
   We need to use the 'raw' pin reading methods
   because timing is very important here and the digitalRead()
   procedure is slower!
   Note: Digital pin #2 is the same as Pin D2 see http://arduino.cc/en/Hacking/PinMapping168 for the 'raw' pin mapping
*************************/

//PORTD pin 2 as input for IR signal fast recording
#define IRpin_PIN PIND
#define IRpin 2  //IR Input PIN SIGNAL pin #2 for recording new IR code

//MAXPULSE defined when to timeout the recording
// MAXPULSE*RESOLUTION = 9000*18 = 162000 microseconds -> 162(ms) max pulse width timeout
uint16_t MAXPULSE = 9000;

/*********
   what our timing resolution should be, larger is better
   as its more 'precise' - but too large and you wont get
   accurate timing minimum is 2 as delayMicroseconds will not work with smaller value
************************/
#define RESOLUTION 18  //20;

// we will store up to 120 pulse pairs (this is -a lot-)
uint16_t pulses[120][2];    // pair is high and low pulse
uint16_t currentpulse = 0;  // index for pulses we're storing

// sending the IR signal is done using PIN # 7 - in code we use port manipulation
//    PORTD |= B10000000 - which is pin 7 in output mode
#define IR_LED_PIN 7    // OUT IR-LED  connected to digital pin 7 - OUT PIN - used to send the IR code
#define REC_LED_PIN 5   // OUT Pin: Color led to indicate to user that we are recording IR signal
#define SEND_LED_PIN 6  // OUT Pin: Color led to indicate to user that we are sending IR signal

bool coderecorded = true;
int counter = 0;  //used for logic

#define RF_SWITCH_LINE1 8
#define SD_SWITCH_LINE1 A5  //due to collistion with bootloader

char fileName[8];
String buffer;

void sendIRCode();
void pulseIR(long microsecs);
void recordircode();
void printAndSavePulsesToSDCard(void);
void disableRF(void);
void enableRF(void);
void disableSD(void);
void enableSD(void);
void disableAllSpiDevices(void);
void commandToFileName(char *f, char *c);
void updateRecLed(void);
void updateUser_SendingLed(bool sending);
void reportLedCriticalError();
void blinkReady();

void setup(void) {
    watchdogReset();
    configureEEPROMAddressForRFAndOTA("006");

    pinMode(RF_SWITCH_LINE1, OUTPUT);
    pinMode(SD_SWITCH_LINE1, OUTPUT);

    pinMode(IR_LED_PIN, OUTPUT);
    pinMode(SelectSD, OUTPUT);
    pinMode(Radio_CSN, OUTPUT);
    pinMode(REC_LED_PIN, OUTPUT);
    pinMode(SEND_LED_PIN, OUTPUT);

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
    disableRF();
    watchdogReset();

    //SPI SDCARD
    Serial.println("SD INIT");
    enableSD();
    delay(500);
    if (!SD.begin(SelectSD)) {
        Serial.println(F("initialization failed!"));
        reportLedCriticalError();
    } else {
        Serial.println(F("initialization ok."));
    }
    delay(1000);
    coderecorded = true;
    watchdogReset();
    // if (coderecorded) {
    //     //Serial.println(F("Play mode."));
    // } else {
    //     //Serial.println(F("Record mode."));
    // }
    updateRecLed();
    disableSD();

    //Init completed
    watchdogReset();
    disableAllSpiDevices();
    watchdogReset();
    digitalWrite(SEND_LED_PIN, HIGH);
    delay(1000);
    digitalWrite(SEND_LED_PIN, LOW);
    delay(500);
    digitalWrite(SEND_LED_PIN, HIGH);
    delay(1000);
    digitalWrite(SEND_LED_PIN, LOW);
    watchdogReset();
    //Serial.println(F("Ready to decode IR!"));
}

/**********************
   Main Arduino Loop
**********************/
void loop(void) {
    if (!coderecorded) {
        //Serial.println(F("Recording IR Signal..."));
        recordircode();
        return;
    }

    enableRF();
    //Serial.println(F("Waiting for remote command..."));
    while (!Mirf.dataReady()) {
        //delay(100);
        blinkReady();
    };

    checkIfOtaRequestOrLoadCommand(cmd);
    //Serial.print(F("Got New command: "));
    //Serial.println(cmd);

    //000 is record
    if (strcmp(cmd, "000") == 0) {
        Serial.print(F("Record cmd: waiting for record file name"));
        while (!Mirf.dataReady()) {
            Serial.print(F("."));
            watchdogReset();
            delay(100);
        }
        char cmd[Mirf.payload + 1];
        Mirf.getData(cmd);
        cmd[3] = 0;
        Serial.print(F("cmd: "));
        Serial.println(cmd);
        commandToFileName(fileName, cmd);
        Serial.print(F("File Name: "));
        Serial.println(fileName);
        Serial.println(F("Starting record sequence."));
        disableRF();
        coderecorded = false;
        updateRecLed();
        return;
    } else if (strcmp(cmd, "001") == 0) {
        Serial.println(F("Restarting..."));
        soft_restart();
        return;
    } else {
        disableRF();

        //Serial.print("Got IR cmd: ");
        //Serial.println(cmd);

        enableSD();

        if (!SD.begin(SelectSD)) {
            //Serial.println(F("move to SD ERROR")); //dbg
        }
        commandToFileName(fileName, cmd);

        myFile = SD.open(fileName);
        if (myFile) {
            //Serial.println(F("File open ok \r\n"));
            sendIRCode();
        } else {
            //wait and try again
            delay(100);
            myFile = SD.open(fileName);
            if (myFile) {
                sendIRCode();
            } else {
                Serial.println(F("File open Error file \n\r\n"));
                reportLedCriticalError();
            }
        }
    }

    disableAllSpiDevices();
}  //end of loop

/*********
   Load IR command from SD and send it via IR
**********************/
void sendIRCode() {
    updateUser_SendingLed(true);
    enableSD();

    //readStringUntil reference: http://stackoverflow.com/questions/35486045/arduino-reading-sd-file-line-by-line-c
    buffer = myFile.readStringUntil('\n');
    //to int reference: https://www.arduino.cc/en/Tutorial/StringToIntExample
    counter = buffer.toInt();
    //Serial.print(counter);                            //dbg
    //Serial.println(" number of line in file header"); //dbg
    for (uint8_t i = 0; i < counter; i++) {
        buffer = myFile.readStringUntil('\n');
        //buffer.toInt() * RESOLUTION
        pulses[i][0] = buffer.toInt();
        buffer = myFile.readStringUntil('\n');
        pulses[i][1] = buffer.toInt();
        //Serial.print("  0  -  ");                     //dbg
        //Serial.print(pulses[i][0] * RESOLUTION, DEC); //dbg
        //Serial.print("  1  -  ");                     //dbg
        //Serial.print(pulses[i][1] * RESOLUTION, DEC); //dbg
        //Serial.println("");                           //dbg
    }
    //Serial.println("will close file");//dbg
    myFile.close();
    //Serial.println("done loading command from file");//dbg

    updateUser_SendingLed(false);
    delay(50);
    updateUser_SendingLed(true);

    //pulseIR(9000);
    //delayMicroseconds(4500);

    cli();  //Disable interrupts
    for (uint8_t i = 0; i < counter; i++) {
        unsigned long t = (unsigned long)(pulses[i][0]) * RESOLUTION;
        //Serial.print("down time: ");
        //Serial.println(t, DEC); //dbg
        //unsigned long time = millis();
        //delayMicroseconds(t - RESOLUTION);
        //Serial.print("dt (ms)  is: ");
        //Serial.println(millis()); //prints time since program started

        //Can not use delay as it does not work when disabling interrupts and also is not accurate
        //so have to use delayMicroseconds
        //why split delayMicroseconds to chuncks of  16383? - this is why...
        //https://www.arduino.cc/reference/en/language/functions/time/delaymicroseconds/
        //Currently, the largest value that will produce an accurate delay is 16383.
        //This could change in future Arduino releases. For delays longer than a few thousand microseconds, you should use delay() instead.

        if (t > 16383) {
            long loopremaining = t;
            //Serial.print("loopremaining  is: ");
            //Serial.println(loopremaining, DEC);
            while (loopremaining > 0) {
                unsigned long d = min(16383, loopremaining);
                //Serial.print("d  is: ");
                // Serial.println(d, DEC);
                delayMicroseconds(d - RESOLUTION);
                loopremaining -= d;
                //Serial.print("loopremaining  is: ");
                //Serial.println(loopremaining, DEC);
            }
        } else {
            delayMicroseconds(t - RESOLUTION);
        }
        pulseIR((((unsigned long)pulses[i][1]) * RESOLUTION));
    }
    sei();  //Enables interrupts
    updateUser_SendingLed(false);
    //Serial.println("DONE");
}  //end of load and send command

/*********
      pulseIR
      This procedure sends a 38KHz pulse to the IR_LED_PIN
      for a certain # of microseconds. We'll use this whenever we need to send codes
 ********************/
void pulseIR(long microsecs) {
    /***********
     we'll count down from the number of microseconds we are told to wait
  *************/

    // Serial.println("pulseIR microsecs");
    //   Serial.println(microsecs);

    while (microsecs > 0) {
        //   PORTD |= B10000000;
        //   //digitalWrite(IR_LED_PIN, HIGH); // this takes about 3 microseconds to happen
        //   //   delayMicroseconds(8);         // hang out for 10 microseconds
        //   delayMicroseconds(14); // hang out for 10 microseconds
        //   //digitalWrite(IR_LED_PIN, LOW); // this also takes about 3 microseconds
        //   PORTD ^= B10000000;
        //   //digitalWrite(IR_LED_PIN, LOW); // this takes about 3 microseconds to happen
        //   //   delayMicroseconds(8);         // hang out for 10 microseconds
        //   delayMicroseconds(14); // hang out for 10 microseconds
        // 38 kHz is about 13 microseconds high and 13 microseconds low
        PORTD |= B10000000;  // this takes about 3 microseconds to happen
        //PORTD |= B10000000;
        //   delayMicroseconds(8);         // hang out for 10 microseconds
        delayMicroseconds(14);  // hang out for 10 microseconds
        PORTD ^= B10000000;     // this also takes about 3 microseconds
        //PORTD ^= B00000000;
        //   delayMicroseconds(8);         // hang out for 10 microseconds
        delayMicroseconds(14);  // hang out for 10 microseconds

        // so 26 microseconds altogether
        microsecs -= 26;
    }
}

uint16_t highpulse, lowpulse;  // temporary storage timing
void recordircode() {
    highpulse = lowpulse = 0;  // start out with no pulse length
    //  while (digitalRead(IRpin)) { // this is too slow!
    while (IRpin_PIN & (1 << IRpin)) {
        /*****************************************
      // pin is still HIGH
      // count off another few microseconds
    ****************************************/
        highpulse++;
        delayMicroseconds(RESOLUTION);

        /*****************************************
      // If the pulse is too long, we 'timed out' - either nothing
      // was received or the code is finished, so print what
      // we've grabbed so far, and then reset
    **************************/
        if ((highpulse >= MAXPULSE) && (currentpulse != 0)) {
            //Serial.println(F("break highpulse....")); //dbg
            //Serial.println(highpulse);                //dbg
            printAndSavePulsesToSDCard();
            currentpulse = 0;
            return;
        }
    }
    //Serial.print(F("done highpulse with count: ")); //dbg
    //Serial.println(highpulse);                      //dbg
    /********************************
    // we didn't time out so lets stash the reading
  *********************************/
    pulses[currentpulse][0] = highpulse;

    // same as above
    while (!(IRpin_PIN & _BV(IRpin))) {
        // pin is still LOW
        lowpulse++;
        delayMicroseconds(RESOLUTION);
        if ((lowpulse >= MAXPULSE) && (currentpulse != 0)) {
            //Serial.println(F("break lowpulse....")); //dbg
            //Serial.println(lowpulse);                //dbg
            printAndSavePulsesToSDCard();
            currentpulse = 0;
            return;
        }
    }
    pulses[currentpulse][1] = lowpulse;

    // we read one high-low pulse successfully, continue!
    currentpulse++;
    //Serial.println(currentpulse);
}

void printAndSavePulsesToSDCard(void) {
    coderecorded = true;
    updateRecLed();
    counter = currentpulse;
    Serial.print(F("Command Data Size: "));  //dbg
    Serial.println(counter);                 //dbg
    //Serial.println("1 \n\r\n");//dbg
    //TODO: Which command to save to which file
    disableRF();
    enableSD();

    if (!SD.begin(SelectSD)) {
        Serial.println(F("move to SD ERROR"));  //dbg
    }

    Serial.print(F("Saving data to file name: "));
    Serial.println(fileName);

    if (SD.exists(fileName)) {
        Serial.print(F("found file deleting first"));
        SD.remove(fileName);
    }

    delay(100);

    myFile = SD.open(fileName, FILE_WRITE);
    //Serial.println("sd open ok \r\n"); //dbg
    if (myFile) {
        myFile.println(counter);
        Serial.println(F("writing command size to file ok \r\n"));  //dbg
    } else {
        Serial.println(F("Error open file \n\r\n"));
        reportLedCriticalError();
    }

    //Serial.println("\n\r\n\rReceived: \n\rOFF \tON");//dbg
    for (uint8_t i = 0; i < currentpulse; i++) {
        Serial.print(pulses[i][0], DEC);  //dbg
        Serial.print(" usec, ");          //dbg
        // if the file opened okay, write to it:
        if (myFile) {
            myFile.println(pulses[i][0]);
        } else {
            Serial.println(F("File not open \r\n"));  //dbg
        }

        Serial.print(pulses[i][1], DEC);  //dbg
        Serial.println(F(" usec"));       //dbg
        if (myFile) {
            myFile.println(pulses[i][1]);
        }
    }
    myFile.close();
    //dbg
    //Serial.println(F("Close file ok \r\n"));
    //enable for debug
    // print it in a 'array' format
    //Serial.println("int IRsignal[] = {");//dbg
    //Serial.println("// ON, OFF (in 10's of microseconds)");//dbg
    //for (uint8_t i = 0; i < currentpulse - 1; i++) {//dbg
    //Serial.print("\t"); // tab//dbg
    //Serial.print(pulses[i][1] * RESOLUTION / 10, DEC);//dbg
    //Serial.print(", ");//dbg
    //Serial.print(pulses[i + 1][0] * RESOLUTION / 10, DEC);//dbg
    //Serial.println(",");//dbg
    //}
    //Serial.print("\t"); // tab  //dbg
    //Serial.print(pulses[currentpulse - 1][1] * RESOLUTION / 10, DEC);//dbg
    //Serial.print(", 0};");//dbg
}

void updateRecLed(void) {
    if (!coderecorded) {
        digitalWrite(REC_LED_PIN, HIGH);
    } else {
        digitalWrite(REC_LED_PIN, LOW);
    }
}

void updateUser_SendingLed(bool sending) {
    if (sending) {
        digitalWrite(SEND_LED_PIN, HIGH);
    } else {
        digitalWrite(SEND_LED_PIN, LOW);
    }
}

void reportLedCriticalError() {
    while (1) {
        digitalWrite(SEND_LED_PIN, HIGH);
        delay(500);
        digitalWrite(SEND_LED_PIN, LOW);
        delay(500);
    }
}

void blinkReady() {
    digitalWrite(REC_LED_PIN, HIGH);
    delay(50);
    digitalWrite(REC_LED_PIN, LOW);
    delay(50);
}

void enableSD(void) {
    digitalWrite(SD_SWITCH_LINE1, HIGH);
    digitalWrite(RF_SWITCH_LINE1, LOW);
    digitalWrite(Radio_CSN, HIGH);  //Disable radio
    digitalWrite(SelectSD, LOW);    //Enable sd-card
    delay(10);
}

void enableRF(void) {
    digitalWrite(SD_SWITCH_LINE1, LOW);
    digitalWrite(RF_SWITCH_LINE1, HIGH);
    digitalWrite(SelectSD, HIGH);  //Disable sd-card
    digitalWrite(Radio_CSN, LOW);  //Enable radio
    delay(10);
}

void disableRF(void) {
    //digitalWrite(RELAY, HIGH);
    //digitalWrite(REALY2, HIGH);
    digitalWrite(RF_SWITCH_LINE1, LOW);
    digitalWrite(Radio_CSN, HIGH);  //Disable radio
    delay(10);
}

void disableSD(void) {
    delay(10);
    digitalWrite(SD_SWITCH_LINE1, LOW);
    digitalWrite(SelectSD, HIGH);  //Disable sd-card
}

void disableAllSpiDevices(void) {
    delay(10);
    digitalWrite(RF_SWITCH_LINE1, LOW);
    digitalWrite(SD_SWITCH_LINE1, LOW);
    digitalWrite(Radio_CSN, HIGH);  //Disable radio
    digitalWrite(SelectSD, HIGH);   //Disable sd-card
}

void commandToFileName(char *f, char *c) {
    strncpy(f, c, 3);
    //memcpy( fileName,cmd , 3*sizeof(char) );
    // fileName[0] = cmd[0];
    // fileName[1] = cmd[1];
    // fileName[2] = cmd[2];
    f[3] = '.';
    f[4] = 't';
    f[5] = 'x';
    f[6] = 't';
    f[7] = 0;
    //Serial.println(f);
}

//Test code for 38Khz send signal
// cli(); //Disable interrupts
// while (1)
// {
//   PORTD |= B10000000;
//   //digitalWrite(IR_LED_PIN, HIGH); // this takes about 3 microseconds to happen
//   //   delayMicroseconds(8);         // hang out for 10 microseconds
//   delayMicroseconds(14); // hang out for 10 microseconds
//   //digitalWrite(IR_LED_PIN, LOW); // this also takes about 3 microseconds
//   PORTD ^= B10000000;
//   //digitalWrite(IR_LED_PIN, LOW); // this takes about 3 microseconds to happen
//   //   delayMicroseconds(8);         // hang out for 10 microseconds
//   delayMicroseconds(14); // hang out for 10 microseconds
// }

// Mirf.csnPin = Radio_CSN;
// Mirf.spi = &MirfHardwareSpi;
// Mirf.init();
// Mirf.setRADDR((byte *)"irir1");
// Mirf.payload = sizeof(unsigned long);
// Mirf.config();
