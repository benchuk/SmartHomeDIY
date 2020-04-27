#define FLASH_TOOL_MODE
#define MAX_PKT_SIZE 32
#define SEQN

static void flasher_setup(void);
static void flasher_rx_handle(void);
static void flasher_tx_handle(void);

/*
   Passthrough between UART and an nRF24.

   Licensed under AGPLv3.
*/
#include <Arduino.h>
#include <EEPROM.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <string.h>

#include "timer1.h"
#include "uart.h"

/* Extra pins configuration for nRF24L01. Here :
    - CE  = PIN9  (PORTB1)
    - CSN = PIN10 (PORTB2)
*/
#define CE_DDR DDRB
#define CE_PORT PORTB
#define CSN_DDR DDRB
#define CSN_PORT PORTB
#define CE_PIN (1 << 1)
#define CSN_PIN (1 << 2)

#include "nrf24.h"
#include "spi.h"

#ifndef MAX_PKT_SIZE
#define MAX_PKT_SIZE 32
#endif

#define FIFO_MASK 255
static struct ring_buffer_s {
    uint8_t data[FIFO_MASK + 1];
    uint8_t start, len;
} tx_fifo;

#define BLINK_LED_PIN 7

void blink(void) {
    digitalWrite(BLINK_LED_PIN, HIGH);
    my_delay(300);
    digitalWrite(BLINK_LED_PIN, LOW);
    my_delay(300);
    //    digitalWrite(BLINK_LED_PIN, HIGH);
    //    my_delay(300);
    //    digitalWrite(BLINK_LED_PIN, LOW);
    //    my_delay(300);
}

static void handle_input(char ch) {
    //serial_write1(ch);
    //blink();
    tx_fifo.data[(tx_fifo.start + tx_fifo.len++) & FIFO_MASK] = ch;
}

// #define min(a, b) \
//   ({ __typeof__ (a) _a = (a); \
//     __typeof__ (b) _b = (b); \
//     _a < _b ? _a : _b; })

static uint8_t eeprom_read(uint16_t addr) {
    while (EECR & (1 << EEPE))
        ;

    EEAR = addr;
    EECR |= 1 << EERE; /* Start eeprom read by writing EERE */

    return EEDR;
}

void programShadeLivingRoom(void) {
    //station 002
    EEPROM.write(3, 48);  //0
    EEPROM.write(4, 48);  //0
    //EEPROM.write(5, 52); //4
    EEPROM.write(5, 55);  //7
}

void setup(void) {
    //local flasher is 48 48 48 => '000'
    EEPROM.write(0, 48);
    EEPROM.write(1, 48);
    EEPROM.write(2, 48);

    //set remote address: for example: 48 48 49 => '001'
    programShadeLivingRoom();

    uint8_t s = SREG;
    uint8_t m = MCUCR;
    uint8_t i, addrs[6];

    wdt_disable();
    serial_init();
    timer_init();
    spi_init();
    nrf24_init();
    sei();
    pinMode(BLINK_LED_PIN, OUTPUT);
    /*
     Set our radio address and the remote end's radio address, read
     the addresses from EEPROM where they need to be saved first.
  */
    for (i = 0; i < 6; i++)
        addrs[i] = eeprom_read(i);

    nrf24_set_rx_addr(addrs + 0);
    nrf24_set_tx_addr(addrs + 3);

    /* Write something to say hello */
    serial_write_str("-SREG:");
    serial_write_hex16(s);
    serial_write_str(", MCUCR:");
    serial_write_hex16(m);
    serial_write_str(", our addr: ");
    serial_write1(addrs[0]);
    serial_write1(addrs[1]);
    serial_write1(addrs[2]);

    serial_write_str(", remote addr: ");
    serial_write1(addrs[3]);
    serial_write1(addrs[4]);
    serial_write1(addrs[5]);
    serial_write_eol();

    nrf24_rx_mode();

    serial_set_handler(handle_input);
}

