
#include <avr/wdt.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

const int Radio_CSN = 10; // pinnenummer brukt for CSN på radio (ikke endre denne)
const int Radio_CE = 9;   //

// Watchdog functions. These are only safe with interrupts turned off.
void watchdogReset() {
  __asm__ __volatile__ (
    "wdr\n"
  );
}

#ifndef _SOFT_RESTART_H
#define _SOFT_RESTART_H


//Enter to bootloader state for OTA
#define soft_restart()         \
  \
  do                      \
    \
  {                       \
    wdt_enable(WDTO_15MS); \
    for (;;)               \
    {                      \
    }                      \
    \
  }                       \
  while (0)

#endif

//void(* resetFunc) (void) = 0; //declare reset function @ address 0 // only restart the code does not go into bootloader
char cmd[4];

void startRF(void)
{
  cmd[3] = 0;
  uint8_t address[6];
  Serial.println("our address:");
  address[0] = EEPROM.read(0);
  Serial.print(address[0]);
  address[1] = EEPROM.read(1);
  Serial.print(address[1]);
  address[2] = EEPROM.read(2);
  Serial.println(address[2]);

  Serial.println("burner address:");
  address[3] = EEPROM.read(3);
  Serial.print(address[3]);
  address[4] = EEPROM.read(4);
  Serial.print(address[4]);
  address[5] = EEPROM.read(5);
  Serial.println(address[5]);

  Mirf.csnPin = Radio_CSN;
  Mirf.cePin = Radio_CE;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  
  // name the receiving channel - must match tranmitter setting!
  Mirf.setRADDR("00001"/*(byte *)address*/);
  
  /*
    Set the payload length to sizeof(unsigned long) the
    return type of millis().

    NB: payload on client and server must be the same.
  */
  Mirf.payload = 3;//sizeof(unsigned long);//3;//sizeof(cmd); //8;//sizeof(uint8_t);
  Serial.print("payload size: ");
  Serial.println(Mirf.payload);
  Mirf.channel = 90;

  Serial.print("channel: ");
  Serial.println(Mirf.channel);
  
  // now config the device.... 
  Mirf.config();  

  //Mirf.configRegister(RF_SETUP,0x06);
}

void checkIfOtaRequestOrLoadCommand(char* data)
{
  Mirf.getData(data);
 
  Serial.print("Data content: ");
  for (int i = 0; i < Mirf.payload; i++)
  {
    Serial.print(data[i]);
  }
  Serial.println("");
  Serial.println("==============");
  data[Mirf.payload] = 0;
}
