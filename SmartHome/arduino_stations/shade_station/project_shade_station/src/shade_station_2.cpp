#include <Arduino.h>
#include <common.h>

#define SHADE_UP 6
#define SHADE_DOWN 7

#define LED_PIN 5
#define LED2_PIN 8

#define MY_HIGH LOW
#define MY_LOW HIGH

unsigned long startTime = millis();
unsigned long currentTime = millis();
bool inAction = false;
// void configureEEPROMAddressForRFAndOTA(void)
// {
//   //my address "003"
//   EEPROM.write(0, 48);
//   EEPROM.write(1, 48);
//   EEPROM.write(2, 50);

//   //OTA station address (always "000")
//   EEPROM.write(3, 48);
//   EEPROM.write(4, 48);
//   EEPROM.write(5, 48);
// }

void stop(void)
{
  inAction = false;
  digitalWrite(SHADE_UP, MY_HIGH);
  digitalWrite(SHADE_DOWN, MY_HIGH);
  Serial.println("Stop...");
  delay(1000);
}



void down(void)
{
  stop();
  startTime = millis();
  inAction = true;
  digitalWrite(SHADE_DOWN, MY_LOW);
  Serial.println("Down...");
}

void up(void)
{
  stop();
  startTime = millis();
  inAction = true;
  digitalWrite(SHADE_UP, MY_LOW);
  Serial.println("Up...");
}

void stopIfNeeded(void)
{
  if (!inAction)
  {
    return;
  }
  currentTime = millis();
  unsigned long elapsedTime = currentTime - startTime;
  Serial.print("elapsedTime: ");
  Serial.println(elapsedTime);
  if (elapsedTime > 38000)
  {
    Serial.println("timeup stopping..");
    stop();
  }
}

void blink(int d)
{
  digitalWrite(LED_PIN, HIGH);
  delay(d/2);
  digitalWrite(LED_PIN, LOW);
  delay(d/2);
}

void blink2(int d)
{
  digitalWrite(LED2_PIN, HIGH);
  delay(d);
  digitalWrite(LED2_PIN, LOW);
}

void setup()
{

  Serial.begin(115200);
  Serial.println("setup...");
  configureEEPROMAddressForRFAndOTA("003");
  pinMode(SHADE_UP, OUTPUT);
  pinMode(SHADE_DOWN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  stop();
  startRF();
}

void loop()
{
  blink(500);
  blink(500);
  stopIfNeeded();
  Serial.println("Listening...");

  while (!Mirf.dataReady())
  {
    stopIfNeeded();
    watchdogReset();
    blink(100);
  };

  Serial.println("Data...");

  blink2(500);
  char cmd[Mirf.payload + 1];
  checkIfOtaRequestOrLoadCommand(cmd);
  Serial.print("Return data: ");
  Serial.println(cmd);
  if (strcmp(cmd, "000") == 0)
  {
    stop();
  }
  else if (strcmp(cmd, "001") == 0)
  {
    up();
  }
  else if (strcmp(cmd, "002") == 0)
  {
    down();
  }
  else if (strcmp(cmd, "003") == 0)
  {
    while (1)
    {
      soft_restart();
    }
  }
  while( Mirf.isSending() ) ; 
}
