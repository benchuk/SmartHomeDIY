#include <Arduino.h>
#include <PinChangeInt.h>
#include <common.h>

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

#define rfCE 9
#define rfCS 10
#define rfMISO 12
#define rfMOSI 11
#define rfCLK 13

//pin change interrupts groups are:
//A0 to A5
//D0 to D7
//D8 to D13
//So using 2 groups pin 7 on one group and pin A3 on the other group
//This way I do not have to ready pin state to make sure which one was changed.
#define SWITCH3_PIN_CAHNGE_INTERRUPT 8
#define SWITCH4_PIN_CAHNGE_INTERRUPT A3

//out to switch relay
#define light1Pin 4
#define light2Pin 5
#define light3Pin 6
#define light4Pin 7

//interrupts from touch button
#define interruptTouch1Pin 2
#define interruptTouch2Pin 3

//swithc state for lights
volatile byte s1 = HIGH;
volatile byte s2 = HIGH;
volatile byte s3 = HIGH;
volatile byte s4 = HIGH;
//only needed for pin change swithces - cannot set rais or fall only change
volatile int s3Counter = 0;
volatile int s4Counter = 0;

void touch3()
{
  s3Counter++;
  if (s3Counter == 1)
  {
    s3 = !s3;
    //digitalWrite(light2Pin, s3);
    Serial.println("TOUCH3");
  }
  if(s3Counter == 2)
  {
    s3Counter = 0;
  }
}

void touch4()
{
  s4Counter++;
  if (s4Counter == 1)
  {
    s4 = !s4;
    //digitalWrite(light2Pin, s4);
    Serial.println("TOUCH4");
  }
  if(s4Counter == 2)
  {
    s4Counter = 0;
  }
}

void setup()
{
  set_Radio_CE(rfCE);
  set_Radio_CSN(rfCS);

  configureEEPROMAddressForRFAndOTA("005");

  Serial.begin(9600);

  Serial.println("INIT PINS and INTERRUPTS OK");
  pinMode(light1Pin, OUTPUT);
  pinMode(light2Pin, OUTPUT);
  pinMode(light3Pin, OUTPUT);
  pinMode(light4Pin, OUTPUT);

  digitalWrite(light1Pin, s1);
  digitalWrite(light2Pin, s2);
  digitalWrite(light3Pin, s3);
  digitalWrite(light4Pin, s4);

  pinMode(interruptTouch1Pin, INPUT_PULLUP);
  pinMode(interruptTouch2Pin, INPUT_PULLUP);
  pinMode(SWITCH3_PIN_CAHNGE_INTERRUPT, INPUT);
  pinMode(SWITCH4_PIN_CAHNGE_INTERRUPT, INPUT);

  attachInterrupt(digitalPinToInterrupt(interruptTouch1Pin), touch1, RISING);
  attachInterrupt(digitalPinToInterrupt(interruptTouch2Pin), touch2, RISING);
  attachPinChangeInterrupt(SWITCH3_PIN_CAHNGE_INTERRUPT, touch3, CHANGE);
  attachPinChangeInterrupt(SWITCH4_PIN_CAHNGE_INTERRUPT, touch4, CHANGE);

  startRF();

  Serial.println("INIT OK");
}

void loop()
{
  //  watchdogReset();
  Serial.print("TICK");
  //  delay(100);
  // return;
  while (!Mirf.dataReady())
  {
    //Serial.print(".");
    watchdogReset();
    delay(100);
  };

  Serial.print("TICK");
  checkIfOtaRequestOrLoadCommand(cmd);
  Serial.print(F("New command "));
  Serial.println(cmd);
  if (strcmp(cmd, "001") == 0)
  {
    Serial.print("remtoe restart");
    soft_restart();
    return;
  }
  //s1
  else if (strcmp(cmd, "002") == 0)
  {
    Serial.print("s1 off");
    s1 = LOW;
    return;
  }
  else if (strcmp(cmd, "003") == 0)
  {
    Serial.print("s1 on");
    s1 = HIGH;
    return;
  }
  else if (strcmp(cmd, "004") == 0)
  {
    Serial.print("toogle s1");
    s1 = !s1;
    digitalWrite(light1Pin, s1);
    return;
  }
  //s2
  else if (strcmp(cmd, "005") == 0)
  {
    Serial.print("s2 off");
    s2 = LOW;
    return;
  }
  else if (strcmp(cmd, "006") == 0)
  {
    Serial.print("s2 on");
    s2 = HIGH;
    return;
  }
  else if (strcmp(cmd, "007") == 0)
  {
    Serial.print("toogle s2");
    s2 = !s2;
    return;
  }
}

void touch1()
{
  s1 = !s1;
  digitalWrite(light1Pin, s1);
  Serial.println("TOUCH1");
}

void touch2()
{
  s2 = !s2;
  digitalWrite(light2Pin, s2);
  Serial.println("TOUCH2");
}