#include <CAN.h>


union dataFrame{
  float fl =0;
  uint8_t bytes[8];
  } dataFrame;

union dataFrame dist_p;
union dataFrame dist_w;
union dataFrame vel_p;
union dataFrame vel_w;

//float dist_p =0;  //propulsion distance
//float dist_w =0;  //propulsion distance
//float vel_p=0;  //propulsion distance
//float vel_w=0;  //propulsion distance
char pnmatic_states[4];
int pn_flag;
int cmd;

void setup() {

 Serial.begin(9600);

 while (!Serial);  Serial.println("CAN Sender");  // start the CAN bus at 500 kbps
 while(!CAN.begin(500E3)) {
   Serial.println("Starting CAN failed!");
  }
  Serial.println("Can OK");

}



// function to convert a float into a 8 byte string
// and then send over can bus
void sendValue(int id, float data){

 char y[8];

 dtostrf(data,8,3,y); // convert data from float to char array 

// write char array to the canbus
 CAN.beginPacket(id);
 CAN.write(y,8);
 CAN.endPacket();
 Serial.print(y);

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
       switch(id){
        case 0x21:      
          for(i=0;i<4;i++){dist_w.bytes[i] = (byte) x[i];}
          for(i=0;i<4;i++){vel_w.bytes[i] = (byte) x[i+4];}
          break;
        case 0x22:
          for(i=0;i<4;i++){dist_p.bytes[i] = (byte) x[i];}
          for(i=0;i<4;i++){vel_p.bytes[i] = (byte) x[i+4];}     
          break;
        case 0x23:
          pnmatic_states[0] = x[0]; // FB LS A
          pnmatic_states[1] = x[1]; // FB LS B
          pnmatic_states[2] = x[2]; // EB LS A
          pnmatic_states[3] = x[3]; // EB LS B
          break;
        default:
          break;
       
       }

       

        
        
        /*if(id == 0x21){
     
          for(i=0;i<4;i++){dist_w.bytes[i] = (byte) x[i];}
          for(i=0;i<4;i++){vel_w.bytes[i] = (byte) x[i+4];}
        }
        if(id == 0x22){
          for(i=0;i<4;i++){vel_p.bytes[i] = (byte) x[i+4];}
          for(i=0;i<4;i++){dist_p.bytes[i] = (byte) x[i];}
        }
        if(id == 0x23){
          pnmatic_states[0] = x[0]; // FB LS A
          pnmatic_states[1] = x[1]; // FB LS B
          pnmatic_states[2] = x[2]; // EB LS A
          pnmatic_states[3] = x[3]; // EB LS B
        }*/
      }else{} 
   }
  }
  
//recieve packets from canbus and save them
/*void recieveValue(){
   
   char x[8];
   int i = 0;
   int id;
   unsigned long t = millis();
// check for packets, loop 10 times checking for packets, num is arbitrary 
   for(i; i < 10; i++){
      
      // read the data from the can bus
      if(CAN.parsePacket()){
        id = CAN.packetId();
        while (CAN.available()) {       
          for(i=0;i<=7;i++){
            x[i]  =(char)CAN.read();
            Serial.flush();          
          }
        }
      // sort the data recieved using their IDs            
        if(id == 0x21){dist_w = atof(x);}
        if(id == 0x22){vel_w = atof(x);}
        if(id == 0x33){vel_p = atof(x);}
        if(id == 0x32){dist_p = atof(x);}
        if(id == 0x41){
          pnmatic_states[0] = x[0]; // FB LS A
          pnmatic_states[1] = x[1]; // FB LS B
          pnmatic_states[2] = x[2]; // EB LS A
          pnmatic_states[3] = x[3]; // EB LS B
        }
      }else{} 
   }
}*/

void pn_flagChecker(int flag){

  if (flag == 1){ 
    if(pnmatic_states[0] == '0' && pnmatic_states[1] == '0'){flag = 0; }
    else if (pnmatic_states[0] == '1' && pnmatic_states[1] == '1'){
        CAN.beginPacket(0x10);
        CAN.write('b');
        CAN.endPacket();
    }
  }
  if (flag == 2){ 
    if(pnmatic_states[0] == '1' && pnmatic_states[1] == '1'){flag = 0; }
    else if (pnmatic_states[0] == '0' && pnmatic_states[1] == '0'){
        CAN.beginPacket(0x10);
        CAN.write('a');
        CAN.endPacket();
    }
  }
  if (flag == 3){ 
    if(pnmatic_states[2] == '1' && pnmatic_states[3] == '1'){flag = 0; }
    else if (pnmatic_states[2] == '0' && pnmatic_states[3] == '0'){
        CAN.beginPacket(0x10);
        CAN.write('c');
        CAN.endPacket();
    }
  }
  if (flag == 4){ 
    if(pnmatic_states[2] == '0' && pnmatic_states[3] == '0'){flag = 0; }
    else if (pnmatic_states[2] == '1' && pnmatic_states[3] == '1'){
        CAN.beginPacket(0x10);
        CAN.write('d');
        CAN.endPacket();
    }
  }      
}


void loop() {

 
reciever();


  if(Serial.available()){
    //the case structure to send commands and data to each system
    cmd = Serial.parseInt(); // codes subject to change
   
    switch(cmd){
      case 200: // send speed to wheels system
        sendValue(0x20, Serial.parseFloat());
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
      case 31: // send frequency to propulsion
        sendValue(0x31, Serial.parseFloat());
        break;
      // eddy breaks and friction breaks
      case 412:  
          CAN.beginPacket(0x10);
          CAN.write('c');
          CAN.endPacket();
          pn_flag = 3;
        break;
      case 413:
        CAN.beginPacket(0x10);
        CAN.write('d');
        CAN.endPacket();
        pn_flag =4;
        break;
      case 411:
        CAN.beginPacket(0x10);
        CAN.write('a');
        CAN.endPacket();
        pn_flag =2;
        break;
      case 410:
        CAN.beginPacket(0x10);
        CAN.write('b');
        CAN.endPacket();
        pn_flag = 1;
        break;
       
       default:
        Serial.print("no input");
        break; 
    }  
  

   }
 pn_flagChecker(pn_flag);    
  Serial.print("FB A B ");
  Serial.print(pnmatic_states[0]);Serial.print(pnmatic_states[1]);
  Serial.print(" EB AB"); 
  Serial.print(pnmatic_states[2]);Serial.print(pnmatic_states[3]);
  Serial.print("\tvelw:");
  Serial.print(vel_w.fl);
  Serial.print("\tdistw:");
  Serial.print(dist_w.fl);
   Serial.print("\tvelp:");
   Serial.print(vel_p.fl);
   Serial.print("\tdistp:");
   Serial.println(dist_p.fl);



  }
