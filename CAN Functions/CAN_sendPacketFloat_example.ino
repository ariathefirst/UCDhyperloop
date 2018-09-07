#include <CAN.h>

int sendPacketFloat(long id, float float1, float float2) { //used to send floats across the CAN bus
  int begin;
  int end;

  union data {
  float fl;
  byte bytes[sizeof(fl)];
  };

  union data data1;
  union data data2;

  data1.fl = float1;
  data2.fl = float2;
   
  begin = CAN.beginPacket(id);
  CAN.write(data1.bytes[0]);
  CAN.write(data1.bytes[1]);
  CAN.write(data1.bytes[2]);
  CAN.write(data1.bytes[3]);
  CAN.write(data2.bytes[0]);
  CAN.write(data2.bytes[1]);
  CAN.write(data2.bytes[2]);
  CAN.write(data2.bytes[3]);
  end = CAN.endPacket();
  
  if (begin != 1 || end != 1) { //verify that data is sent correctly
    Serial.print("ERROR: Failed to send packet ");
    Serial.println(id);
    return 0;
  } else {
    Serial.print("Packet sent!\n");
    return 1;
  }
}

void setup() {
  Serial.begin(9600); //halt operation until serial connection opens
  while (!Serial);

  if (!CAN.begin(500E3)) { //halt operation if CAN won't start
    Serial.println("ERROR: Starting CAN failed");
    while (1);
  }
}

void loop() {
  float velocity = 859;
  float distance = 43829;
  sendPacketFloat(0x11, velocity, distance);
}

