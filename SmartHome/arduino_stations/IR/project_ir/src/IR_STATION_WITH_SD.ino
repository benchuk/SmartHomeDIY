
/*********************
 * Using  "ANSI Shadow" art font.
   Version: 1.10.2016 - Version 1
**********************/

/*********************
 ██╗   ██╗███████╗██████╗ ███████╗██╗ ██████╗ ███╗   ██╗    ███████╗███████╗ █████╗ ████████╗██╗   ██╗██████╗ ███████╗███████╗
 ██║   ██║██╔════╝██╔══██╗██╔════╝██║██╔═══██╗████╗  ██║    ██╔════╝██╔════╝██╔══██╗╚══██╔══╝██║   ██║██╔══██╗██╔════╝██╔════╝
 ██║   ██║█████╗  ██████╔╝███████╗██║██║   ██║██╔██╗ ██║    █████╗  █████╗  ███████║   ██║   ██║   ██║██████╔╝█████╗  ███████╗
 ╚██╗ ██╔╝██╔══╝  ██╔══██╗╚════██║██║██║   ██║██║╚██╗██║    ██╔══╝  ██╔══╝  ██╔══██║   ██║   ██║   ██║██╔══██╗██╔══╝  ╚════██║
  ╚████╔╝ ███████╗██║  ██║███████║██║╚██████╔╝██║ ╚████║    ██║     ███████╗██║  ██║   ██║   ╚██████╔╝██║  ██║███████╗███████║
   ╚═══╝  ╚══════╝╚═╝  ╚═╝╚══════╝╚═╝ ╚═════╝ ╚═╝  ╚═══╝    ╚═╝     ╚══════╝╚═╝  ╚═╝   ╚═╝    ╚═════╝ ╚═╝  ╚═╝╚══════╝╚══════╝
                                                                                                                              
 * *******************
   SPI init for IR and SDCARD on same BUS
   Record IR command to SDCARD
   Wait for dispatcher station to get command from PI server and send it here
   Send this command by loading from SDCARD to mem and then send via IR
**********************/

/************************
   ██╗███╗   ███╗██████╗  ██████╗ ██████╗ ████████╗ █████╗ ███╗   ██╗████████╗    ██████╗ ███████╗███████╗███████╗██████╗ ███████╗███╗   ██╗ ██████╗███████╗███████╗
   ██║████╗ ████║██╔══██╗██╔═══██╗██╔══██╗╚══██╔══╝██╔══██╗████╗  ██║╚══██╔══╝    ██╔══██╗██╔════╝██╔════╝██╔════╝██╔══██╗██╔════╝████╗  ██║██╔════╝██╔════╝██╔════╝
   ██║██╔████╔██║██████╔╝██║   ██║██████╔╝   ██║   ███████║██╔██╗ ██║   ██║       ██████╔╝█████╗  █████╗  █████╗  ██████╔╝█████╗  ██╔██╗ ██║██║     █████╗  ███████╗
   ██║██║╚██╔╝██║██╔═══╝ ██║   ██║██╔══██╗   ██║   ██╔══██║██║╚██╗██║   ██║       ██╔══██╗██╔══╝  ██╔══╝  ██╔══╝  ██╔══██╗██╔══╝  ██║╚██╗██║██║     ██╔══╝  ╚════██║
   ██║██║ ╚═╝ ██║██║     ╚██████╔╝██║  ██║   ██║   ██║  ██║██║ ╚████║   ██║       ██║  ██║███████╗██║     ███████╗██║  ██║███████╗██║ ╚████║╚██████╗███████╗███████║
   ╚═╝╚═╝     ╚═╝╚═╝      ╚═════╝ ╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═══╝   ╚═╝       ╚═╝  ╚═╝╚══════╝╚═╝     ╚══════╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═══╝ ╚═════╝╚══════╝╚══════╝
                                                                                                                                                                    
 * **********************
   http://www.instructables.com/id/Clone-a-Remote-with-Arduino/
   https://www.arduino.cc/en/Tutorial/ReadWrite
   check this for sd adn rf on the same spi
   https://forum.arduino.cc/index.php?topic=360718.0
   ohter sd card interesting stuff: https://www.arduino.cc/en/Tutorial/DumpFile
   http://forum.arduino.cc/index.php?topic=104109.0

**********************/

