

char key;
float mapFreq = 1440;
float maxFreq = 60.0;
float frequency;
//PIN ASSIGNMENTS

const int outputFreq = 7;
const int inputFreq = 7;
const int S1 = 2;
const int S2 = 3;
const int S3 = 4;
const int S4 = 5;
const int S5 = 6;
const int S6 = 7;
const int S7 = 8;


//function prototypes
void showFreq(int frequency);
void outputControl(void);// 
float readVFD(void); // analog input


void setup() {
  
//control switches
  pinMode(S1,OUTPUT);
/*  pinMode(S2,OUTPUT);
  pinMode(S3,OUTPUT);
  pinMode(S4,OUTPUT);
  pinMode(S5,OUTPUT);
  pinMode(S6,OUTPUT);
*/  digitalWrite(S1,LOW);
//  digitalWrite(S2,LOW);
//  digitalWrite(S3,LOW);
//  digitalWrite(S4,LOW);
//  digitalWrite(S5,LOW);
//  digitalWrite(S6,LOW);
 // pinMode(S7,OUTPUT);

  Serial.begin(9600);
  
  frequency = readVFD(); 

}
int sh = 1;

void loop() {
  // put your main code here, to run repeatedly:
  key = NULL;
  if(Serial.available() > 0){
    key = Serial.read();
    Serial.print("key == ");
    Serial.println(key);
  }
  if(key == 's' && sh == 0){sh = 1;} 
  else if (key == 's' && sh == 1){sh = 0;}
  
    switch (key){
    case 0: // nothing pressed
      frequency = readVFD();
      if (sh == 1){showFreq(frequency);}
      break;
   // case 'l':
   //   outputControl();
   //   break;
    case '1':
      digitalWrite(S2,LOW);
      digitalWrite(S1,HIGH);
      Serial.println("on");
      break;
 /*   case '2':
      digitalWrite(S1,LOW);
      digitalWrite(S2,HIGH);
      //Serial.println("reverse");
      break;
    case '3':
      digitalWrite(S3,HIGH);
      delay(1000);
      digitalWrite(S3,LOW);
      break;      
    case '4':
      digitalWrite(S4,HIGH);
      delay(1000);
      digitalWrite(S4,LOW);
      break; 
    case '5':
      digitalWrite(S6,LOW);
      digitalWrite(S5,HIGH);
      break;
    case '6':
      digitalWrite(S5,LOW);
      digitalWrite(S6,HIGH);
      break; 
 */   case '0':
     digitalWrite(S1,LOW);
     digitalWrite(S2,LOW);
     digitalWrite(S3,LOW);
     digitalWrite(S4,LOW);
     digitalWrite(S5,LOW);
     digitalWrite(S6,LOW);
     // make sure chip and vfd and arduino share ground!
     Serial.println("off");
     Serial.flush();
     break;
    case 'f':
      
      sendFreq();
      break; 
    case 'e':
      tone(outputFreq,1440);          
  }
}

//read VFD analog output and return frequency
float readVFD(void){
  int sum = 0;
  float vout;
  // magic numbers - needs calibration
  float k = 1; 
  float c = 0;
  
  
  analogRead(inputFreq); // discard first sample
//  for (int i = 0; i<8; ++i){
//    sum += analogRead(freqSignal);
//  }
//  sum = sum >> 3; // take average
//  vout = sum * 5.0 / 1024; // convert to voltage
  vout = analogRead(inputFreq);
  vout  = (vout/1024.0)*5;
  //Serial.println(vout);
  frequency = (vout/5.0)*maxFreq;
  frequency = k* frequency - c;
  frequency = (int)(frequency + 0.5); // round to integer value
  //Serial.println(frequency);
  return frequency;  
}

void showFreq(int frequency){
  Serial.print("Freq ==   ");
  Serial.println(frequency);
  }


void sendFreq(){
  //vfd para B101=4
  //         B102=1
  //         H602 -> 100% freq at this freq you are at the max set freq
  //        control of freq is done by changing the pulse freq
 

 int freq;
 Serial.print("Enter Frequency: ");
 while(!Serial.available()){ }
 freq = Serial.parseInt();      
 tone(outputFreq,freq/maxFreq * mapFreq);
 Serial.print(freq);
 Serial.print(" ");
 Serial.println(freq/maxFreq * mapFreq);
 Serial.flush();
}


