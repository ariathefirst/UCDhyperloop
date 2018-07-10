/* motor_driver_PID.ino
 * reads encoder position and speed
 * outputs data to serial monitor
 * takes velocity input and changes to it linearly
 * NOT OPTIMIZED - look for ways to improve performance
 */

#include <TimerOne.h>
#include <time.h>
#include <CAN.h>

#define A 5 //encoder A: pin 5
#define B 3 //encoder B: pin 3
#define DIR1 4 //direction 1: pin 4
#define PWM1 6 //PWM 1: pin 6

#define CPR 40.00 //counts per revolution on the encoder
//used to be 80, but answers were halved
//needs to have to digits past decimal to prevent rounding error
#define RAD 10.16 //radius of wheel in cm
#define VCT 0.1 //velocity calculation time interval in seconds
#define VIT 100 //time change interval between new target velocity (no units)

#define ESM 240 //max error sum for PID calculation (varies with Ki factor)
//PID constants. NEED TO BE TUNED MORE PRECISELY.
#define Kp 5  //proportional gain factor for PID feedback
#define Ki 1 //integral gain factor for PID feedback
#define Kd -1 //derivative gain factor for PID feedback //not sure why negative worked better

//initialize global variables
//volatile variables can be changed inside interrupts
volatile int a;  //state of encoder signal A
volatile int b;  //state of encoder signal B
volatile long int count;  //angular Position in encoder counts
volatile long int countOld;  //previous count (for calculating speed)
volatile double rpm; //rotations per minute
const double rpmFactor = 60 / (CPR * VCT); //part of RPM calculation
double distance = 0; //distance traveled in meters
float velocityIn = 0; //desired velocity input in meters/sec
volatile float velocityOut; //velocity read in meters/sec
float velocityTarget = 0; //what the veliocity should be (used for slow v transitions)
float velocityOld = 0; //previous value of velocityTarget
float errorSum = 0;
float errorOld = 0;
float pwmWrite = 0; //PWM value sent to the motor

//interrupt service routine for encoder signal A
//increments or decrements count
void transitionA()
{
  a = digitalRead(A);
  b = digitalRead(B);
  b == 0 ? (a == 0 ? count++ : count--) : (a == 0 ? count-- : count++);
}

//interrupt service routine for encoder signal B
//increments or decrements count
void transitionB()
{
  b = digitalRead(B);
  a = digitalRead(A);
  a == 0 ? (b == 0 ? count++ : count--) : (b == 0 ? count-- : count++);
}

//calculates RPM with count change over constant time change 
//**need to stick this in the main loop for PID. Need to include delta time.
void calcSpeed() {
  rpm = (double) rpmFactor * (count - countOld);
  velocityOut = (2*PI*RAD)*rpm/6000;
  countOld = count; 
}

//resets count and velocity variables, measures t, a, and b
void resetEncoder()
{
  count = 0;
  countOld = 0;
  rpm = 0;
  distance = 0;
  velocityOut = 0;

  a = digitalRead(A);
  b = digitalRead(B);
}

//takes new PWM input and changes to it linearly over time t
float changeVelocity(float vCurrent, int vBegin, int vEnd, float t)
{
  float vNext;

  if(vEnd > vBegin) {
    if(vCurrent < vEnd) {
      vNext = vCurrent - (vBegin - vEnd)/t; 
    } else {
      vNext = vEnd;
    }
  } else if(vEnd < vBegin) {
    if (vCurrent > vEnd) {
      vNext = vCurrent - (vBegin - vEnd)/t;
    } else {
      vNext = vEnd;
    }
  }
  return vNext;
}

float pidCalculate(float *errorSum, float velocityTarget, float velocityOut, float *errorOld) //feedback loop for inputing v instead of PWM
{
  float error;
  float pwmOut;
  
  error = velocityTarget - velocityOut;
  *errorSum = *errorSum + error;
  if(*errorSum > ESM || *errorSum < -ESM){
    Serial.print("MAX ERROR SUM\t");
  }
  *errorSum = constrain(*errorSum, -ESM, ESM);
  pwmOut = (Kp * error) + (Ki * *errorSum) + (Kd * (error - *errorOld));
  if(velocityTarget == 0) {
    pwmOut = 0;
  }
  *errorOld = error;
  return pwmOut;
}
void receiver(){
   char x[8];
   int i = 0;
   Serial.print("packet with id 0x");
   Serial.print(CAN.packetId(), HEX);
   Serial.print(" ");
   while (CAN.available()) {
      for(i=0;i<=7;i++){x[i]  =(char)CAN.read();}
   }
   velocityOld = velocityTarget;
   velocityIn = atof(x);
   
}
void setup()
{
  //set encoder pins as inputs
  pinMode(A, INPUT);
  pinMode(B, INPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(PWM1, OUTPUT);
  
  //begin serial monitor
  Serial.begin(9600);
  
  //set interrupt service routines to run when signals A and B are changed
  attachInterrupt(digitalPinToInterrupt(A), transitionA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(B), transitionB, CHANGE);
  
  //set timer interrupt service routines to calculate velocity at certain time intervals
  Timer1.initialize(VCT*1000000);
  Timer1.attachInterrupt(calcSpeed);
  
  //initialize PWM and direction. **should this get put in resetEncoder function?
  digitalWrite(DIR1, LOW);
  analogWrite(PWM1, 0);
  
  //reset encoder measurements
  resetEncoder();
  CAN.onReceive(receiver);
}

void loop()
{
  
  //linear velocity change to input velocity value
  if(velocityIn != velocityTarget) {
    velocityTarget = changeVelocity(velocityTarget, velocityOld, velocityIn, VIT);
  }

  
  pwmWrite = pidCalculate(&errorSum, velocityTarget, velocityOut, &errorOld);
  pwmWrite = constrain(pwmWrite, -254, 254); //255 and -255 are bugged and won't run the motor at max speed

  //set motor direction on DIR1 output pin
  if(pwmWrite > 0) {
    digitalWrite(DIR1, LOW);
  } else if(pwmWrite < 0) {
    digitalWrite(DIR1, HIGH);
  }

  //write PWM value to PWM1 output pin
  analogWrite(PWM1, abs(pwmWrite));

  distance = (2*PI*RAD)*(count/CPR)/100.00;
  
  //print relevant parameters
  Serial.print("PWM: ");
  Serial.print(pwmWrite);
  //Serial.print("\tCOUNT: ");
  //Serial.print(count);
  Serial.print("\tDISTANCE: ");
  Serial.print(distance);
  //Serial.print("\tRPM: ");
  //Serial.print(rpm, 4);
  Serial.print("\tTARGET VELOCITY: ");
  Serial.print(velocityTarget);
  Serial.print("\tVELOCITY OUT: ");
  Serial.println(velocityOut);
}
