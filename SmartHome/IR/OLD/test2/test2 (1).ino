#include <IRremote.h>
#include <IRremoteInt.h>


int RECV_PIN = 11;
IRrecv irrecv(RECV_PIN);
decode_results results;
IRsend irsend;
//unsigned int powerOn[148] = {4400,4250,550,1600,600,1550,600,1550,550,1600,550,500,550,550,550,1550,600,500,550,500,600,500,550,500,600,450,600,1550,600,1550,550,500,600,1550,550,550,550,500,600,500,550,500,550,500,600,500,550,1600,550,1550,600,1550,600,1550,600,1550,600,1550,550,1600,550,1550,600,500,550,500,600,500,550,500,550,550,550,500,600,450,600,500,550,500,550,1600,550,1600,550,500,600,500,550,1600,550,500,550,500,550,550,550,500,600,500,550,500,600,450,600,500,550,500,550,550,550,500,600,1550,600,450,600,500,550,500,550,550,550,500,600,450,600,500,550,500,600,1550,550,500,600,500,600,1550,550,500,600,500,550,500,550,500,600};
unsigned int powerOn[40] = {45583,54,17,10,16,10,8,9,8,27,26,9,8,10,8,9,8,10,8,9,8,10,7,10,8,10,7,10,8,10,7,19,16,10,8,9,8,10,8,9};
unsigned long long OnOff = 0xc800f740cLL;
int toggle = 0;

void sendOnOff() {
  if (toggle == 0) {
    irsend.sendRC6(OnOff, 36);
  } else {
    irsend.sendRC6(OnOff ^ 0x8000, 36);
  }
  toggle = 1 - toggle;
}
   
   
void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  //irsend.enableIROut(38); // Start the receiver

}

void loop()
{
   irsend.sendRC6(65552, 36);
 
  //irsend.sendRaw(powerOn, 40, 36);
//  if (irrecv.decode(&results))
//    {
//        Serial.println("okokokokok");
//     Serial.println(results.value);
//             Serial.println("len");
//             Serial.println(results.rawlen);
//             Serial.println("len end");
//    Serial.println("DATA");
//    for(int i = 0; i < results.rawlen; i++)
//    {
//     Serial.println(results.rawbuf[i]);
//    }
//        Serial.println("EDATA");
//     //irsend.sendRaw((unsigned int*)results.rawbuf, results.rawlen, 38);
//     delay(100);
//     Serial.println("sendRaw OK");
//      delay(100);
 //   irrecv.resume(); // Receive the next value
//      delay(100);
//          Serial.println("sendRaw OKOKOKOKOK");
   // }
    delay(10);
    
}

