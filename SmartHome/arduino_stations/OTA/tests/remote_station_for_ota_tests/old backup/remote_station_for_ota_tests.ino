#include <EEPROM.h>

#define CE_DDR    DDRB
#define CE_PORT   PORTB
#define CSN_DDR   DDRB
#define CSN_PORT  PORTB
#define CE_PIN    (1 << 1)
#define CSN_PIN   (1 << 2)

#include "spi.h"
#include "nrf24.h"


//#include <SPI.h>
//#include <Mirf.h>
//#include <nRF24L01.h>
//#include <MirfHardwareSpiDriver.h>
/*
Uses a FOR loop for data and prints a number in various formats.
*/
int x = 0; // variable

const int Radio_CSN = 10; // pinnenummer brukt for CSN på radio (ikke endre denne)
const int Radio_CE = 9;   //
uint8_t add[6];

// Watchdog functions. These are only safe with interrupts turned off.
void watchdogReset() {
  __asm__ __volatile__ (
    "wdr\n"
  );
}

#ifndef _SOFT_RESTART_H
#define _SOFT_RESTART_H

#include <avr/wdt.h>

#define soft_restart()        \
do                          \
{                           \
    wdt_enable(WDTO_15MS);  \
    for(;;)                 \
    {                       \
    }                       \
} while(0)

#endif

void enableRF()
{
    spi_init();
    nrf24_init();
    sei();
    nrf24_set_rx_addr(add + 0);
    nrf24_set_tx_addr(add + 3);
    nrf24_rx_mode();
}
//void enableRF(void)
//{
//  SPI.begin();
//  Mirf.csnPin = Radio_CSN;
//  Mirf.spi = &MirfHardwareSpi;
//  /*
//     Setup pins / SPI.
//  */
//  Mirf.init();
//  /*
//     Configure reciving address.
//  */
//  //Mirf.setRADDR(add);
//  Mirf.setRADDR((byte *)"001");
//  Mirf.setTADDR((byte *)"000");
//  /*
//     Set the payload length to sizeof(unsigned long) the
//     return type of millis().
//
//     NB: payload on client and server must be the same.
//  */
//  Mirf.payload = 1;//sizeof(uint8_t);
// // Mirf.channel = 90;
//  /*
//     Write channel and payload config then power up reciver.
//  */
//  Mirf.config();
//
//  //Mirf.configRegister(RF_SETUP,0x06);
//}
void(* resetFunc) (void) = 0; //declare reset function @ address 0

void setup()
{
  Serial.begin(115200); // open the serial port at 9600 bps:
  //pinMode(5, OUTPUT);
  //digitalWrite(5, HIGH);
  
  //local flasher is 48 48 48
  //remote is 48 48 49

  //001 - 000
  EEPROM.write(0, 48);
  EEPROM.write(1, 48);
  EEPROM.write(2, 49);
  EEPROM.write(3, 48);
  EEPROM.write(4, 48);
  EEPROM.write(5, 48);

  //001 - 000
  //EEPROM.write(0, 48);
  //EEPROM.write(1, 48);
  //EEPROM.write(2, 49);
  //EEPROM.write(3, 48);
  //EEPROM.write(4, 48);
  //EEPROM.write(5, 48);
  //000 - 001
  // EEPROM.write(0, 48);
  // EEPROM.write(1, 48);
  // EEPROM.write(2, 48);
  // EEPROM.write(3, 48);
  // EEPROM.write(4, 48);
  // EEPROM.write(5, 49);
  //
  Serial.println("our address:");
  add[0] = EEPROM.read(0);
  Serial.print(add[0]);
  add[1] = EEPROM.read(1);
  Serial.print(add[1]);
  add[2] = EEPROM.read(2);
  Serial.println(add[2]);

  Serial.println("burner address:");
  add[3] = EEPROM.read(3);
  Serial.print(add[3]);
  add[4] = EEPROM.read(4);
  Serial.print(add[4]);
  add[5] = EEPROM.read(5);
  Serial.println(add[5]);

  enableRF();
}

void loop()
{
   
   Serial.println("Listening...");
   if(nrf24_rx_fifo_data())
   {
      Serial.println("Data...");
      uint8_t pkt_len, pkt_buf[32], i;
      
      nrf24_rx_read(pkt_buf, &pkt_len);
      Serial.print("Size: ");
      Serial.println(pkt_len);
      
      for (i = 0; i < pkt_len; i ++)
          Serial.print(pkt_buf[i]);
  
          if(pkt_buf[0]==0xff)
          {
            Serial.println("RESET");
            while(1)
            {
              //digitalWrite(5, LOW);
              soft_restart();
            }
          }
        
    }
    watchdogReset();
    delay(1000);
    watchdogReset();
   

      
//  byte data[Mirf.payload] = {0};
//  while(!Mirf.dataReady())
//  {
//  };
//    // wait for data to arrive
//    Mirf.getData(data); // read to buffer
//    Serial.println("Network data");
//    Serial.println(data[0]);
//    Serial.println(data[1]);
//    Serial.println("---------");

  
//  // print labels
//  Serial.print("NO FORMAT"); // prints a label
//  Serial.print("\t");        // prints a tab
//
//  Serial.print("DEC");
//  Serial.print("\t");
//
//  Serial.print("HEX");
//  Serial.print("\t");
//
//  Serial.print("OCT");
//  Serial.print("\t");
//
//  Serial.print("BIN");
//  Serial.print("\t");

//  for (x = 0; x < 64; x++)
//  { // only part of the ASCII chart, change to suit
// watchdogReset();
//    Serial.print("----------1nowtoday");
//    // print it out in many formats:
//    Serial.print(x);    // print as an ASCII-encoded decimal - same as "DEC"
//    Serial.print("\t"); // prints a tab
//
//    Serial.print(x, DEC); // print as an ASCII-encoded decimal
//    Serial.print("\t");   // prints a tab
//
//    Serial.print(x, HEX); // print as an ASCII-encoded hexadecimal
//    Serial.print("\t");   // prints a tab
//
//    Serial.print(x, OCT); // print as an ASCII-encoded octal
//    Serial.print("\t");   // prints a tab
//
//    Serial.println(x, BIN); // print as an ASCII-encoded binary
//    //                             then adds the carriage return with "println"
//    delay(20); // delay 200 milliseconds
//  }
//  Serial.println(""); // prints another carriage return
}
