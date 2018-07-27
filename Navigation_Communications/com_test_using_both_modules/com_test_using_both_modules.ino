#include <CAN.h>
char pnmatic_states[4];
void setup() {

 Serial.begin(9600);
 //digitalWrite(2,HIGH); 
 while (!Serial);  Serial.println("CAN Sender");  // start the CAN bus at 500 kbps
 while(!CAN.begin(500E3)) {
   Serial.println("Starting CAN failed!");
  }
 // CAN.setPins(3,4);
  Serial.print("ok");
}

void reciever(){
  char x[8];
  int i;
  int id;
  for(i; i < 10; i++){
      
      // read the data from the can bus
      if(CAN.parsePacket()){
        id = CAN.packetId();
        while (CAN.available()) {       
          for(i=0;i<=7;i++){
            x[i]  =(char) CAN.read();
           
            Serial.flush();          
          }
        }
       
      // sort the data recieved using their IDs            
        
        if(id == 0x41){
          pnmatic_states[0] = x[0]; // FB LS A
          pnmatic_states[1] = x[1]; // FB LS B
          pnmatic_states[2] = x[2]; // EB LS A
          pnmatic_states[3] = x[3]; // EB LS B
        }
      }else{} 
   }
  }

int cmd;
void loop() {

  reciever();
 
    if(Serial.available()){
    //the case structure to send commands and data to each system
    cmd = Serial.parseInt(); // codes subject to change
   
    switch(cmd){
      case 1:
        digitalWrite(2,LOW);
        break;
      case 2:
        digitalWrite(2,HIGH);
        break;

      case 300: // propulsion VFD on
        CAN.beginPacket(0x30);
        CAN.write('1');
        CAN.endPacket();
        break;
      case 301: // propulsion VFD off
        CAN.beginPacket(0x30);
        CAN.write('0');
        CAN.endPacket();
        break;
 
      // eddy breaks and friction breaks
      case 412:  
        CAN.beginPacket(0x40);
        CAN.write('c');
        CAN.endPacket();
        break;
      case 413:
        CAN.beginPacket(0x40);
        CAN.write('d');
        CAN.endPacket();
        break;
      case 411:
        CAN.beginPacket(0x40);
        CAN.write('a');
        CAN.endPacket();
        break;
      case 410:
        CAN.beginPacket(0x40);
        CAN.write('b');
        CAN.endPacket();
        break;
       
       default:
        Serial.print("no input");
        break; 
    }  
  }
  Serial.print("FB A B ");
  Serial.print(pnmatic_states[0]);Serial.print(pnmatic_states[1]);
  Serial.print(" EB AB"); 
  Serial.print(pnmatic_states[2]);Serial.println(pnmatic_states[3]);
 
 
}