void loop(void) {
    static uint8_t tx_cnt; /* Consecutive Tx packets counter */

    /*
     Note: all nrf24 calls are serialised in this function so as
     to avoid any concurrency issues.
  */

    if (nrf24_rx_fifo_data()) {
        uint8_t pkt_len, pkt_buf[32], i;
#ifdef SEQN
        static uint8_t seqn = 0xff;
#endif

#ifdef FLASH_TOOL_MODE
        flasher_rx_handle();
#endif

        nrf24_rx_read(pkt_buf, &pkt_len);

#ifdef SEQN
        if (pkt_buf[0] != seqn) {
            seqn = pkt_buf[0];
            for (i = 1; i < pkt_len; i++)
                serial_write1(pkt_buf[i]);
        }
#else
        for (i = 0; i < pkt_len; i++)
            serial_write1(pkt_buf[i]);
#endif

        tx_cnt = 0;
    }

    if (tx_fifo.len) { /* .len access should be atomic */
        uint8_t pkt_len, pkt_buf[MAX_PKT_SIZE], split;
#ifdef SEQN
        static uint8_t seqn = 0x00;
        uint8_t count = 128;
#define MAX_PLD_SIZE (MAX_PKT_SIZE - 1)

        pkt_buf[0] = seqn++;
#else
        uint8_t count = 2;
#define MAX_PLD_SIZE MAX_PKT_SIZE
#endif

#ifdef FLASH_TOOL_MODE
        flasher_tx_handle();
#endif

        tx_cnt++;

        cli();
        pkt_len = min(tx_fifo.len, MAX_PLD_SIZE);
        sei();

        /* HACK */
        if (tx_cnt == 2 && MAX_PLD_SIZE > 2 && pkt_len == MAX_PLD_SIZE)
            pkt_len = MAX_PLD_SIZE - 2;
        else if (MAX_PLD_SIZE > 2 && tx_cnt == 3)
            pkt_len = 1;

        split = min(pkt_len,
                    (uint16_t)(~tx_fifo.start & FIFO_MASK) + 1);

#define START (MAX_PKT_SIZE - MAX_PLD_SIZE)
        memcpy(pkt_buf + START, tx_fifo.data + (tx_fifo.start & FIFO_MASK), split);
        memcpy(pkt_buf + START + split, tx_fifo.data, pkt_len - split);
        /*
       Or we could just do pkt_buf = tx_fifo.data + ...;
       pkt_len = split;
    */

        cli();
        tx_fifo.len -= pkt_len;
        tx_fifo.start += pkt_len;
        sei();

        while (--count) {
            /* Don't flood the remote end with the comms */
            my_delay(4);

            //blink();
            nrf24_tx(pkt_buf, pkt_len + START);
            if (!nrf24_tx_result_wait())
                break;
        }
    }
}

int main(void) {
    setup();

#ifdef FLASH_TOOL_MODE
    flasher_setup();
#endif

    for (;;)
        loop();

    return 0;
}

static void flasher_setup(void) {
}

static uint32_t prev_txrx_ts = 0;

static void flasher_rx_handle(void) {
    prev_txrx_ts = timer_read();
}

static void flasher_tx_handle(void) {
    //serial_write1('t');
    static uint8_t first_tx = 1;

    /*
     If more than a second has passed since previous communication
     the bootloader will have left the flash mode by now so this is
     probably a new boot and a new flashing attempt.
  */
    if ((uint32_t)(timer_read() - prev_txrx_ts) > F_CPU)
        first_tx = 1;

    /*
     Before any actual STK500v2 communication begins we need to
     attempt to reset the board to start the bootloader, and send it
     our radio address to return the ACK packets to.
  */
    if (first_tx) {
        uint8_t ch, pkt[4];

        /*
       Our protocol requires any program running on the board
       to reset it if it receives a 0xff byte.
    */
        ch = 0xff;
        nrf24_tx(&ch, 1);
        nrf24_tx_result_wait();

        /* Give the board time to reboot and enter the bootloader */
        my_delay(100);

        /* Finally send our address + pkt size as a separate packet */
        pkt[0] = eeprom_read(0);
        pkt[1] = eeprom_read(1);
        pkt[2] = eeprom_read(2);
        pkt[3] = MAX_PKT_SIZE;
        /* TODO: set the no-ACK bit, the remote board can't ACK yet */
        nrf24_tx(pkt, 4);
        nrf24_tx_result_wait();

        first_tx = 0;
        blink();
    }

    prev_txrx_ts = timer_read();
}
