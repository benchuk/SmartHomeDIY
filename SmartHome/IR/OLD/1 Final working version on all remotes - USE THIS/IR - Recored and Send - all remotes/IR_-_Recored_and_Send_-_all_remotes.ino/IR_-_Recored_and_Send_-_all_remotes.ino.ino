

//USE ME 19.9.2016
//http://www.instructables.com/id/Clone-a-Remote-with-Arduino/
///https://www.arduino.cc/en/Tutorial/ReadWrite
/*
  SD card read/write

 This example shows how to read and write data to and from an SD card file
 The circuit:
 * SD card attached to SPI bus as follows:
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

/* Raw IR decoder sketch!

 This sketch/program uses the Arduno and a PNA4602 to
 decode IR received. This can be used to make a IR receiver
 (by looking for a particular code)
 or transmitter (by pulsing an IR LED at ~38KHz for the
 durations detected

 Code is public domain, check out www.ladyada.net and adafruit.com
 for more tutorials!
 */
//#include <RF24Network.h>
//#include <RF24.h>
#include <Mirf.h>
#include <MirfHardwareSpiDriver.h>
#include <MirfSpiDriver.h>
#include <nRF24L01.h>

#include <SPI.h>
#include <SD.h>

// nRF24L01(+) radio attached using Getting Started board
//RF24 radio(9, 10);//CE-9/CSN-10
const int SelectSD  = 4;  // pinnenummer brukt for CS til SD-kortet
const int Radio_CSN = 10; // pinnenummer brukt for CSN på radio (ikke endre denne)
const int Radio_CE  = 9;  // 

// Network uses that radio
//RF24Network network(radio);

// Structure of our payload
//struct payload_t
//{
//  unsigned long main_command;//1 ac
//  unsigned long sub_command;//1=on, 0=off
//};
unsigned long main_command;
unsigned long sub_command;
const byte payloadSize = (sizeof(unsigned long)+sizeof(unsigned long));  // max er 32 bytes i hver pakke. 

union
{
  byte ch[payloadSize];
  long tid;
} rx_buffer ; 

// Address of our node
//const uint16_t this_node_ir_station_1 = 1;
//const uint16_t master_node = 0;

// Structure of our payload
//struct payload_t
//{
//   uint8_t main_command;//1 ac
//   uint8_t sub_command;//1=on, 0=off
//};

// We need to use the 'raw' pin reading methods
// because timing is very important here and the digitalRead()
// procedure is slower!
//uint8_t IRpin = 2;
// Digital pin #2 is the same as Pin D2 see
// http://arduino.cc/en/Hacking/PinMapping168 for the 'raw' pin mapping
#define IRpin_PIN      PIND
#define IRpin          2

// the maximum pulse we'll listen for - 65 milliseconds is a long time
#define MAXPULSE 3800

// what our timing resolution should be, larger is better
// as its more 'precise' - but too large and you wont get
// accurate timing
unsigned long RESOLUTION = 20;
File myFile;

// we will store up to 100 pulse pairs (this is -a lot-)
uint16_t pulses[50][2];  // pair is high and low pulse
uint8_t currentpulse = 0; // index for pulses we're storing

bool coderecorded = false;
// int IRledPin =  13;    // LED connected to digital pin 13
int IRledPin =  7;    // LED connected to digital pin 13
int counter = 0;