/*********************
    ██████╗██████╗ ███████╗██████╗ ██╗████████╗███████╗
   ██╔════╝██╔══██╗██╔════╝██╔══██╗██║╚══██╔══╝██╔════╝
   ██║     ██████╔╝█████╗  ██║  ██║██║   ██║   ███████╗
   ██║     ██╔══██╗██╔══╝  ██║  ██║██║   ██║   ╚════██║
   ╚██████╗██║  ██║███████╗██████╔╝██║   ██║   ███████║
    ╚═════╝╚═╝  ╚═╝╚══════╝╚═════╝ ╚═╝   ╚═╝   ╚══════╝
                                                       
**********************/

/* Raw IR decoder sketch!

  This sketch/program uses the Arduno and a PNA4602 to
  decode IR received. This can be used to make a IR receiver
  (by looking for a particular code)
  or transmitter (by pulsing an IR LED at ~38KHz for the
  durations detected

  Code is public domain, check out www.ladyada.net and adafruit.com
  for more tutorials!
*/

/*
  SD card read/write

  This example shows how to read and write data to and from an SD card file
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4

  5V POWER Taken from Vin pin - workaroud - because I have run out of 5v pins
  see trick here: http://www.open-electronics.org/the-power-of-arduino-this-unknown/

  created   Nov 2010
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/

/*********************
   ███████╗██████╗  ██████╗ █████╗ ██████╗ ██████╗               ███╗   ███╗███████╗███╗   ███╗██████╗ ███████╗██████╗ ███████╗
   ██╔════╝██╔══██╗██╔════╝██╔══██╗██╔══██╗██╔══██╗              ████╗ ████║██╔════╝████╗ ████║██╔══██╗██╔════╝██╔══██╗██╔════╝
   ███████╗██║  ██║██║     ███████║██████╔╝██║  ██║    █████╗    ██╔████╔██║█████╗  ██╔████╔██║██████╔╝█████╗  ██████╔╝███████╗
   ╚════██║██║  ██║██║     ██╔══██║██╔══██╗██║  ██║    ╚════╝    ██║╚██╔╝██║██╔══╝  ██║╚██╔╝██║██╔══██╗██╔══╝  ██╔══██╗╚════██║
   ███████║██████╔╝╚██████╗██║  ██║██║  ██║██████╔╝              ██║ ╚═╝ ██║███████╗██║ ╚═╝ ██║██████╔╝███████╗██║  ██║███████║
   ╚══════╝╚═════╝  ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═════╝               ╚═╝     ╚═╝╚══════╝╚═╝     ╚═╝╚═════╝ ╚══════╝╚═╝  ╚═╝╚══════╝
                                                                                                                               
**********************/
#include <Arduino.h>
#include <common.h>
#include <SD.h>
File myFile;
const int SelectSD = 4; // pinnenummer brukt for CS til SD-kortet

/*********************
      ██████╗ ███████╗██████╗ ██╗  ██╗              ███╗   ███╗███████╗███╗   ███╗██████╗ ███████╗██████╗ ███████╗
      ██╔══██╗██╔════╝╚════██╗██║  ██║              ████╗ ████║██╔════╝████╗ ████║██╔══██╗██╔════╝██╔══██╗██╔════╝
      ██████╔╝█████╗   █████╔╝███████║    █████╗    ██╔████╔██║█████╗  ██╔████╔██║██████╔╝█████╗  ██████╔╝███████╗
      ██╔══██╗██╔══╝  ██╔═══╝ ╚════██║    ╚════╝    ██║╚██╔╝██║██╔══╝  ██║╚██╔╝██║██╔══██╗██╔══╝  ██╔══██╗╚════██║
      ██║  ██║██║     ███████╗     ██║              ██║ ╚═╝ ██║███████╗██║ ╚═╝ ██║██████╔╝███████╗██║  ██║███████║
      ╚═╝  ╚═╝╚═╝     ╚══════╝     ╚═╝              ╚═╝     ╚═╝╚══════╝╚═╝     ╚═╝╚═════╝ ╚══════╝╚═╝  ╚═╝╚══════╝
                                                                                                                  
**********************/

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

