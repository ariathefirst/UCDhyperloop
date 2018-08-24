#include <CAN.h>

int pneumaticInput;
int pneumaticInputOld = -1;
int rtr;
int packetSize;
int serialBuffer;
int brakeCheck;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
}

void loop() {
  if(Serial.available() > 0) {
    serialBuffer = Serial.parseInt();
    if(serialBuffer <= 15 && serialBuffer >= 0) {
      pneumaticInput = serialBuffer;
    } else {
      rtr = 1;
    }
  }

  if(pneumaticInput != pneumaticInputOld) {
    CAN.beginPacket(0x10);
    CAN.write(pneumaticInput);
    CAN.endPacket();
    Serial.println("Packet sent");
  }

  pneumaticInputOld = pneumaticInput;

  if(rtr == 1) {
    CAN.beginPacket(0x23, 1, true);
    CAN.endPacket();
    Serial.println("RTR sent");
  }
  
  rtr = 0;

  packetSize = CAN.parsePacket();
  if(packetSize) {
    Serial.print("Packet received: ");

    if(CAN.packetId() == 0x23) {
      Serial.println("Brake verification");
      if(CAN.available() > 0 && CAN.peek() != -1) {
        brakeCheck = CAN.read();
        Serial.println(brakeCheck);
      }
    }
  }
}
