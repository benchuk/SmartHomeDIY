#include <Arduino.h>
#include <common.h>
#include <SoftwareSerial.h>

SoftwareSerial BTSerial(6, 7); // RX | TX

bool firstPart = true;
char add[4];
int ttt = 0;
int i = 0;
bool newCommand = false;

// void configureEEPROMAddressForRFAndOTA(void)
// {
//   //my address "001"
//   EEPROM.write(0, 48);
//   EEPROM.write(1, 48);
//   EEPROM.write(2, 49);

//   //OTA station address (always "000")
//   EEPROM.write(3, 48);
//   EEPROM.write(4, 48);
//   EEPROM.write(5, 48);
// }

void setup()
{
  //STRING NULL TERMINATION
  add[3] = 0;
  add[0] = 'x';
  add[1] = 'x';
  add[2] = 'x';
  

  Serial.begin(115200);
  Serial.println("setup...");
  configureEEPROMAddressForRFAndOTA("001");
  startRF();
  //BTSerial.begin(38400);  // HC-05 default speed in AT command more
  BTSerial.begin(9600); //custom
  Serial.println("Dispatch station ready.");
}

void loop()
{
  watchdogReset();
  char otaCmd[Mirf.payload + 1];
  checkIfOtaRequestOrLoadCommand(otaCmd);

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
      Serial.println(command);
      Serial.println(i);
      add[i] = command;
      Serial.println("========");
      Serial.println(add[i]);
      Serial.println("========");
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

    if (newCommand)
    {
      Serial.println("sending: ");
      Serial.print("address: ");
      Serial.println(add);
      Mirf.setTADDR((byte *)add);
      Serial.print("command: ");
      Serial.println(cmd);
      Mirf.send((uint8_t *)cmd);
      newCommand = false;
      while (Mirf.isSending());
    }
  }
}
