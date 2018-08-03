/* Raw IR decoder sketch!
 
 This sketch/program uses the Arduno and a PNA4602 to 
 decode IR received. This can be used to make a IR receiver
 (by looking for a particular code)
 or transmitter (by pulsing an IR LED at ~38KHz for the
 durations detected 
 
 Code is public domain, check out www.ladyada.net and adafruit.com
 for more tutorials! 
 */
 
// We need to use the 'raw' pin reading methods
// because timing is very important here and the digitalRead()
// procedure is slower!
//uint8_t IRpin = 2;
// Digital pin #2 is the same as Pin D2 see
// http://arduino.cc/en/Hacking/PinMapping168 for the 'raw' pin mapping
#define IRpin_PIN      PIND
#define IRpin          2
 
// the maximum pulse we'll listen for - 65 milliseconds is a long time
#define MAXPULSE 40000
 
// what our timing resolution should be, larger is better
// as its more 'precise' - but too large and you wont get
// accurate timing
#define RESOLUTION 20 
 
// we will store up to 100 pulse pairs (this is -a lot-)
uint16_t pulses[200][2];  // pair is high and low pulse 
uint8_t currentpulse = 0; // index for pulses we're storing
 bool coderecorded = false;
// int IRledPin =  13;    // LED connected to digital pin 13
   int IRledPin =  7;    // LED connected to digital pin 13
 int counter = 0;
   uint16_t highpulse, lowpulse;  // temporary storage timing
   
void setup(void) {
  pinMode(IRledPin, OUTPUT);   
  Serial.begin(9600);
  Serial.println("Ready to decode IR!");
}
 
void loop(void) {
  if(!coderecorded)
  {
    recordircode();
    
  }
  else
  {
    //Serial.println("Sending....");
    sendcode();
    delay(68);
    sendcode();
//    delay(68);
//    sendcode();
    delay(1500);
  }
  
  
}
 
 void sendcode()
 {
   cli(); // this turns off any background interrupts
    for (uint8_t i = 0; i < counter; i++) {

      delayMicroseconds((pulses[i][0] * RESOLUTION));      
      pulseIR((pulses[i][1] * RESOLUTION));

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
    delayMicroseconds((pulses[0][0] * RESOLUTION));   
    sei();  // this turns them back on
 }
 
 // This procedure sends a 38KHz pulse to the IRledPin 
// for a certain # of microseconds. We'll use this whenever we need to send codes
void pulseIR(long microsecs) {
  // we'll count down from the number of microseconds we are told to wait
 
  
 
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

 void recordircode()
 {
  
  highpulse = lowpulse = 0; // start out with no pulse length
 
   //while(1)
   {
      cli(); // this turns off any background interrupts
      while (IRpin_PIN & (1 << IRpin))
      {
       // pin is still HIGH
   
       // count off another few microseconds
       highpulse++;
       delayMicroseconds(RESOLUTION);
   
       // If the pulse is too long, we 'timed out' - either nothing
       // was received or the code is finished, so print what
       // we've grabbed so far, and then reset
         if ((highpulse*RESOLUTION >= MAXPULSE) && (currentpulse != 0)) 
         {
           printpulses();
           currentpulse=0;
           return;
         }
      }
      
      // we didn't time out so lets stash the reading
      //pulses[currentpulse][0] = highpulse+2;
      pulses[currentpulse][0] = highpulse;
 
      // same as above
      while (! (IRpin_PIN & (1 << IRpin) ))
      {
         // pin is still LOW
         lowpulse++;
         delayMicroseconds(RESOLUTION);
         if ((lowpulse*RESOLUTION >= MAXPULSE)  && (currentpulse != 0)) 
         {
            printpulses();
            currentpulse=0;
            return;
         }
      }
      
      //pulses[currentpulse][1] = lowpulse+1;
       pulses[currentpulse][1] = lowpulse;
 
      // we read one high-low pulse successfully, continue!
      currentpulse++;
      sei();  // this turns them back on
  }
}
void printpulses(void) {
  coderecorded = true;
  Serial.println("\n\r\n\rReceived: \n\rOFF \tON");
  //currentpulse++;
//  pulses[currentpulse][0] = 44*1000 / RESOLUTION;
//  pulses[currentpulse][1] = 9*1000 / RESOLUTION;
//  currentpulse++;
//   pulses[currentpulse][0] = 2*1000 / RESOLUTION;
//  pulses[currentpulse][1] = 0.53*1000 / RESOLUTION;
//  currentpulse++;
  for (uint8_t i = 0; i < currentpulse; i++) {
    Serial.print(pulses[i][0] * RESOLUTION, DEC);
    Serial.print(" usec, ");
    Serial.print(pulses[i][1] * RESOLUTION, DEC);
    Serial.println(" usec");
  }
  counter = currentpulse;
}
