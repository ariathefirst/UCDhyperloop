/* tiva_encoder.ino
 * reads encoder position and speed
 * outputs data to Serial monitor
 * sends input PWM to motor
 * NOT OPTIMIZED - look for ways to improve performance
 */

#include <TimerOne.h>
#include <time.h>

#define CN 40.00 //counts per revolution on the encoder. used to be 80, but answers were halved. make sure it goes to hundredths place.
#define T 0.1 //velocity calculation time interval in seconds
#define A 5 //encoder A: pin 5
#define B 3 //encoder B: pin 3
#define DIR1 4 //direction 1: pin 4
#define PWM1 6 //PWM 1: pin 6
#define RAD 10.16 //radius of wheel in cm
#define CT 100 //time to change between new input PWMs (no units)

//initialize global variables
//volatile variables can be changed inside interrupts
volatile int a;  //state of encoder signal A
volatile int b;  //state of encoder signal B
volatile long int count;  //angular Position in encoder counts
volatile long int countOld;  //previous count (for calculating speed)
volatile double rpm; //rotations per minute
const double RPMfactor = 60 / (CN * T); //part of RPM calculation
double distance; //distance traveled in meters
double velocity; //velocity in meters/sec
int pwmIn = 0; //PWM input value
int pwmOld = 0; //store previous PWM for rate of change
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
void calcRPM() {
  rpm = (double) RPMfactor * (count - countOld);
  countOld = count;
}

//resets count and velocity variables, measures t, a, and b
void resetEncoder()
{
  count = 0;
  countOld = 0;
  rpm = 0;
  distance = 0;
  velocity = 0;
  pwmIn = 0;

  a = digitalRead(A);
  b = digitalRead(B);
}

//takes new PWM input and changes to it linearly
float changePWM(float pwmCurrent, int pwmBegin, int pwmEnd, float t) {
  float pwmTrans;

  if(pwmEnd > pwmBegin) {
    if(pwmCurrent < pwmEnd) {
      pwmTrans = pwmCurrent - (pwmBegin - pwmEnd)/t; 
    } else {
      pwmTrans = pwmEnd;
    }
  } else if(pwmEnd < pwmBegin) {
    if (pwmCurrent > pwmEnd) {
      pwmTrans = pwmCurrent - (pwmBegin - pwmEnd)/t;
    } else {
      pwmTrans = pwmEnd;
    }
  }
  return pwmTrans;
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
  Timer1.initialize(T*1000000);
  Timer1.attachInterrupt(calcRPM);
  
  //initialize PWM and direction. **should this get put in resetEncoder function?
  digitalWrite(DIR1, LOW);
  analogWrite(PWM1, 0);
  
  //reset encoder measurements
  resetEncoder();
}

void loop()
{
  //take PWM input
  if(Serial.available() > 0) {
     pwmOld = pwmWrite;
     pwmIn = Serial.parseFloat();
     pwmIn = constrain(pwmIn, -254, 254); //255 and -255 wouldn't run at full speed for some reason
  }

  
  //linear PWM change to input PWM value
  pwmWrite = changePWM(pwmWrite, pwmOld, pwmIn, CT);
  pwmWrite = constrain(pwmWrite, -254, 254); //255 and -255 wouldn't run at full speed for some reason

  //set motor direction on DIR1 output pin
  if(pwmWrite > 0) {
    digitalWrite(DIR1, LOW);
  } else if(pwmWrite < 0) {
    digitalWrite(DIR1, HIGH);
  }

  //write PWM value to PWM1 output pin
  analogWrite(PWM1, abs(pwmWrite));

  distance = (2*PI*RAD)*(count/CN)/100.00;
  velocity = (2*PI*RAD)*rpm/6000;
  
  //print relevant parameters
  Serial.print("PWM: ");
  Serial.print(pwmWrite);
  Serial.print("\tCOUNT: ");
  Serial.print(count);
  Serial.print("\tDISTANCE: ");
  Serial.print(distance);
  Serial.print("\tRPM: ");
  Serial.print(rpm, 4);
  Serial.print("\tVELOCITY: ");
  Serial.println(velocity);
}
