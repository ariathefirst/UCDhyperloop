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
#define FA_ON '0'
#define FA_OFF '1'

char cmd;
/*void sender(){
  char x[8];
  CAN.beginPacket(0x32);
  CAN.write(dtostrf(distance,8,3,x),8);
  CAN.endPacket();
  CAN.beginPacket(0x33);
  CAN.write(dtostrf(lin_speed,8,3,x),8);
  CAN.endPacket();
  }
  */

// recieve commands from canbus, recieve on/off and input frequency
char frcBrake;
char ecBrake;
void reciever(){
   char x[1];
   int i = 0;
   int packetSize = CAN.parsePacket();
   if(packetSize){
      while (CAN.available()) {       
        for(i=0;i<=0;i++){
          x[i]  =(char)CAN.read();
          Serial.flush();
          
        }
      }
     if(CAN.packetId() == 0x41){cmd = x[0];} // if pkt ID for frequency, send input frequency to VFD
     //if(CAN.packetId() == 0x40){ecBrake = x[0];}  // if pkt ID for VFD on/off, only take first element of the message x[0]
     
   }else{}
  
  
  }

void setup()
{
  // put your setup code here, to run once:
  pinMode(FA, OUTPUT);
  pinMode(FB, OUTPUT);
  pinMode(EA, OUTPUT);
  pinMode(EB, OUTPUT);
  pinMode(SW_FA, INPUT_PULLUP);
  pinMode(SW_FB, INPUT_PULLUP);
  pinMode(SW_EA, INPUT_PULLUP);
  pinMode(SW_EB, INPUT_PULLUP);
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
  /*
  Serial.print("Limit Switch FA: ");
  Serial.println(digitalRead(SW_FA));
  Serial.print("Limit Switch FB: ");
  Serial.println(digitalRead(SW_FB));
  Serial.print("Limit Switch EA: ");
  Serial.println(digitalRead(SW_EA));
  Serial.print("Limit Switch EB: ");
  Serial.println(digitalRead(SW_EB));
  Serial.println("");
  */
  

    
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
  delay(1000);
  
}
