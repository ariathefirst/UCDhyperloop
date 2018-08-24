#include <CAN.h>

int pneumaticInput;
int pneumaticInputOld = -1;

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
    pneumaticInput = Serial.parseInt();
  }

  if(pneumaticInput != pneumaticInputOld) {
    CAN.beginPacket(0x10);
    CAN.write(pneumaticInput)
    CAN.endPacket;
    Serial.println("Packet sent");
  }

  pneumaticInputOld = pneumaticInput

  

  delay(1000);
}
