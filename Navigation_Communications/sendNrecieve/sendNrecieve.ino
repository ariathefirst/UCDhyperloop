#include <CAN.h>

void setup() {

 Serial.begin(9600);

 while (!Serial);  Serial.println("CAN Sender");  // start the CAN bus at 500 kbps

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

 }int canDecoder(int c, int data){

 int val = c*255 + data;

 return val;

}struct message{

 int value;

 int Time;

 }message;void sendValue(int id, float data){

 char x[8];

 dtostrf(data,8,3,x);

 CAN.beginPacket(id);

 CAN.write(x,8);

 CAN.endPacket();

 Serial.print(x);

 }

char dist[8];

char vel[8];

float recieveValue(){

   char x[8];

   int i=0;

   int pkt;

   if(CAN.parsePacket()){

     while(CAN.available()){

        if(pkt == 0x21){for(i=0;i<=7;i++){dist[i] = (char)CAN.read();}}

        if(pkt == 0x22){for(i=0;i<=7;i++){vel[i] = (char)CAN.read();}}

      }

   }    

     Serial.print(dist);

     Serial.print("\tvel");

     Serial.println(vel);

 }

 float data;

void loop() {

 // send packet: id is 11 bits, packet can contain up to 8 bytes of data

 recieveValue();

 if(Serial.available()){

   data = Serial.parseFloat();

   sendValue(0x20,data);

   Serial.print("Sending packet1 ... ");

   Serial.println(data);

   }



 

delay(500);}

