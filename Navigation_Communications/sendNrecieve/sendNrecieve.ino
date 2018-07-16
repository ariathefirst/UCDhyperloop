#include <CAN.h>

char dist[8];

char vel[8];

void setup() {

 Serial.begin(9600);

 while (!Serial);  Serial.println("CAN Sender");  // start the CAN bus at 500 kbps

 while(!CAN.begin(250E3)) {

   Serial.println("Starting CAN failed!");

  }

}




void sendValue(int id, float data){

 char y[8];

 dtostrf(data,8,3,y);

 CAN.beginPacket(id);

 CAN.write(y,8);

 CAN.endPacket();

 Serial.print(y);

 }



float recieveValue(){

   char y[8];

   int i=0;

 
 if(CAN.parsePacket()){
  if(CAN.packetId() == 0x21){
   
   while (CAN.available()) {
      for(i=0;i<=7;i++){dist[i]  =(char)CAN.read();}
   }
   
  }
 }
 
  if(CAN.parsePacket()){
    if(CAN.packetId() == 0x22){
  
      while (CAN.available()) {
        for(i=0;i<=7;i++){vel[i]  =(char)CAN.read();}
      }
    }
 }
}

 float data;

void loop() {

 // send packet: id is 11 bits, packet can contain up to 8 bytes of data

 recieveValue();

 if(Serial.available()){

   data = Serial.parseFloat();

   sendValue(0x20,data);

   Serial.print("Sending packet1 ... ");

   Serial.println(data);}
     
   
   Serial.print("vel:");
   Serial.println(vel);
   Serial.print("dist:");
   Serial.print(dist);


  }
