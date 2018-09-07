#include <CAN.h>

int num1;
int num2;
int num3;
int num4;

int receivePacketInt(long id, int *int1, int *int2, int *int3, int *int4) { //used to receive floats from the CAN bus
  int packetSize = CAN.parsePacket();

  union data {
    int num;
    byte bytes[sizeof(num)];
  };

  union data data1;
  union data data2;
  union data data3;
  union data data4;

  if(packetSize && !CAN.packetRtr() && CAN.packetId() == id) {
    data1.bytes[0] = CAN.read();
    data1.bytes[1] = CAN.read();
    data2.bytes[0] = CAN.read();
    data2.bytes[1] = CAN.read();
    data3.bytes[0] = CAN.read();
    data3.bytes[1] = CAN.read();
    data4.bytes[0] = CAN.read();
    data4.bytes[1] = CAN.read();

    *int1 = data1.num;
    *int2 = data2.num;
    *int3 = data3.num;
    *int4 = data4.num;

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
  if(receivePacketInt(0x11, &num1, &num2, &num3, &num4)){
    Serial.print(num1);
    Serial.print("  ");
    Serial.print(num2);
    Serial.print("  ");
    Serial.print(num3);
    Serial.print("  ");
    Serial.println(num4);

  }
}
