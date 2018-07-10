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
  CAN.beginPacket(0x11);
  CAN.write(data);
  CAN.write(c);
  CAN.endPacket();
  }

int canDecoder(int c, int data){
  int val = c*255 + data;
  return val;
}

struct message{
  int value;
  int Time;
  }message;

void sendValue(int id, float data){
  char x[8];
  dtostrf(data,8,3,x);
  CAN.beginPacket(id);
  CAN.write(x,8);
  CAN.endPacket(); 
  Serial.print(x); 
  }
float recieveValue(){
    
  }
  
void loop() {
  // send packet: id is 11 bits, packet can contain up to 8 bytes of data
  Serial.print("Sending packet1 ... ");
  sendValue(0x11,345.5);
  Serial.println("done");delay(500);


}

