/*
 * IRrecord: record and play back IR signals as a minimal 
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * An IR LED must be connected to the output PWM pin 3.
 * A button must be connected to the input BUTTON_PIN; this is the
 * send button.
 * A visible LED can be connected to STATUS_PIN to provide status.
 *
 * The logic is:
 * If the button is pressed, send the IR code.
 * If an IR code is received, record it.
 *
 * Version 0.11 September, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

#include <IRremote.h>
#include <IRremoteInt.h>
int RECV_PIN = 11;
//int BUTTON_PIN = 12;
//int STATUS_PIN = 13;
int IRledPin = 13;
IRrecv irrecv(RECV_PIN);
IRsend irsend;

decode_results results;
int buttonState = LOW;
void setup()
{
  Serial.begin(9600);
   pinMode(IRledPin, OUTPUT);
  irrecv.enableIRIn(); // Start the receiver
  //pinMode(BUTTON_PIN, INPUT);
  //pinMode(STATUS_PIN, OUTPUT);
  digitalWrite(IRledPin, LOW);
}

// Storage for the recorded code
int codeType = -1; // The type of code
unsigned long codeValue; // The code value if not raw
unsigned int rawCodes[RAWBUF]; // The durations if raw
int codeLen; // The length of the code
int toggle = 0; // The RC5/6 toggle state
unsigned int *rawbuf2;
int counter = 0;
// Stores the code for later playback
// Most of this code is just logging
void storeCode(decode_results *results) {
  codeType = results->decode_type;
  int count = results->rawlen;
    Serial.println("Received unknown code, saving as raw");
    codeLen = results->rawlen - 1;
    counter = codeLen;
    // To store raw codes:
    // Drop first value (gap)
    // Convert from ticks to microseconds
    // Tweak marks shorter, and spaces longer to cancel out IR receiver distortion
    for (int i = 1; i <= codeLen; i++) {
      if (i % 2) {
        // Mark
        //Serial.println(results->rawbuf[i], DEC);
       
        rawCodes[i - 1] = results->rawbuf[i]*USECPERTICK - MARK_EXCESS;
        Serial.print(" m");
      } 
      else {
        // Space
        //Serial.println(results->rawbuf[i], DEC);
        
        rawCodes[i - 1] = results->rawbuf[i]*USECPERTICK + MARK_EXCESS;
        Serial.print(" s");
      }
      //counter++;
      Serial.print(rawCodes[i - 1], DEC);
    }
    Serial.println("");

    Serial.println(results->value, HEX);
    codeValue = results->value;
    Serial.print("codeLen: ");
    codeLen = counter;//results->bits;
    Serial.println(codeLen);
    rawbuf2 = (unsigned int *)results->rawbuf;
  
}

void sendCode(int repeat) {

    // Assume 38 KHz
    Serial.println("sendCode");
    Serial.print("code length: ");
    Serial.println(codeLen);
    //irsend.sendRaw(rawCodes, codeLen, 38);
    for (int i = 0; i < codeLen; i++) {
    if (i % 2) {
       //Serial.println("LOW");
       //digitalWrite(out_PIN, 10);
       //Serial.println(rawCodes[i]);
       //delayMicroseconds(rawCodes[i]);
       
      
      
      delayMicroseconds(rawCodes[i]);
      
    }
    else {
      //Serial.println("HIGH");
      //analogWrite(out_PIN, 127);
      //Serial.println(rawCodes[i]);
      //Serial.println(rawCodes[i]/100);
      //delayMicroseconds(rawCodes[i]);
      pulseIR((rawCodes[i])); 
       
     
    }
  }
    //irsend.sendRaw(rawbuf2, codeLen, 38);
    //Serial.println("Sent raw");
    //Serial.println(rawCodes, HEX);
    //Serial.println(codeLen);

}

int lastButtonState;

void loop() {
  digitalWrite(IRledPin, LOW);
  //Serial.println(TIMER_PWM_PIN);
  // If button pressed, send the code.
   //digitalRead(BUTTON_PIN);
  if (lastButtonState == HIGH && buttonState == LOW) {
    Serial.println("Released");
    irrecv.enableIRIn(); // Re-enable receiver
  }

  if (buttonState) {
    Serial.println("wating...");
    digitalWrite(IRledPin, LOW);
    delay(3000);
    //noInterrupts();
    Serial.println("Pressed, sending");
    //digitalWrite(STATUS_PIN, HIGH);
    sendCode(lastButtonState == buttonState);
    digitalWrite(IRledPin, LOW);
     //digitalWrite(out_PIN, LOW);
    //digitalWrite(STATUS_PIN, LOW);
    Serial.println("sent...");
    //delay(3000); // Wait a bit between retransmissions
    
  } 
  else if (irrecv.decode(&results)) {
    //digitalWrite(STATUS_PIN, HIGH);
    Serial.println("Saving Code...");
    
    storeCode(&results);
    buttonState = HIGH;
    //irrecv.resume(); // resume receiver
    //digitalWrite(STATUS_PIN, LOW);
  }
  
  lastButtonState = buttonState;
   
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
