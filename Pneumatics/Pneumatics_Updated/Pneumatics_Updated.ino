/*
Pneumatics software
Controls the actuation of the pneumatics
Currently controlled by Serial commands
Will be updated to be controlled by CAN bus
*/
#include <CAN.h>
// map pins
#define FA 2 // Friction Brake A
#define FB 3 // Friction Brake B
#define EA 4 // Eddy Current Brake A
#define EB 5 // Eddy Current Brake B
#define SW_FA 6 // Friction Brake Limit Switch A
#define SW_FB 7 // Friction Brake Limit Switch B
#define SW_EA 8 // Eddy Current Brake  Limit Switch A
#define SW_EB 9 // Eddy Current Brake Limit Switch B

// encode commands

int FB_LSA=0;
int FB_LSB=0;
int ECB_LSA=0;
int ECB_LSB=0;
char cmd;


void sender(){

  CAN.beginPacket(0x23);
  CAN.write(FB_LSA + '0');
  CAN.write(FB_LSB + '0');
  CAN.write(ECB_LSA + '0');
  CAN.write(ECB_LSB + '0');
  CAN.write('0');
  CAN.write('0');
  CAN.write('0');
  CAN.write('0');
  CAN.endPacket();
  
  }
  

// recieve commands from canbus, recieve on/off and input frequency
char frcBrake;
char ecBrake;
void reciever(){
   char x[1];
   int i = 0;
   int packetSize = CAN.parsePacket();
   for(i=0;i<=5;i++){    
       if(packetSize){
          while (CAN.available()) {       
            for(i=0;i<=0;i++){
              x[i]  =(char)CAN.read();
              Serial.flush();
              
            }
          }
         if(CAN.packetId() == 0x10){cmd = x[0];} 
         
         
       }else{}
  
   }
}

void setup()
{
  // put your setup code here, to run once:
  pinMode(FA, OUTPUT);
  pinMode(FB, OUTPUT);
  pinMode(EA, OUTPUT);
  pinMode(EB, OUTPUT);
  pinMode(SW_FA, INPUT);
  pinMode(SW_FB, INPUT);
  pinMode(SW_EA, INPUT);
  pinMode(SW_EB, INPUT);
  Serial.begin(9600);
  Serial.println("Input command:");
  Serial.println("0: Actuate Friction Brake A");
  Serial.println("1: Actuate Friction Brake B");
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
   
  }
}

void loop()
{
  reciever();
  
  FB_LSA=digitalRead(SW_FA);
  FB_LSB=digitalRead(SW_FB);
  ECB_LSA=digitalRead(SW_EA);
  ECB_LSB=digitalRead(SW_EB);
  sender();
  
  

    
    switch(cmd)
    {
      case 'a':
        Serial.println("Friction Brake A ON");
        digitalWrite(FA, HIGH);
        digitalWrite(FB, HIGH);
        break;
      case 'b':
        Serial.println("Friction Brake A OFF");
        digitalWrite(FA, LOW);
        digitalWrite(FB, LOW);
        break;
      case 'c':
        Serial.println("Eddie Current Brake A ON");
        digitalWrite(EA, HIGH);
        digitalWrite(EB, HIGH);
        break;
      case 'd':
        Serial.println("Eddie Current Brake B OFF");
        digitalWrite(EA, LOW);
        digitalWrite(EB, LOW);
        break;
      case 'e':
        Serial.println("Friction Break Limit Switch A ON");
        digitalWrite(SW_FA, HIGH);
        digitalWrite(SW_FB, HIGH);
        break;
      case 'f':
        Serial.println("Friction Break Limit Switch B OFF");
        digitalWrite(SW_FA, LOW);
        digitalWrite(SW_FB, LOW);
        break;
      case 'g':
        Serial.println("Eddie Current Brake Limit Switch A ON");
        digitalWrite(SW_EA, HIGH);
        digitalWrite(SW_EB, HIGH);
        break;
      case 'h':
        Serial.println("Eddie Current Brake Limit Switch B OFF");
        digitalWrite(SW_EA, LOW);
        digitalWrite(SW_EB, LOW);
        break;  
      default:
        Serial.println("Invalid Command");
    }
    //Serial.println("Input command:");
    //Serial.println("0: Actuate Friction Brake A");
    //Serial.println("1: Actuate Friction Brake B");
  
  Serial.print("Brake state:");
  Serial.println(cmd);
  //Serial.print("Eddy Current Brake state:");
  //Serial.println(ecBrake);
    Serial.print("FB_A State:");
  Serial.println(FB_LSA);
    Serial.print("FB_B state:");
  Serial.println(FB_LSB);
    Serial.print("ECB_A state:");
  Serial.println(ECB_LSA);
    Serial.print("ECB_B state:");
  Serial.println(ECB_LSB);
  delay(10);
  
}
