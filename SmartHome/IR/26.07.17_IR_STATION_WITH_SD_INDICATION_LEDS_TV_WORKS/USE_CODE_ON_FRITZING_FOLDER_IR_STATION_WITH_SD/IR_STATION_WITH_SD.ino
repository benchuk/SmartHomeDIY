
/*********************
   Version: 1.10.2016 - Version 1
**********************/

/*********************
   Version: Features
 * *******************
   SPI init for IR and SDCARD on same BUS
   Record IR command to SDCARD
   Wait for dispatcher station to get command from PI server and send it here
   Send this command by loading from SDCARD to mem and then send via IR
**********************/

/************************
   Important References
 * **********************
   http://www.instructables.com/id/Clone-a-Remote-with-Arduino/
   https://www.arduino.cc/en/Tutorial/ReadWrite
   check this for sd adn rf on the same spi
   https://forum.arduino.cc/index.php?topic=360718.0
   ohter sd card interesting stuff: https://www.arduino.cc/en/Tutorial/DumpFile
   http://forum.arduino.cc/index.php?topic=104109.0

**********************/


/*********************
   Credits
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
   SDCARD - Members
**********************/

#include <SD.h>
File myFile;
const int SelectSD  = 4;  // pinnenummer brukt for CS til SD-kortet


/*********************
   RF24 - Members
**********************/
//#include <RF24Network.h>
//#include <RF24.h>
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

//CE-9/CSN-10 - Currently usin 8 PIN instead on 9 which is the default CE - no config needed
const int Radio_CSN = 6; 

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
   IR - Members
**********************/

/*****
   We need to use the 'raw' pin reading methods
   because timing is very important here and the digitalRead()
   procedure is slower!
   Note: Digital pin #2 is the same as Pin D2 see http://arduino.cc/en/Hacking/PinMapping168 for the 'raw' pin mapping
*************************/
//uint8_t IRpin = 2;
#define IRpin_PIN      PIND
#define IRpin          2

// the maximum pulse we'll listen for - 65 milliseconds is a long time
#define MAXPULSE 3800

/*********
   what our timing resolution should be, larger is better
   as its more 'precise' - but too large and you wont get
   accurate timing
************************/
unsigned long RESOLUTION = 18;//20;

// we will store up to 100 pulse pairs (this is -a lot-)
//uint8_t pulses[120][2];  // pair is high and low pulse - need to optimize
uint8_t pulses[120][2];  // pair is high and low pulse - need to optimize
uint8_t currentpulse = 0; // index for pulses we're storing
// int IRledPin =  13;    // LED connected to digital pin 13
int IRledPin =  7;    // OUT LED connected to digital pin 7 - OUT PIN
int RecLedPin =  5;
int SendLedPin =  10;

bool coderecorded = true;
int counter = 0; //used for logic


int RF_SWITCH_LINE1 = 3;
int SD_SWITCH_LINE1 = 9;

void updateRecLed(void)
{
  if(!coderecorded)
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
  if(sending)
  {
    digitalWrite(SendLedPin, HIGH);
  }
  else
  {
    digitalWrite(SendLedPin, LOW);
  }
}
/*********
   Main Arduino Setup
**********************/
void setup(void) {

  pinMode(RF_SWITCH_LINE1, OUTPUT);
  pinMode(SD_SWITCH_LINE1, OUTPUT);

  pinMode(IRledPin, OUTPUT);
  pinMode(SelectSD, OUTPUT);
  pinMode(Radio_CSN, OUTPUT);
  pinMode(RecLedPin, OUTPUT);
  pinMode(SendLedPin, OUTPUT);

  Serial.begin(9600);

  //setup SPI devices
  SPI.begin();


  //SPI - rf24 radio
  Serial.println("RF INIT");

  enableRF();

  Mirf.csnPin =  Radio_CSN;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"irir1");
  Mirf.payload = sizeof(unsigned long);
  Mirf.config();

  Serial.println("Done RF INIT");
  disableRF();

  //SPI SDCARD
  Serial.println("SD INIT");

  enableSD();

  if (!SD.begin(SelectSD)) {
    Serial.println("initialization failed!");
    while(1)
    {
       digitalWrite(SendLedPin, HIGH);
       delay(1000);
       digitalWrite(SendLedPin, LOW);
    }
  }
  else
  {
    Serial.println("initialization ok.");
  }
  delay(1000);
  if (!SD.exists("ac-salon"))
  {
    SD.mkdir("ac-salon");
  }

  coderecorded = SD.exists("off.txt");

  updateRecLed();
  disableSD();

  //Init completed
  disableAllSpiDevices();

   digitalWrite(SendLedPin, HIGH);
   delay(1000);
   digitalWrite(SendLedPin, LOW);
   delay(500);
   digitalWrite(SendLedPin, HIGH);
   delay(1000);
   digitalWrite(SendLedPin, LOW);
   
  Serial.println("Ready to decode IR!");
}

void enableSD(void)
{
  digitalWrite(SD_SWITCH_LINE1, HIGH);
  digitalWrite(RF_SWITCH_LINE1, LOW);
  digitalWrite(Radio_CSN, HIGH); //Disable radio
  digitalWrite(SelectSD, LOW); //Enable sd-card
  delay(1000);
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
  delay(1000);
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
  digitalWrite(SelectSD, HIGH); //Disable sd-card
}

