#include <CAN.h>

int sendPacketFloat(long id, int int1, int int2, int int3, int int4) { //used to send floats across the CAN bus
  int begin;
  int end;

  union data {
  //float fl;
  //byte bytes[sizeof(fl)];
  int num;
  byte bytes[sizeof(num)];
  };

  union data data1;
  union data data2;
  union data data3;
  union data data4;

  data1.num = int1;
  data2.num = int2;
  data3.num = int3;
  data4.num = int4;
   
  begin = CAN.beginPacket(id);
  CAN.write(data1.bytes[0]);
  CAN.write(data1.bytes[1]);
  CAN.write(data2.bytes[0]);
  CAN.write(data2.bytes[1]);
  CAN.write(data3.bytes[0]);
  CAN.write(data3.bytes[1]);
  CAN.write(data4.bytes[0]);
  CAN.write(data4.bytes[1]);
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
  int velocity      = 800;
  int distance      = 390;
  int acceleration  = 03;
  int luckyNumber   = 23;
  sendPacketFloat(0x11, velocity, distance, acceleration, luckyNumber);
}

