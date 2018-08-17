#include <CAN.h>

//output pins
#define FA 2 //friction brake a
#define FB 3 //friction brake b
#define EA 4 //eddy current brake a
#define EB 5 //eddy current brake b

//verification pins wired to respective output pins to check output values
#define FA_VERIFY 6
#define FB_VERIFY 7
#define EA_VERIFY 8
#define EB_VERIFY 9

int brakeIn; //input brake configuration
int frictionACheck;
int frictionBCheck;
int eddyACheck;
int eddyBCheck;
int brakeReturn; //brake configuration sent to master controller
int verifyOutput; //enables output pin verification triggered by incoming rtr

//send data to master controller via can bus
void sendData(int data)
{
  int begin;
  int end;
  
  begin = CAN.beginPacket(0x23);
  CAN.write(data);
  end = CAN.endPacket();

  if(begin != 1 || end != 1) { //verify that data is sent correctly
    Serial.println("sendData failed!");
  } else {
    Serial.println("packet sent");
  }
  
}

void receiveData(int *brakeCheck, int *brakeInput)
{
  int packetSize = CAN.parsePacket();
  int i = 0;

  if(packetSize) { //only receive data if it's size is nonzero
    Serial.println("packet received: ");

    if(CAN.packetRtr()) { //check if incoming pack is a rtr to send back brake verification
      Serial.print("brake check rtr");
      *brakeCheck = 1;
      
    } else if(CAN.packetId() == 0x10){ //otherwise check if it's a brake input command
      do {
        *brakeInput = CAN.read();
        if(i<=16) {
          Serial.print(".");
          i++;
        }
      } while(CAN.available() == 0);

      Serial.print("brake input command");
    }
  }
}

void setup()
{
  pinMode(FA, OUTPUT);
  pinMode(FB, OUTPUT);
  pinMode(EA, OUTPUT);
  pinMode(EB, OUTPUT);

  pinMode(FA_VERIFY, INPUT);
  pinMode(FB_VERIFY, INPUT);
  pinMode(EA_VERIFY, INPUT);
  pinMode(EB_VERIFY, INPUT);
  
  Serial.begin(9600);
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }

}

void loop()
{
  receiveData(&verifyOutput, &brakeIn);
  
  /* Brake binary values are encoded into decimal numbers
   * FA FB EA EB
   * 0  0  0  0  = 0 
   * 0  0  0  1  = 1
   * 0  0  1  0  = 2
   * 0  0  1  1  = 3
   * ...
   * 1  1  0  0  = 12
   * ...
   * 1  1  1  1  = 15
   */

  //assign brake outputs based on brake input command
  switch(brakeIn)
    {
    case 0:
      Serial.println("Friction brake: OFF");
      Serial.println("Eddy current brake: OFF");
      digitalWrite(FA, LOW);
      digitalWrite(FB, LOW);
      digitalWrite(EA, LOW);
      digitalWrite(EB, LOW);
      break;
    case 3:
      Serial.println("Friction brake: ON");
      Serial.println("Eddy current brake: ON");
      digitalWrite(FA, LOW);
      digitalWrite(FB, LOW);
      digitalWrite(EA, HIGH);
      digitalWrite(EB, HIGH);
      break;
    case 12:
      Serial.println("Friction brake: ON");
      Serial.println("Eddy current brake: OFF");
      digitalWrite(FA, HIGH);
      digitalWrite(FB, HIGH);
      digitalWrite(EA, LOW);
      digitalWrite(EB, LOW);
      break;
    case 15:
      Serial.println("Friction brake: ON");
      Serial.println("Eddy current brake: ON");
      digitalWrite(FA, HIGH);
      digitalWrite(FB, HIGH);
      digitalWrite(EA, HIGH);
      digitalWrite(EB, HIGH);
      break;
    default:
      Serial.println("Invalid command");
    }

  //sends back output brake configuration value when requested
  if(verifyOutput == 1) {
    if(digitalRead(FA_VERIFY) == HIGH) {
      frictionACheck = 1;  
    } else {
      frictionACheck = 0;
    }
    if(digitalRead(FB_VERIFY) == HIGH) {
      frictionBCheck = 1;
    } else {
      frictionBCheck = 0;
    }
    if(digitalRead(EA_VERIFY) == HIGH){
      eddyACheck = 1;  
    } else {
      eddyACheck = 0;
    }
    if(digitalRead(EB_VERIFY) == HIGH) {
      eddyBCheck = 1; 
    } else {
      eddyBCheck = 0;
    }

    brakeReturn = (frictionACheck * 8) + (frictionBCheck * 4) + (eddyACheck * 2) + (eddyBCheck);
    sendData(brakeReturn);
    verifyOutput = 0;  
  }

}