/*********
   Main Arduino Loop
**********************/
void loop(void) {

  if (!coderecorded)
  {
    //Serial.println("Waiting for IR code...");
    recordircode();
    return;
  }

  delay(500);

  Serial.println("CHECKING FOR DATA");

  enableRF();

  byte data[Mirf.payload] = {0};
  while (!Mirf.dataReady()) {}; // wait for data to arrive
  Mirf.getData(data);  // read to buffer
  Serial.println("Network data");
  Serial.println(data[0]);
  Serial.println(data[1]);

  disableRF();

  if (data[0] == 1 && data[1] == 1)
  {
    Serial.println("Got AC Power on command - Sending....");

    enableSD();

    if (!SD.begin(SelectSD))
    {
      Serial.println("move to SD ERROR");//dbg
    }
    SD.remove("off.txt");
    coderecorded = false;
    updateRecLed();
    //myFile = SD.open("/ac-salon/power-on.txt");
    //sendcode();
    //    delay(68);
    //    sendcode();
    //    delay(68);
    //    sendcode();
    delay(1500);
  }
  else if (data[0] == 1 && data[1] == 0)
  {
    Serial.println("Got AC Power off command - Sending....");

    enableSD();

    if (!SD.begin(SelectSD))
    {
      Serial.println("move to SD ERROR");//dbg
    }

    myFile = SD.open("off.txt");
    sendcode();
    //delay(1500);
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

}//end of loop


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
  for (uint8_t i = 0; i < counter; i++) {
    buffer = myFile.readStringUntil('\n');
    //buffer.toInt() * RESOLUTION
    pulses[i][0] = buffer.toInt();
    buffer = myFile.readStringUntil('\n');
    pulses[i][1] = buffer.toInt();

    //Serial.print("  0  -  ");//dbg
    //Serial.print(pulses[i][0]* RESOLUTION, DEC);//dbg
    //Serial.print("  1  -  ");//dbg
    //Serial.print(pulses[i][1]* RESOLUTION, DEC);//dbg
    //Serial.println("");//dbg
  }
  //Serial.println("will close file");//dbg
  myFile.close();
  //Serial.println("done loading command from file");//dbg

  updateSendLed(false);
  delay(1000);
  updateSendLed(true);
  
  pulseIR(9000);
  delayMicroseconds(4500);
  
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
  updateSendLed(false);
}//end of load and send command

/*********
      pulseIR
      This procedure sends a 38KHz pulse to the IRledPin
      for a certain # of microseconds. We'll use this whenever we need to send codes
 ********************/
void pulseIR(long microsecs) {
  /***********
     we'll count down from the number of microseconds we are told to wait
  *************/

  // Serial.println("pulseIR microsecs");
  //   Serial.println(microsecs);

  while (microsecs > 0) {
    // 38 kHz is about 13 microseconds high and 13 microseconds low
    digitalWrite(IRledPin, HIGH);  // this takes about 3 microseconds to happen
    //   delayMicroseconds(8);         // hang out for 10 microseconds
    delayMicroseconds(9);         // hang out for 10 microseconds
    digitalWrite(IRledPin, LOW);   // this also takes about 3 microseconds
    //   delayMicroseconds(8);         // hang out for 10 microseconds
    delayMicroseconds(7);         // hang out for 10 microseconds

    // so 26 microseconds altogether
    microsecs -= 26;
  }
}
uint16_t highpulse, lowpulse;  // temporary storage timing
void recordircode()
{
  highpulse = lowpulse = 0; // start out with no pulse length

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
      Serial.println("break highpulse....");//dbg
      Serial.println(highpulse);//dbg
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
  while (! (IRpin_PIN & _BV(IRpin))) {
    // pin is still LOW
    lowpulse++;
    delayMicroseconds(RESOLUTION);
    if ((lowpulse >= MAXPULSE)  && (currentpulse != 0)) {
      Serial.println("break lowpulse....");//dbg
      Serial.println(lowpulse);//dbg
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

void printpulses(void) {
  coderecorded = true;
  updateRecLed();
  counter = currentpulse;
  Serial.print("Command Data Size: ");//dbg
  Serial.println(counter);//dbg

  //Serial.println("1 \n\r\n");//dbg
  //TODO: Which command to save to which file
  enableSD();

  if (!SD.begin(SelectSD))
  {
    Serial.println("move to SD ERROR");//dbg
  }
  //SD.remove("/ac-salon/poweroff.txt");
  SD.remove("off.txt");
  myFile = SD.open("off.txt", FILE_WRITE);
  Serial.println("sd open ok \r\n");//dbg
  if (myFile)
  {
    myFile.println(counter);
    Serial.println("writing command size to file ok \r\n");//dbg
  }
  else
  {
    Serial.println("Error open file \n\r\n");
  }

  //Serial.println("\n\r\n\rReceived: \n\rOFF \tON");//dbg
  for (uint8_t i = 0; i < currentpulse; i++) {
    Serial.print(pulses[i][0] * RESOLUTION, DEC);//dbg
    // if the file opened okay, write to it:
    if (myFile) {
      myFile.println(pulses[i][0]);
    }
    else
    {
      Serial.println("File not open \r\n");//dbg
    }
    Serial.print(" usec, ");//dbg
    Serial.print(pulses[i][1] * RESOLUTION, DEC);//dbg
    if (myFile) {
      myFile.println(pulses[i][1]);
    }
    Serial.println(" usec");//dbg
  }
  myFile.close();
  Serial.println("Close file ok \r\n");//dbg
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
