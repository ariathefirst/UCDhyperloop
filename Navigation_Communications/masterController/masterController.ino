#include <CAN.h>




float dist_p =0;  //propulsion distance
float dist_w =0;  //propulsion distance
float vel_p=0;  //propulsion distance
float vel_w=0;  //propulsion distance

int cmd;

void setup() {

 Serial.begin(9600);

 while (!Serial);  Serial.println("CAN Sender");  // start the CAN bus at 500 kbps
 while(!CAN.begin(500E3)) {
   Serial.println("Starting CAN failed!");
  }
  //CAN.onReceive(recieveValue);
}




void sendValue(int id, float data){

 char y[8];

 dtostrf(data,8,3,y); // convert data from float to char array 

// write char array to the canbus
 CAN.beginPacket(id);
 CAN.write(y,8);
 CAN.endPacket();
 Serial.print(y);

 }



void recieveValue(){
   
   char x[8];
   int i = 0;
   int id;
   for(i=0;i<3;i++){
      if(CAN.parsePacket()){
        id = CAN.packetId();
        while (CAN.available()) {       
          for(i=0;i<=7;i++){
            x[i]  =(char)CAN.read();
            Serial.flush();          
          }
        }
                  
      //if(id == 0x21){dist_w = atof(x);}
      //if(id == 0x22){vel_w = atof(x);}
      if(id == 0x32){vel_p = atof(x);}
      if(id == 0x33){dist_p = atof(x);}
      }else{}  
   }
}




void loop() {

 // send packet: id is 11 bits, packet can contain up to 8 bytes of data

  recieveValue();

  if(Serial.available()){

    cmd = Serial.parseInt();
    switch(cmd){
      case 300:
        CAN.beginPacket(0x30);
        CAN.write('1');
        CAN.endPacket();
        break;
      case 301:
        CAN.beginPacket(0x30);
        CAN.write('0');
        CAN.endPacket();
        break;
      case 31:
        sendValue(0x31, Serial.parseFloat());
        break;
      case 412:
        CAN.beginPacket(0x01);
        CAN.write('c');
        CAN.endPacket();
        break;
      case 413:
        CAN.beginPacket(0x04);
        CAN.write('d');
        CAN.endPacket();
        break;
      case 411:
        CAN.beginPacket(0x04);
        CAN.write('a');
        CAN.endPacket();
        break;
      case 410:
        CAN.beginPacket(0x04);
        CAN.write('b');
        CAN.endPacket();
        break;
       
       default:
        Serial.print("no input");
        break; 
    }  
  
//    Serial.print("Sending packet1 ... ");
//    Serial.println(data);
   }
     
   
   Serial.print("\tvelw:");
   Serial.print(vel_w);
   Serial.print("\tdistw:");
   Serial.print(dist_w);
   Serial.print("\tvelp:");
   Serial.print(vel_p);
   Serial.print("\tdistp:");
   Serial.println(dist_p);



  }
