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

#define rfCLK 13
#define rfMISO 12
#define rfMOSI 11
#define D3_PIN_CAHNGE_INTERRUPT 7

const int rfCS = 10;
const int rfCE = 9;

//out to switch relay
const byte light1Pin = 5;
const byte light2Pin = 6;

//interrupts from touch button
const byte interruptTouch1Pin = 2;
const byte interruptTouch2Pin = 3;

//swithc state for lights
volatile byte s1 = HIGH;
volatile byte s2 = HIGH;
volatile byte s3 = HIGH;
volatile int s3Counter = 0;

void touch3()
{
  s3Counter++;
  if (s3Counter == 2)
  {
    s3 = !s3;
    //digitalWrite(light2Pin, s2);
    Serial.print("TOUCH3");
    s3Counter = 0;
  }
}

void setup()
{
  set_Radio_CE(rfCE);
  set_Radio_CSN(rfCS);

  configureEEPROMAddressForRFAndOTA("005");

  Serial.begin(9600);

  pinMode(light1Pin, OUTPUT);
  pinMode(light2Pin, OUTPUT);

  pinMode(interruptTouch1Pin, INPUT_PULLUP);
  pinMode(interruptTouch2Pin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(interruptTouch1Pin), touch1, RISING);
  attachInterrupt(digitalPinToInterrupt(interruptTouch2Pin), touch2, RISING);
  pinMode(D3_PIN_CAHNGE_INTERRUPT, INPUT);
  attachPinChangeInterrupt(D3_PIN_CAHNGE_INTERRUPT, touch3, CHANGE);

  digitalWrite(light1Pin, s1);
  digitalWrite(light2Pin, s2);

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
    Serial.print(".");
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
  Serial.print("TOUCH1");
}

void touch2()
{
  s2 = !s2;
  digitalWrite(light2Pin, s2);
  Serial.print("TOUCH2");
}