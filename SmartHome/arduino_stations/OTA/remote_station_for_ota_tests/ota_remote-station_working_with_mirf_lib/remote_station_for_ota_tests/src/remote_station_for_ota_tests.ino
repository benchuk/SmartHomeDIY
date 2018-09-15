#include <Arduino.h>
#include <EEPROM.h>

/* Memory Map */
#define CONFIG      0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define STATUS      0x07
#define OBSERVE_TX  0x08
#define CD          0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
#define FIFO_STATUS 0x17
#define DYNPD      0x1C
#define FEATURE     0x1D

/* Bit Mnemonics */
#define MASK_RX_DR  6
#define MASK_TX_DS  5
#define MASK_MAX_RT 4
#define EN_CRC      3
#define CRCO        2
#define PWR_UP      1
#define PRIM_RX     0
#define ENAA_P5     5
#define ENAA_P4     4
#define ENAA_P3     3
#define ENAA_P2     2
#define ENAA_P1     1
#define ENAA_P0     0
#define ERX_P5      5
#define ERX_P4      4
#define ERX_P3      3
#define ERX_P2      2
#define ERX_P1      1
#define ERX_P0      0
#define AW          0
#define ARD         4
#define ARC         0
#define PLL_LOCK    4
#define RF_DR       3
#define RF_PWR      6
#define RX_DR       6
#define TX_DS       5
#define MAX_RT      4
#define RX_P_NO     1
#define TX_FULL     0
#define PLOS_CNT    4
#define ARC_CNT     0
#define TX_REUSE    6
#define FIFO_FULL   5
#define TX_EMPTY    4
#define RX_FULL     1
#define RX_EMPTY    0
#define DPL_P5      5
#define DPL_P4      4
#define DPL_P3      3
#define DPL_P2      2
#define DPL_P1      1
#define DPL_P0      0
#define EN_DPL      2
#define EN_ACK_PAY  1
#define EN_DYN_ACK  0

/* Instruction Mnemonics */
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define REGISTER_MASK 0x1F
#define ACTIVATE      0x50
#define R_RX_PL_WID   0x60
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define W_ACK_PAYLOAD 0xA8
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define NOP           0xFF

/* Non-P omissions */
#define LNA_HCURR   0

/* P model memory Map */
#define RPD         0x09

/* P model bit Mnemonics */
#define RF_DR_LOW   5
#define RF_DR_HIGH  3
#define RF_PWR_LOW  1
#define RF_PWR_HIGH 2


#define CE_DDR    DDRB
#define CE_PORT   PORTB
#define CSN_DDR   DDRB
#define CSN_PORT  PORTB
#define CE_PIN    (1 << 1)
#define CSN_PIN   (1 << 2)

//#include "spi.h"
//#include "nrf24.h"


// #include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
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

//void enableRF()
//{
//    spi_init();
//     nrf24_init();
//     sei();
//     nrf24_set_rx_addr(add + 0);
//     nrf24_set_tx_addr(add + 3);
//     nrf24_rx_mode();
//}
#define CONFIG_VAL ((1 << MASK_RX_DR) | (1 << MASK_TX_DS) | \
    (1 << MASK_MAX_RT) | (1 << CRCO) | (1 << EN_CRC))
    
void enableRF(void)
{
  //SPI.begin();
  Mirf.csnPin = Radio_CSN;
  Mirf.cePin = Radio_CE;
  
  Mirf.spi = &MirfHardwareSpi;

  
  /*
     Setup pins / SPI.
  */
  Mirf.init();

  //Mirf.configRegister(SETUP_AW, 0x01);
 //Mirf.configRegister(EN_AA, 0x03);

  Mirf.configRegister(SETUP_RETR, 0x7f);
  
  /* Maximum Tx power, 250kbps data rate */
  Mirf.configRegister(RF_SETUP, (1 << RF_PWR_LOW) | (1 << RF_PWR_HIGH) |(1 << RF_DR_LOW));
  /* Dynamic payload length for TX & RX (pipes 0 and 1) */
  Mirf.configRegister(DYNPD, 0x03);
  Mirf.configRegister(FEATURE, 1 << EN_DPL);
  /* Reset status bits */
  Mirf.configRegister(STATUS, (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT));
  /* Set some RF channel number */
  Mirf.configRegister(RF_CH, 42);
  /* 3-byte addresses */
  Mirf.configRegister(SETUP_AW, 0x01);
  /* Enable ACKing on both pipe 0 & 1 for TX & RX ACK support */
  Mirf.configRegister(EN_AA, 0x03);
  
  /*
     Configure reciving address.
  */
  //Mirf.setRADDR(add);
  Mirf.setRADDR((byte *)"006");
  Mirf.setTADDR((byte *)"000");
  /*
     Set the payload length to sizeof(unsigned long) the
     return type of millis().

     NB: payload on client and server must be the same.
  */
  Mirf.payload = sizeof(uint8_t);//8;//sizeof(uint8_t);
  Mirf.channel = 42;
  /*
     Write channel and payload config then power up reciver.
  */
  Mirf.config();
  /* Enable 16-bit CRC */

  Mirf.configRegister(CONFIG, CONFIG_VAL | (1 << PWR_UP) | (1 << PRIM_RX));
  /* Only use data pipe 1 for receiving, pipe 0 is for TX ACKs */
  Mirf.configRegister(EN_RXADDR, 0x02);

  //nrf24_ce(1);
  //Mirf.configRegister(RF_SETUP,0x06);
}

//void(* resetFunc) (void) = 0; //declare reset function @ address 0 // only restart the code does not go into bootloader

void setup()
{
 
  Serial.begin(115200); // open the serial port at 9600 bps:
  //pinMode(5, OUTPUT);
  //digitalWrite(5, HIGH);
  
  //local flasher is 48 48 48
  //remote is 48 48 49

  //006 - 000
  EEPROM.write(0, 48);
  EEPROM.write(1, 48);
  EEPROM.write(2, 54);
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
   
   Serial.println("Listening...2");
   while(!Mirf.dataReady())
  {
    watchdogReset();
  };

    byte c; 
     // well, get it
     Mirf.getData(&c);

    // ... and write it out to the PC
     Serial.print(c);

     if(c=0xff)
     {
      Serial.println("RESET");
            while(1)
            {
              //digitalWrite(5, LOW);
              soft_restart();
            }
     }
  
//   if(nrf24_rx_fifo_data())
//   {
//      Serial.println("Data...");
//      uint8_t pkt_len, pkt_buf[32], i;
//      
//      nrf24_rx_read(pkt_buf, &pkt_len);
//      Serial.print("Size: ");
//      Serial.println(pkt_len);
//      
//      for (i = 0; i < pkt_len; i ++)
//          Serial.print(pkt_buf[i]);
//  
//          if(pkt_buf[0]==0xff)
//          {
//            Serial.println("RESET");
//            while(1)
//            {
//              //digitalWrite(5, LOW);
//              soft_restart();
//            }
//          }
//        
//    }
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