//CE-9/CSN-10 - Currently usin 8 PIN instead off 9 which is the default CE - no config needed
#define Radio_CSN 6

/** JUNK  *****
  // Structure of our payload
  //struct payload_t
  //{
  //  unsigned long main_command;//1 ac
  //  unsigned long sub_command;//1=on, 0=off
  //};
  //unsigned long main_command;
  //unsigned long sub_command;
  //const byte payloadSize = sizeof(unsigned long);//(sizeof(unsigned long)+sizeof(unsigned long));  // max er 32 bytes i hver pakke.

  //union
  //{
  //  byte ch[payloadSize];
  //  long tid;
  //} rx_buffer ;

  // Structure of our payload
  //struct payload_t
  //{
  //   uint8_t main_command;//1 ac
  //   uint8_t sub_command;//1=on, 0=off
  //};

******* end of junk ********/

/*********************
   ██╗██████╗               ███╗   ███╗███████╗███╗   ███╗██████╗ ███████╗██████╗ ███████╗
   ██║██╔══██╗              ████╗ ████║██╔════╝████╗ ████║██╔══██╗██╔════╝██╔══██╗██╔════╝
   ██║██████╔╝    █████╗    ██╔████╔██║█████╗  ██╔████╔██║██████╔╝█████╗  ██████╔╝███████╗
   ██║██╔══██╗    ╚════╝    ██║╚██╔╝██║██╔══╝  ██║╚██╔╝██║██╔══██╗██╔══╝  ██╔══██╗╚════██║
   ██║██║  ██║              ██║ ╚═╝ ██║███████╗██║ ╚═╝ ██║██████╔╝███████╗██║  ██║███████║
   ╚═╝╚═╝  ╚═╝              ╚═╝     ╚═╝╚══════╝╚═╝     ╚═╝╚═════╝ ╚══════╝╚═╝  ╚═╝╚══════╝
                                                                                          
**********************/

/*****
   We need to use the 'raw' pin reading methods
   because timing is very important here and the digitalRead()
   procedure is slower!
   Note: Digital pin #2 is the same as Pin D2 see http://arduino.cc/en/Hacking/PinMapping168 for the 'raw' pin mapping
*************************/
//uint8_t IRpin = 2;
#define IRpin_PIN PIND
#define IRpin 2

// the maximum pulse we'll listen for - 65 milliseconds is a long time
#define MAXPULSE 5000

/*********
   what our timing resolution should be, larger is better
   as its more 'precise' - but too large and you wont get
   accurate timing
************************/
#define RESOLUTION 18 //20;

//char fileName[8];
// we will store up to 100 pulse pairs (this is -a lot-)
//uint8_t pulses[120][2];  // pair is high and low pulse - need to optimize
uint8_t pulses[120][2];   // pair is high and low pulse - need to optimize
uint8_t currentpulse = 0; // index for pulses we're storing
// int IRledPin =  13;    // LED connected to digital pin 13
#define IRledPin 7 // OUT LED connected to digital pin 7 - OUT PIN
#define RecLedPin 5
#define SendLedPin 10

bool coderecorded = true;
int counter = 0; //used for logic

#define RF_SWITCH_LINE1 3
#define SD_SWITCH_LINE1 9

void updateRecLed(void)
{
  if (!coderecorded)
  {
    digitalWrite(RecLedPin, HIGH);
  }
  else
  {
    digitalWrite(RecLedPin, LOW);
  }
}

void updateSendLed(bool sending)
{
  if (sending)
  {
    digitalWrite(SendLedPin, HIGH);
  }
  else
  {
    digitalWrite(SendLedPin, LOW);
  }
}

