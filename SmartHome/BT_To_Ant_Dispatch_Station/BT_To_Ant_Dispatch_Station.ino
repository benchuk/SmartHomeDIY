#include "common.h"

SoftwareSerial BTSerial(6, 7); // RX | TX

bool firstPart = true;
char add[4];
int ttt = 0;
int i = 0;
bool newCommand = false;
void configureEEPROMAddressForRFAndOTA(void)
{
  //my address "001"
  EEPROM.write(0, 48);
  EEPROM.write(1, 48);
  EEPROM.write(2, 49);

  //OTA station address (always "000")
  EEPROM.write(3, 48);
  EEPROM.write(4, 48);
  EEPROM.write(5, 48);
}

void setup()
{
  cmd[3] = 0;
  add[3] = 0;

  Serial.begin(9600);
  Serial.println("setup...");

  configureEEPROMAddressForRFAndOTA();
  startRF();

  Serial.print("address size: ");
  Serial.println(Mirf.adderSize);
  Serial.print("payload size: ");
  Serial.println(Mirf.payload);

  //BTSerial.begin(38400);  // HC-05 default speed in AT command more
  BTSerial.begin(9600);  //custom

  Serial.println("Dispatch station ready.");
}


void loop()
{

  //digitalWrite(Radio_CSN, LOW); // ENABLE radio

  // Keep reading from HC-05 and send to Arduino Serial Monitor
  if (BTSerial.available())
  {
    char command = BTSerial.read();
    //Serial.println(command);
    if (command == ':')
    {
      Serial.println("middle");
      i = 0;
      firstPart = false;
      return;
    }
    if (firstPart)
    {
      Serial.println("firstPart");
      add[i] = command;
      i++;
    }
    else
    {
      Serial.println("end");
      cmd[i] = command;
      i++;
      if (i == 3)
      {
        firstPart = true;
        i = 0;
        newCommand = true;
      }
    }

    //delay(1000);
    //add code to check what was requested from serial server (PI station)
    //byte data[Mirf.payload];

    //dispatch command to remote rf station
    //if (!Mirf.isSending() /*&& Mirf.dataReady()*/)
    //{
    if (newCommand)
    {
      Serial.println("sending: ");
      Serial.print("address: ");
      Serial.println(add);
      Serial.print("command: ");
      Serial.println(cmd);
      //Mirf.setTADDR("002"/*(byte *)add/*"irir1"*/);
      Mirf.setTADDR((byte *)add);
      Mirf.send(cmd);
      newCommand = false;
      while( Mirf.isSending() ) ;
    }
    //}
  }
}
