#include <Arduino.h>

#define BOOTLOADERVER 0x12
#define SECRET_CODE   0xABCD1234U

const uint32_t AUTH_CMD = 0x03;
const uint32_t SEED     = 0x12345678;
const uint32_t XOR_KEY  = 0xABCD1234;

uint32_t expectedKey = 0;

void processTestTarget();
void processBootLoaderVerReq();
void processAuthorization();


void setup()
{
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
     
}

void loop()
{
    static uint8_t authCmdRcvd = 0;
    static uint8_t commandProcessInprogress = 0;

    uint8_t serialData = 0;

    // Check if any Serial command arrives
    if (Serial.available())
    {
        serialData = Serial.read();
        // if no commandProcesssInprogress then its a command
        if (!commandProcessInprogress) {
          commandProcessInprogress = 1;
          // learn which command is it
          switch(serialData) {
            case 1:
              commandProcessInprogress = 1;
              processTestTarget();
              commandProcessInprogress = 0;
              break;
            case 2:
              commandProcessInprogress = 1;
              processBootLoaderVerReq();
              commandProcessInprogress = 0;
              break;
            case 3:
              commandProcessInprogress = 1;
              processAuthorization();
              commandProcessInprogress = 0;
        }
      }
    }
}

void processBootLoaderVerReq() {
  // accept the bootloader version request by a 0 response
  Serial.write(0);
  delay(1000);
  // provide the bootloader version
  Serial.write(BOOTLOADERVER);
}


void processTestTarget() {
  // accept the bootloader version request by a 0 response
  Serial.write(0);
  // blink LED
  for(int i=0; i < 10; i++) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(480);
  }
  Serial.write(0);
}

void processAuthorization() {
  char match = 1;
  unsigned long seed, key; 
  unsigned char rcvdKey[4];
  // accept the bootloader version request by a 0 response
  delay(100);
  Serial.write(0);
  delay(100);
  // create seed as current time in milliseconds.
  seed = millis();
  key = (unsigned long)(seed ^ SECRET_CODE);
  Serial.write(seed & 0xFF);
  Serial.write((seed & 0xFF00) >> 8);
  Serial.write((seed & 0xFF0000) >> 16);
  Serial.write((seed & 0xFF000000) >> 24);

  // wait for a serial message
  while(!Serial.available());
    rcvdKey[0] = Serial.read();
  while(!Serial.available());
    rcvdKey[1] = Serial.read();
  while(!Serial.available());
    rcvdKey[2] = Serial.read();
  while(!Serial.available());
    rcvdKey[3] = Serial.read();

  // match key
  match = rcvdKey[0] == (key & 0xFF);
  match &= rcvdKey[1] == ((key & 0xFF00) >> 8);
  match &= rcvdKey[2] == ((key & 0xFF0000) >> 16);
  match &= rcvdKey[3] == ((key & 0xFF000000) >> 24);
  if(match) Serial.write(0);
  else Serial.write(-1);
}