void reportLedCriticalError()
{
  while (1)
  {
    digitalWrite(SendLedPin, HIGH);
    delay(500);
    digitalWrite(SendLedPin, LOW);
    delay(500);
  }
}

void blinkReady()
{
  digitalWrite(RecLedPin, HIGH);
  delay(50);
  digitalWrite(RecLedPin, LOW);
  delay(50);
}

void enableSD(void)
{
  digitalWrite(SD_SWITCH_LINE1, HIGH);
  digitalWrite(RF_SWITCH_LINE1, LOW);
  digitalWrite(Radio_CSN, HIGH); //Disable radio
  digitalWrite(SelectSD, LOW);   //Enable sd-card
  delay(10);
}

void enableRF(void)
{
  digitalWrite(SD_SWITCH_LINE1, LOW);
  digitalWrite(RF_SWITCH_LINE1, HIGH);
  digitalWrite(SelectSD, HIGH); //Disable sd-card
  digitalWrite(Radio_CSN, LOW); //Enable radio
  delay(10);
}

void disableRF(void)
{
  //digitalWrite(RELAY, HIGH);
  //digitalWrite(REALY2, HIGH);
  digitalWrite(RF_SWITCH_LINE1, LOW);
  digitalWrite(Radio_CSN, HIGH); //Disable radio
  delay(10);
}

void disableSD(void)
{
  delay(10);
  digitalWrite(SD_SWITCH_LINE1, LOW);
  digitalWrite(SelectSD, HIGH); //Disable sd-card
}

void disableAllSpiDevices(void)
{
  delay(10);
  digitalWrite(RF_SWITCH_LINE1, LOW);
  digitalWrite(SD_SWITCH_LINE1, LOW);
  digitalWrite(Radio_CSN, HIGH); //Disable radio
  digitalWrite(SelectSD, HIGH);  //Disable sd-card
}

char fileName[8];
void commandToFileName(char *f, char *c)
{
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
  Serial.println(f);
}

/*********
   ███╗   ███╗ █████╗ ██╗███╗   ██╗     █████╗ ██████╗ ██████╗ ██╗   ██╗██╗███╗   ██╗ ██████╗     ███████╗███████╗████████╗██╗   ██╗██████╗ 
   ████╗ ████║██╔══██╗██║████╗  ██║    ██╔══██╗██╔══██╗██╔══██╗██║   ██║██║████╗  ██║██╔═══██╗    ██╔════╝██╔════╝╚══██╔══╝██║   ██║██╔══██╗
   ██╔████╔██║███████║██║██╔██╗ ██║    ███████║██████╔╝██║  ██║██║   ██║██║██╔██╗ ██║██║   ██║    ███████╗█████╗     ██║   ██║   ██║██████╔╝
   ██║╚██╔╝██║██╔══██║██║██║╚██╗██║    ██╔══██║██╔══██╗██║  ██║██║   ██║██║██║╚██╗██║██║   ██║    ╚════██║██╔══╝     ██║   ██║   ██║██╔═══╝ 
   ██║ ╚═╝ ██║██║  ██║██║██║ ╚████║    ██║  ██║██║  ██║██████╔╝╚██████╔╝██║██║ ╚████║╚██████╔╝    ███████║███████╗   ██║   ╚██████╔╝██║     
   ╚═╝     ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝    ╚═╝  ╚═╝╚═╝  ╚═╝╚═════╝  ╚═════╝ ╚═╝╚═╝  ╚═══╝ ╚═════╝     ╚══════╝╚══════╝   ╚═╝    ╚═════╝ ╚═╝     
                                                                                                                                            
**********************/
void setup(void)
{

  watchdogReset();
  configureEEPROMAddressForRFAndOTA("004");

  pinMode(RF_SWITCH_LINE1, OUTPUT);
  pinMode(SD_SWITCH_LINE1, OUTPUT);

  pinMode(IRledPin, OUTPUT);
  pinMode(SelectSD, OUTPUT);
  pinMode(Radio_CSN, OUTPUT);
  pinMode(RecLedPin, OUTPUT);
  pinMode(SendLedPin, OUTPUT);

  watchdogReset();
  Serial.begin(9600);

  //setup SPI devices
  SPI.begin();

  //SPI - rf24 radio
  Serial.println(F("RF INIT"));

  watchdogReset();
  enableRF();
  set_Radio_CSN(Radio_CSN);
  startRF();
  // Mirf.csnPin = Radio_CSN;
  // Mirf.spi = &MirfHardwareSpi;
  // Mirf.init();
  // Mirf.setRADDR((byte *)"irir1");
  // Mirf.payload = sizeof(unsigned long);
  // Mirf.config();

  Serial.println(F("Done RF INIT"));
  disableRF();
  watchdogReset();
  //SPI SDCARD
  Serial.println("SD INIT");

  enableSD();
  delay(500);
  if (!SD.begin(SelectSD))
  {
    Serial.println(F("initialization failed!"));
    reportLedCriticalError();
  }
  else
  {
    Serial.println(F("initialization ok."));
  }
  delay(1000);
  // if (!SD.exists("ac-salon"))
  // {
  //   SD.mkdir("ac-salon");
  // }

  coderecorded = true;
  watchdogReset();
  if (coderecorded)
  {
    Serial.println(F("Play mode."));
  }
  else
  {
    Serial.println(F("Record mode."));
  }

  watchdogReset();
  updateRecLed();
  disableSD();

  //Init completed
  watchdogReset();
  disableAllSpiDevices();

  watchdogReset();
  digitalWrite(SendLedPin, HIGH);
  delay(1000);
  digitalWrite(SendLedPin, LOW);
  delay(500);
  digitalWrite(SendLedPin, HIGH);
  delay(1000);
  digitalWrite(SendLedPin, LOW);
  watchdogReset();
  Serial.println(F("Ready to decode IR!"));
}