void setup(void) {
  //pinMode(8, OUTPUT);
  //  digitalWrite(8, HIGH);
  pinMode(IRledPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("Ready to decode IR!");
  SPI.begin();
  //radio.begin();
  //network.begin(/*channel*/ 90, /*node address*/ this_node_ir_station_1);

  if (!SD.begin(SelectSD)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  if (! SD.exists("ac-salon")) {
    SD.mkdir("ac-salon");
  }
  digitalWrite(SelectSD,HIGH); // disable sd-kortet
  Mirf.csnPin =  Radio_CSN;                                                                                                                                                          ; // kan endres etter behov. Kan utelates hvis default
  Mirf.cePin  = Radio_CE; 
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.payload = payloadSize;
  Mirf.channel = 90; // any channel:  1..125 (avtales med den som sender)
  //Mirf.config();
  Mirf.setRADDR((byte *)"irir1"); // sett inn 'receive'-navn - alltid 5 tegn!
  Mirf.config();
  digitalWrite(Radio_CSN,HIGH); // disable radio
  digitalWrite(SelectSD,HIGH); // disable sd-kortet
}

void loop(void) {
  
  // Pump the network regularly
  //SD.begin(10);//SPI: move to network
  //network.update();
  digitalWrite(Radio_CSN,LOW); // enable radio
  while(!Mirf.dataReady()) {};  // wait for data to arrive
  Mirf.getData(rx_buffer.ch);  // read to buffer
  Serial.println("Network data");
  Serial.println(rx_buffer.ch[0]);
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
  Serial.println(rx_buffer.tid); 
  digitalWrite(Radio_CSN,HIGH); // disable radio
  digitalWrite(SelectSD,LOW); // enable sd-kortet
  
//  while ( network.available() )
//  {
//    // If so, grab it and print it out
//    RF24NetworkHeader header;
//    payload_t payload;
//    network.read(header, &payload, sizeof(payload));
//    Serial.print("Dispatch station Received ack/status packet #");
//    Serial.print(payload.main_command);
//    Serial.print(" at ");
//    Serial.println(payload.sub_command);
//  }
  
  SD.begin(4);//SPI: go back to sd card
  
//  if (!coderecorded)
//  {
//    Serial.println("Wiating for IR code");
//    recordircode();
//  }
//  else
//  {
//    Serial.println("Recorded ok - Sending....");
//    sendcode();
//    //    delay(68);
//    //    sendcode();
//    //    delay(68);
//    //    sendcode();
//    delay(1500);
//  }


}
String buffer;

//ohter sd card interesting stuff: https://www.arduino.cc/en/Tutorial/DumpFile
//http://forum.arduino.cc/index.php?topic=104109.0
void sendcode()
{
  myFile = SD.open("/ac-salon/power-on.txt");

  //readStringUntil reference: http://stackoverflow.com/questions/35486045/arduino-reading-sd-file-line-by-line-c
  buffer = myFile.readStringUntil('\n');
  //to int reference: https://www.arduino.cc/en/Tutorial/StringToIntExample
  counter = buffer.toInt();
  Serial.println(counter);
  Serial.println("from file");
  for (uint8_t i = 0; i < counter; i++) {
    buffer = myFile.readStringUntil('\n');
    //buffer.toInt() * RESOLUTION
    pulses[i][0] = buffer.toInt();
    buffer = myFile.readStringUntil('\n');
    pulses[i][1] = buffer.toInt();

    Serial.print("  0  -  ");
    Serial.print(pulses[i][0]* RESOLUTION, DEC);
    Serial.print("  1  -  ");
    Serial.print(pulses[i][1]* RESOLUTION, DEC);
    Serial.println("");
  }
  Serial.println("will close file");

  myFile.close();

  Serial.println("done loading");
  //cli(); // this turns off any background interrupts
  for (uint8_t i = 0; i < counter; i++) {
    //Serial.println("s1");
    unsigned long t = (unsigned long)(pulses[i][0]) * RESOLUTION;
    // Serial.println("delayMicroseconds-> (pulses[i][0]) * RESOLUTION");
    //   Serial.println(t);
    if (t > 32000)
    {
      delay(t / 1000);
    }
    else
    {
      delayMicroseconds(t - RESOLUTION);
    }
    pulseIR((((unsigned long)pulses[i][1]) * RESOLUTION));

    //      if(i <= counter-2)
    //      {
    //       pulseIR((pulses[i][1] * RESOLUTION)-10);
    //       delayMicroseconds((pulses[i+1][0] * RESOLUTION)-10);
    //      }
    //      else
    //      {
    //         delayMicroseconds((pulses[i][0] * RESOLUTION)-10);
    //         pulseIR((pulses[i][1] * RESOLUTION)-10);
    //      }
  }
  //    delayMicroseconds((pulses[0][0] * RESOLUTION));
  //sei();  // this turns them back on
}

// This procedure sends a 38KHz pulse to the IRledPin
// for a certain # of microseconds. We'll use this whenever we need to send codes
void pulseIR(long microsecs) {
  // we'll count down from the number of microseconds we are told to wait

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

void recordircode() {
  //  long test;
  //  test = 3500L*20L;
  //  Serial.println("test max....");
  //  Serial.println(test);

  uint16_t highpulse, lowpulse;  // temporary storage timing
  highpulse = lowpulse = 0; // start out with no pulse length


  //  while (digitalRead(IRpin)) { // this is too slow!
  while (IRpin_PIN & (1 << IRpin)) {
    // pin is still HIGH

    // count off another few microseconds
    highpulse++;
    delayMicroseconds(RESOLUTION);

    // If the pulse is too long, we 'timed out' - either nothing
    // was received or the code is finished, so print what
    // we've grabbed so far, and then reset
    if ((highpulse >= MAXPULSE) && (currentpulse != 0)) {
      Serial.println("break highpulse....");
      Serial.println(highpulse);

      printpulses();
      currentpulse = 0;
      return;
    }
  }
  // we didn't time out so lets stash the reading
  pulses[currentpulse][0] = highpulse;

  // same as above
  while (! (IRpin_PIN & _BV(IRpin))) {
    // pin is still LOW
    lowpulse++;
    delayMicroseconds(RESOLUTION);
    if ((lowpulse >= MAXPULSE)  && (currentpulse != 0)) {
      Serial.println("break lowpulse....");
      Serial.println(lowpulse);
      printpulses();
      currentpulse = 0;
      return;
    }
  }
  pulses[currentpulse][1] = lowpulse;

  // we read one high-low pulse successfully, continue!
  currentpulse++;
}

void printpulses(void) {
  coderecorded = true;
  counter = currentpulse;
  Serial.print("Data Size: ");
  Serial.println(counter);

  Serial.println("1 \n\r\n");
  SD.remove("/ac-salon/power-on.txt");
  myFile = SD.open("/ac-salon/power-on.txt", FILE_WRITE);
  Serial.println("2 \n\r\n");
  if (myFile) {
    myFile.println(counter);
  }
  else
  {
    Serial.println("Error open file \n\r\n");
  }

  Serial.println("\n\r\n\rReceived: \n\rOFF \tON");
  for (uint8_t i = 0; i < currentpulse; i++) {
    Serial.print(pulses[i][0] * RESOLUTION, DEC);
    // if the file opened okay, write to it:
    if (myFile) {
      myFile.println(pulses[i][0]);
    }
    Serial.print(" usec, ");
    Serial.print(pulses[i][1] * RESOLUTION, DEC);
    if (myFile) {
      myFile.println(pulses[i][1]);
    }
    Serial.println(" usec");
  }
  myFile.close();
  // print it in a 'array' format
  Serial.println("int IRsignal[] = {");
  Serial.println("// ON, OFF (in 10's of microseconds)");
  for (uint8_t i = 0; i < currentpulse - 1; i++) {
    Serial.print("\t"); // tab
    Serial.print(pulses[i][1] * RESOLUTION / 10, DEC);
    Serial.print(", ");
    Serial.print(pulses[i + 1][0] * RESOLUTION / 10, DEC);
    Serial.println(",");
  }
  Serial.print("\t"); // tab
  Serial.print(pulses[currentpulse - 1][1] * RESOLUTION / 10, DEC);
  Serial.print(", 0};");
}
