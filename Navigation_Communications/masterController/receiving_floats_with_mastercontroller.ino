#include <CAN.h>

float velocity;
float distance;

int receivePacketFloat(long id, float *float1, float *float2) { //used to receive floats from the CAN bus
  int packetSize = CAN.parsePacket();

  union data {
    float fl;
    byte bytes[sizeof(fl)];
  };

  union data data1;
  union data data2;

  if(packetSize && !CAN.packetRtr() && CAN.packetId() == id) {
    data1.bytes[0] = CAN.read();
    data1.bytes[1] = CAN.read();
    data1.bytes[2] = CAN.read();
    data1.bytes[3] = CAN.read();
    data2.bytes[0] = CAN.read();
    data2.bytes[1] = CAN.read();
    data2.bytes[2] = CAN.read();
    data2.bytes[3] = CAN.read();

    *float1 = data1.fl;
    *float2 = data2.fl;

    return 1;
  }

  return 0;
}

void setup() {
  Serial.begin(9600); //halt operation until serial connection opens
  while(!Serial);

  if(!CAN.begin(500E3)) { //halt operation if CAN won't start
    Serial.println("ERROR: Starting CAN failed");
    while(1);
  }

}

void loop() {
  if(receivePacketFloat(0x11, &velocity, &distance)){
    Serial.print("Velocity: ");
    Serial.print(velocity);
    Serial.print("          Distance: ");
    Serial.println(distance);
  }
}
