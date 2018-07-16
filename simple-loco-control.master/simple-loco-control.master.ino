/* Copyright 2018 Stefan Daniel Homfeld, www.stedaho.de */

#include <CAN.h>
#include <PinChangeInterruptHandler.h>
#include <RFReceiver.h> /* Siehe https://github.com/zeitgeist87/RFReceiver */

void onReceive(int packetSize);

RFReceiver receiver(3);

/* Zeitstempel für den Blinker */
unsigned long timeLedBlinker = 0;
const long buttonDelay = 5000;
const long blinkerInterval = 500;
bool blinkerState = true;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Master starting up...");

  CAN.setPins(10, 2);
  CAN.setClockFrequency(8E6);
  if (!CAN.begin(250E3)) {
      Serial.println("CAN.begin failed");
  }
  CAN.onReceive(onReceive);

  receiver.begin();

  Serial.println("Master started");
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (receiver.ready()) {
    char msg[4];
    byte senderId = 0;
    byte packageId = 0;
    byte len = receiver.recvPackage((byte *)msg, &senderId, &packageId);
    
    Serial.print("Package-ID: "); Serial.println(packageId);

    if (senderId == 28 && len == 4) {
      if (msg[0] == 28 && msg[3] == 82) {
        sendCan(msg[1], msg[2]);
        Serial.print("Button ");
        Serial.print((int)msg[1]);
        switch (msg[2]) {
          case 0:
            Serial.println(" losgelassen");
            break;
          case 1:
            Serial.println(" gedrueckt");
            break;
        }
      } else {
        Serial.println("Unexpected packet format");
      }
    } else {
      Serial.println("Unexpected packet received");
    }
  }

  /* LED-Status-Blinker */
  if (currentMillis - timeLedBlinker >= blinkerInterval) {
    timeLedBlinker = currentMillis;
    blinkerState = !blinkerState;
    digitalWrite(LED_BUILTIN, blinkerState);
  }
}

void onReceive(int packetSize) {
  // received a packet
  Serial.print("Received ");

  if (CAN.packetExtended() && !CAN.packetRtr()) {
    long id = CAN.packetId();
    int prio = (id >> 25) & 0xF;
    int command = (id >> 17) & 0xFF;
    bool resp = (id >> 16) & 0x1;
    int hash = id & 0xFFFF;
    
    Serial.print("Prio ");
    Serial.print(prio);
    Serial.print(" Command 0x");
    Serial.print(command, HEX);
    Serial.print(" Resp ");
    Serial.print(resp);
    Serial.print(" Hash 0x");
    Serial.print(hash, HEX);

    Serial.print(" Length ");
    Serial.println(packetSize);

    // only print packet data for non-RTR packets
    while (CAN.available()) {
      Serial.print((byte)CAN.read(), HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
}

void sendCan(byte button, byte state) {
  if (button == 0 && state == 1) {    
    CAN.beginExtendedPacket(0x00002B05);
    CAN.write(0);
    CAN.write(0);
    CAN.write(0);
    CAN.write(0);
    CAN.write(1);
    CAN.endPacket();
  }
  if (button == 1 && state == 1) {
    CAN.beginExtendedPacket(0x00002B05);
    CAN.write(0);
    CAN.write(0);
    CAN.write(0);
    CAN.write(0);
    CAN.write(0);
    CAN.endPacket();
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == 's') {
      CAN.beginExtendedPacket(0x00002B05);
      CAN.write(0);
      CAN.write(0);
      CAN.write(0);
      CAN.write(0);
      CAN.write(0);
      CAN.endPacket();
    }
    if (inChar == 'g') {
      CAN.beginExtendedPacket(0x00002B05);
      CAN.write(0);
      CAN.write(0);
      CAN.write(0);
      CAN.write(0);
      CAN.write(1);
      CAN.endPacket();
    }
    if (inChar == '3') {
      CAN.beginExtendedPacket(0x00082B05);
      CAN.write(0);
      CAN.write(0);
      CAN.write(0x40);
      CAN.write(0x06);
      CAN.write(0x02);
      CAN.write(0x40);
      CAN.endPacket();
    }
  }
}

