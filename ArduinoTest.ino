#include <Arduino.h>

#define BOOTLOADERVER 0x12
#define SECRET_CODE   0xABCD1234

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
}

void processAuthorization() {
  unsigned long seed, key, rcvdKey;
  // accept the bootloader version request by a 0 response
  Serial.write(0);
  // create seed as current time in milliseconds.
  seed = millis();
  key = (seed ^ SECRET_CODE) & 0xFFFFFFFF;
  Serial.write(key & 0xFF);
  Serial.write((key & 0xFF00) >> 8);
  Serial.write((key & 0xFF0000) >> 16);
  Serial.write((key & 0xFF000000) >> 24);

  while(!Serial.available());
  rcvdKey = Serial.read() << 24;
  rcvdKey = rcvdKey | (Serial.read() << 16);
  rcvdKey = rcvdKey | (Serial.read() << 8);
  rcvdKey = rcvdKey | Serial.read();

  // match key
  if(key == rcvdKey) Serial.write(0);
  else Serial.write(-1);
}


     /*   

        if (cmd == AUTH_CMD)
        {
            // Authorization accepted
            Serial.write((int8_t)0);

            delay(1000);

            uint32_t seed = SEED;

            expectedKey = (seed ^ XOR_KEY) & 0xFFFFFFFF;

            // Send seed (little-endian)
            Serial.write((uint8_t *)&seed, sizeof(seed));

            // Wait for 4-byte key
            uint8_t keyBytes[4];
            uint32_t timeoutStart = millis();

            while (Serial.available() < 4)
            {
                if (millis() - timeoutStart > 5000)
                {
                    return;
                }
            }

            Serial.readBytes(keyBytes, 4);

            uint32_t receivedKey =
                ((uint32_t)keyBytes[0]) |
                ((uint32_t)keyBytes[1] << 8) |
                ((uint32_t)keyBytes[2] << 16) |
                ((uint32_t)keyBytes[3] << 24);

            if (receivedKey == expectedKey)
            {
                int8_t success = 0;
                Serial.write((uint8_t *)&success, 1);
            }
            else
            {
                int8_t fail = -1;
                Serial.write((uint8_t *)&fail, 1);
            }
        }
    }
} */
