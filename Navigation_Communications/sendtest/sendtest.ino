// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <CAN.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("CAN Sender");

  // start the CAN bus at 500 kbps
  while(!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
   }
}
void canEncoder(int data){
  int c = data / 255;
  CAN.beginPacket(0x12);
  CAN.write(data);
  CAN.write(c);
  CAN.endPacket();
  }

int canDecoder(int c, int data){
  int val = c*255 + data;
  return val;
}
  
void loop() {
  // send packet: id is 11 bits, packet can contain up to 8 bytes of data
  Serial.print("Sending packet ... ");

  canEncoder(500);

  Serial.println("done");

  delay(1000);

}

