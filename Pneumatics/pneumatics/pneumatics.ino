/*
Pneumatics software
Controls the actuation of the pneumatics
Currently controlled by Serial commands
Will be updated to be controlled by CAN bus
*/

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

int cmd;

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
}

void loop()
{
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
  
  if(Serial.available() > 0)
  {
    cmd = Serial.read();
    Serial.print("CMD: ");
    
    switch(cmd)
    {
      case FA_ON:
        Serial.println("Friction Brake A ON");
        break;
      case FA_OFF:
        Serial.println("Friction Brake A OFF");
        break;
      default:
        Serial.println("Invalid Command");
    }
    Serial.println("Input command:");
    Serial.println("0: Actuate Friction Brake A");
    Serial.println("1: Actuate Friction Brake B");
  }
  
  delay(1000);
}
