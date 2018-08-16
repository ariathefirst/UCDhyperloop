#include <CAN.h>

// map pins
#define FA 2 // Friction Brake A
#define FB 3 // Friction Brake B
#define EA 4 // Eddy Current Brake A
#define EB 5 // Eddy Current Brake B
#define FA_VERIFY 6
#define FB_VERIFY 7
#define EA_VERIFY 8
#define EB_VERIFY 9

int brakeIn;
int frictionACheck;
int frictionBCheck;
int eddyACheck;
int eddyBCheck;
int brakeReturn;
int verifyOutput;

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

void setup() {

  pinMode(FA, OUTPUT);
  pinMode(FB, OUTPUT);
  pinMode(EA, OUTPUT);
  pinMode(EB, OUTPUT);

  pinMode(FA_VERIFY, INPUT);
  pinMode(FB_VERIFY, INPUT);
  pinMode(EA_VERIFY, INPUT);
  pinMode(EB_VERIFY, INPUT);
  
  Serial.begin(9600);
  Serial.println("Input command:");
  Serial.println("0: Actuate Friction Brake A");
  Serial.println("1: Actuate Friction Brake B");
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }

}

void loop() {

  switch(brakeIn)
    {
    case 0:
      Serial.println("Friction Brake A: OFF");
      Serial.println("Eddy Current Brake: OFF");
      digitalWrite(FA, LOW);
      digitalWrite(FB, LOW);
      digitalWrite(EA, LOW);
      digitalWrite(EB, LOW);
      break;
    case 3:
      Serial.println("Friction Brake A: ON");
      Serial.println("Eddy Current Brake: OFF");
      digitalWrite(FA, LOW);
      digitalWrite(FB, LOW);
      digitalWrite(EA, HIGH);
      digitalWrite(EB, HIGH);
      break;
    case 12:
      Serial.println("Friction Brake A: OFF");
      Serial.println("Eddy Current Brake: OFF");
      digitalWrite(FA, HIGH);
      digitalWrite(FB, HIGH);
      digitalWrite(EA, LOW);
      digitalWrite(EB, LOW);
      break;
    case 15:
      Serial.println("Friction Brake A: OFF");
      Serial.println("Eddy Current Brake: OFF");
      digitalWrite(FA, HIGH);
      digitalWrite(FB, HIGH);
      digitalWrite(EA, HIGH);
      digitalWrite(EB, HIGH);
      break;
    default:
      Serial.println("Invalid Command");
      break;
    }

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
    
    verifyOutput = 0;  
  }

}
