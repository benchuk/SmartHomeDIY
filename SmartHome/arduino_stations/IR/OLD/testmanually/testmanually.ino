#include <Arduino.h>
int IRledPin = 13;


void setup() {
  // put your setup code here, to run once:
  pinMode(IRledPin, OUTPUT);
Serial.begin(9600);
}

void loop() {
 delayMicroseconds(36328);
  pulseIR(10000);
delayMicroseconds(820);
  pulseIR(1800);
   delay(10*1000);
}



void pulseIR(long microsecs) {
  // we'll count down from the number of microseconds we are told to wait

  cli();  // this turns off any background interrupts

  while (microsecs > 0) {
    // 38 kHz is about 13 microseconds high and 13 microseconds low
   digitalWrite(IRledPin, HIGH);  // this takes about 3 microseconds to happen
   delayMicroseconds(10);         // hang out for 10 microseconds
   digitalWrite(IRledPin, LOW);   // this also takes about 3 microseconds
   delayMicroseconds(10);         // hang out for 10 microseconds

   // so 26 microseconds altogether
   microsecs -= 26;
  }

  sei();  // this turns them back on
}