//Test code for 38Khz send signal
// cli(); //Disable interrupts
// while (1)
// {
//   PORTD |= B10000000;
//   //digitalWrite(IRledPin, HIGH); // this takes about 3 microseconds to happen
//   //   delayMicroseconds(8);         // hang out for 10 microseconds
//   delayMicroseconds(14); // hang out for 10 microseconds
//   //digitalWrite(IRledPin, LOW); // this also takes about 3 microseconds
//   PORTD ^= B10000000;
//   //digitalWrite(IRledPin, LOW); // this takes about 3 microseconds to happen
//   //   delayMicroseconds(8);         // hang out for 10 microseconds
//   delayMicroseconds(14); // hang out for 10 microseconds
// }

/**********************
   Main Arduino Loop
**********************/

void loop(void)
{
  if (!coderecorded)
  {
    //Serial.println(F("Recording IR Signal..."));
    recordircode();
    return;
  }

  enableRF();
  Serial.println(F("Waiting for remote command..."));
  while (!Mirf.dataReady())
  {
    //delay(100);
    blinkReady();
  };

  checkIfOtaRequestOrLoadCommand(cmd);
  //Serial.print(F("Got New command: "));
  //Serial.println(cmd);

  if (strcmp(cmd, "000") == 0) //000 is record
  {
    Serial.print(F("Record cmd: waiting for record file name"));
    while (!Mirf.dataReady())
    {
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
  }
  else if (strcmp(cmd, "001") == 0)
  {
    Serial.println(F("Restarting..."));
    soft_restart();
    return;
  }
  else
  {
    disableRF();

    //Serial.print("Got IR cmd: ");
    //Serial.println(cmd);

    enableSD();

    if (!SD.begin(SelectSD))
    {
      //Serial.println(F("move to SD ERROR")); //dbg
    }
    commandToFileName(fileName, cmd);

    myFile = SD.open(fileName);
    if (myFile)
    {
      //Serial.println(F("File open ok \r\n"));
      sendcode();
    }
    else
    {
      //wait and try again
      delay(100);
      myFile = SD.open(fileName);
      if (myFile)
      {
        sendcode();
      }
      else
      {
        Serial.println(F("File open Error file \n\r\n"));
        reportLedCriticalError();
      }
    }
  }

  disableAllSpiDevices();

  //junk/tests
  //  for(int i = 0; i < 4; i++) {
  //    main_command = (main_command << 8) | rx_buffer.ch[i];
  //  }
  //  for(int i = 0; i < 4; i++) {
  //    sub_command = (sub_command << 8) | rx_buffer.ch[i];
  //  }
  //  Serial.println("main_command");
  //  Serial.println(main_command);
  //  Serial.println("sub_command");
  //  Serial.println(sub_command);
  //Serial.println(((payload_t)rx_buffer.ch).sub_command
  //Serial.println(rx_buffer.tid);

} //end of loop

String buffer;

/*********
   Load IR command from SD and send it via IR
**********************/
void sendcode()
{
  updateSendLed(true);
  enableSD();

  //readStringUntil reference: http://stackoverflow.com/questions/35486045/arduino-reading-sd-file-line-by-line-c
  buffer = myFile.readStringUntil('\n');
  //to int reference: https://www.arduino.cc/en/Tutorial/StringToIntExample
  counter = buffer.toInt();
  //Serial.print(counter);//dbg
  //Serial.println(" number of line in file header");//dbg
  for (uint8_t i = 0; i < counter; i++)
  {
    buffer = myFile.readStringUntil('\n');
    //buffer.toInt() * RESOLUTION
    pulses[i][0] = buffer.toInt();
    buffer = myFile.readStringUntil('\n');
    pulses[i][1] = buffer.toInt();

    // Serial.print("  0  -  ");//dbg
    // Serial.print(pulses[i][0]* RESOLUTION, DEC);//dbg
    // Serial.print("  1  -  ");//dbg
    // Serial.print(pulses[i][1]* RESOLUTION, DEC);//dbg
    // Serial.println("");//dbg
  }
  //Serial.println("will close file");//dbg
  myFile.close();
  //Serial.println("done loading command from file");//dbg

  updateSendLed(false);
  delay(1000);
  updateSendLed(true);

  pulseIR(9000);
  delayMicroseconds(4500);
  cli(); //Disable interrupts
  for (uint8_t i = 0; i < counter; i++)
  {
    unsigned long t = (unsigned long)(pulses[i][0]) * RESOLUTION;
    if (t > 32000)
    {
      delay(t / 1000);
    }
    else
    {
      delayMicroseconds(t - RESOLUTION);
    }
    pulseIR((((unsigned long)pulses[i][1]) * RESOLUTION));
  }
  sei(); //Enables interrupts
  updateSendLed(false);
} //end of load and send command

/*********
      pulseIR
      This procedure sends a 38KHz pulse to the IRledPin
      for a certain # of microseconds. We'll use this whenever we need to send codes
 ********************/
void pulseIR(long microsecs)
{
  /***********
     we'll count down from the number of microseconds we are told to wait
  *************/

  // Serial.println("pulseIR microsecs");
  //   Serial.println(microsecs);

  while (microsecs > 0)
  {

    //   PORTD |= B10000000;
    //   //digitalWrite(IRledPin, HIGH); // this takes about 3 microseconds to happen
    //   //   delayMicroseconds(8);         // hang out for 10 microseconds
    //   delayMicroseconds(14); // hang out for 10 microseconds
    //   //digitalWrite(IRledPin, LOW); // this also takes about 3 microseconds
    //   PORTD ^= B10000000;
    //   //digitalWrite(IRledPin, LOW); // this takes about 3 microseconds to happen
    //   //   delayMicroseconds(8);         // hang out for 10 microseconds
    //   delayMicroseconds(14); // hang out for 10 microseconds
    // 38 kHz is about 13 microseconds high and 13 microseconds low
    PORTD |= B10000000; // this takes about 3 microseconds to happen
    //PORTD |= B10000000;
    //   delayMicroseconds(8);         // hang out for 10 microseconds
    delayMicroseconds(14); // hang out for 10 microseconds
    PORTD ^= B10000000;    // this also takes about 3 microseconds
    //PORTD ^= B00000000;
    //   delayMicroseconds(8);         // hang out for 10 microseconds
    delayMicroseconds(14); // hang out for 10 microseconds

    // so 26 microseconds altogether
    microsecs -= 26;
  }
}

uint16_t highpulse, lowpulse; // temporary storage timing

void recordircode()
{
  highpulse = lowpulse = 0; // start out with no pulse length

  //  while (digitalRead(IRpin)) { // this is too slow!
  while (IRpin_PIN & (1 << IRpin))
  {
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
    if ((highpulse >= MAXPULSE) && (currentpulse != 0))
    {
      Serial.println(F("break highpulse....")); //dbg
      Serial.println(highpulse);                //dbg
      printpulses();
      currentpulse = 0;
      return;
    }
  }
  /********************************
    // we didn't time out so lets stash the reading
  *********************************/
  pulses[currentpulse][0] = highpulse;

  // same as above
  while (!(IRpin_PIN & _BV(IRpin)))
  {
    // pin is still LOW
    lowpulse++;
    delayMicroseconds(RESOLUTION);
    if ((lowpulse >= MAXPULSE) && (currentpulse != 0))
    {
      Serial.println(F("break lowpulse....")); //dbg
      Serial.println(lowpulse);                //dbg
      printpulses();
      currentpulse = 0;
      return;
    }
  }
  pulses[currentpulse][1] = lowpulse;

  // we read one high-low pulse successfully, continue!
  currentpulse++;
  //Serial.println(currentpulse);
}

void printpulses(void)
{
  coderecorded = true;
  updateRecLed();
  counter = currentpulse;
  Serial.print(F("Command Data Size: ")); //dbg
  Serial.println(counter);                //dbg

  //Serial.println("1 \n\r\n");//dbg
  //TODO: Which command to save to which file
  disableRF();
  enableSD();

  if (!SD.begin(SelectSD))
  {
    Serial.println(F("move to SD ERROR")); //dbg
  }

  Serial.print(F("Saving data to file name: "));
  Serial.println(fileName);

  if (SD.exists(fileName))
  {
    Serial.print(F("found file deleting first"));
    SD.remove(fileName);
  }

  delay(100);

  myFile = SD.open(fileName, FILE_WRITE);
  //Serial.println("sd open ok \r\n"); //dbg
  if (myFile)
  {
    myFile.println(counter);
    Serial.println(F("writing command size to file ok \r\n")); //dbg
  }
  else
  {
    Serial.println(F("Error open file \n\r\n"));
    reportLedCriticalError();
  }

  //Serial.println("\n\r\n\rReceived: \n\rOFF \tON");//dbg
  for (uint8_t i = 0; i < currentpulse; i++)
  {
    Serial.print(pulses[i][0] * RESOLUTION, DEC); //dbg
    // if the file opened okay, write to it:
    if (myFile)
    {
      myFile.println(pulses[i][0]);
    }
    else
    {
      Serial.println(F("File not open \r\n")); //dbg
    }
    Serial.print(" usec, ");                      //dbg
    Serial.print(pulses[i][1] * RESOLUTION, DEC); //dbg
    if (myFile)
    {
      myFile.println(pulses[i][1]);
    }
    Serial.println(F(" usec")); //dbg
  }
  myFile.close();
  Serial.println(F("Close file ok \r\n")); //dbg
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

// if (1)
// {
//   Serial.println(F("Got AC Power off command - Sending...."));
//   disableRF();
//   enableSD();

//   if (!SD.begin(SelectSD))
//   {
//     Serial.println(F("move to SD ERROR")); //dbg
//   }

//   myFile = SD.open("off.txt");
//   if (myFile)
//   {
//     Serial.println(F("File open ok \r\n"));
//     sendcode();
//   }
//   else
//   {
//     Serial.println(F("File open Error file \n\r\n"));
//   }
//   delay(500);
//   return;
// }

// byte data[Mirf.payload] = {0};
// while (!Mirf.dataReady())
// {
// };                  // wait for data to arrive
// Mirf.getData(data); // read to buffer
// Serial.println(F("Network data"));
// Serial.println(data[0]);
// Serial.println(data[1]);
