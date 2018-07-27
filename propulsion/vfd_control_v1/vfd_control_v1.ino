
#include <TimerOne.h>
#define CN 1200.00 //Number of counts per revolution on the encoder.MAKE SURE IT HAS ENOUGH DIGITS
#define T 0.500 //Velocity calculation time interval in seconds
#define A 5 //Encoder A = Pin 5 PB0
#define B 3 //Encoder B = Pin 3 PB1

//gear diameters in cm..    original measurements in inch
#define gear_A 3.302 // 1.3"
#define gear_B 1.27 // 0.5"
#define gear_C 3.81 // 1.5"

#define mapFreq 1440.00 // frequency corresponding to max freq, parameter controlled from vfd. make sure matches!
#define maxFreq 60.00 // max freq we can run, controlled from vfd. make sure matches!

//vfd pins for freq
#define outputFreq 7 // pwm pin
#define inputFreq 7 // analog pin
//VFD control pins as of right now we only need S1(run/stop)
#define S1 2
//#define S2 = 3;
//#define S3 = 4;
//#define S4 = 5;
//#define S5 = 6;
//#define S6 = 7;
//#define S7 = 8;

const double RPMfactor = 60.00 / (CN * T);
const double GR_BA = (gear_B/gear_A); // gear ratio of b to a
//Initialize global variables
//volatile variables can be changed inside interrupts
volatile int a;  //state of encoder signal A
volatile int b;  //state of encoder signal B
volatile long int count;  //Angular Position in encoder counts
volatile long int countOld;  //Previous count (for calculating speed)
volatile double rpm; // rotations per minute
int dir;

double distance = 0.0;
double lin_speed = 0.0;
char key;
float frequency;
int sh; // controls whether outputs are shown. not very important can remove





//function prototypes
void showFreq(int frequency);
void outputControl(void);// 
float readVFD(void); // analog input





//Interrupt service routine for encoder signal A
//Increments or decrements count
//Updates speed
void transitionA()
{
  
  a = digitalRead(A);
  b = digitalRead(B);
  if(!b) { //b = 0
    if(!a) { //a = 0 (old a = 1)
      count--;
    }else { //a=1 (old a = 0)
      count++;
    }
  }else { //b=1
    if(!a) { //a = 0 
      count++;
    } else { //a = 1
      count--;
    }
  }
}
//Interrupt service routine for encoder signal B
//Increments or decrements count

void transitionB()
{
 
  b = digitalRead(B);
  a = digitalRead(A);
  if(!a) { //a = 0
    if(!b) {//b = 0
      count++;
    }else {//b = 1
      count--;
    }
  } else { //a = 1
    if(!b) {//b = 0
      count--;
    }else {//b = 1
      count++;
    }
  }
}

void calcRPM() //calculates RPM with count change over time change. time change is constant so we only need to update update countOld = count for varying RPMs
{
  rpm = (double) GR_BA*RPMfactor * (count - countOld);
  countOld = count;
}

//Resets count and velocity variables, measures t, a, and b
void resetEncoder()
{
  count = 0;
  countOld = 0;
  rpm = 0;
 
  a = digitalRead(A);
  b = digitalRead(B);
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
 freq = constrain(freq,0,maxFreq);      
 tone(outputFreq,freq/maxFreq * mapFreq);
 Serial.print(freq);
 Serial.print(" ");
 Serial.println(freq/maxFreq * mapFreq);
 Serial.flush();
}

void showEncoderVal(){
  Serial.print("\tPOSITION: ");
  Serial.print(count);
  Serial.print("\t");
  Serial.print(distance);
  Serial.print("\t");
  Serial.print(lin_speed);
  Serial.print("\tRPM: ");
  Serial.println(rpm, 4);
}

void setup()
{
  //Set encoder pins as inputs
  pinMode(A, INPUT);
  pinMode(B, INPUT);
  pinMode(S1,OUTPUT);
  
  digitalWrite(S1,LOW);
  //creates pullup resistors
  digitalWrite(A,HIGH);
  digitalWrite(B,HIGH);
  
  //Begin serial monitor
  Serial.begin(9600);
  
  //Set interrupt service routines to run when signals A and B are changed
  attachInterrupt(digitalPinToInterrupt(5), transitionA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), transitionB, CHANGE);
  
  //Set timer interrupt service routines to calculate velocity at certain time intervals
  Timer1.initialize(T*1000000);
  Timer1.attachInterrupt(calcRPM);
  
  //Reset encoder measurements
  resetEncoder();

  frequency = readVFD();

}

void loop()
{
  lin_speed = rpm*PI*gear_C/6000.0; // linear speed. divide by 60s*100cm to get get meters/sec
  distance = GR_BA*(PI*gear_C)*(count/CN)/100.00; // distaance travelled from start point. divide by 100 to get meters
  key = NULL;
  
  if(Serial.available() > 0){
    key = Serial.read();
    Serial.print("key == ");
    Serial.println(key);
  }
  
  if(key == 's' && sh == 0){sh = 1;} 
  else if (key == 's' && sh == 1){sh = 0;}
  if(key == 'e' && sh == 0){sh = 2;} 
  else if (key == 'e' && sh == 2){sh = 0;}
  
  switch (key){
    case 0: // nothing pressed
      frequency = readVFD();
      if (sh == 1){showFreq(frequency);}
      if (sh == 2){showEncoderVal();}
      break;
    case '1':
    // if adding other switches check which are toggles or pushes
    // also which can be on at the same time      
      digitalWrite(S1,HIGH);
      Serial.println("on");
      break;
     case '0':
      digitalWrite(S1,LOW);
      // make sure chip and vfd and arduino share ground!
      Serial.println("off");
      Serial.flush();
     break;
    case 'f':      
      sendFreq();
      break;          
  }
  

}
