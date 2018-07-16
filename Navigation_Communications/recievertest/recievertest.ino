// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <CAN.h>
void receiver(){
   char x[8];
   int i = 0;
   Serial.print("packet with id 0x");
   Serial.print(CAN.packetId(), HEX);
   Serial.print(" ");
   while (CAN.available()) {
      for(i=0;i<=7;i++){x[i]  =(char)CAN.read();}
   }
   Serial.println(atof(x));
   
  
  
  }
void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("CAN Receiver");

  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
  CAN.onReceive(receiver);
  CAN.filter(0x11);
}


void loop() {

}

