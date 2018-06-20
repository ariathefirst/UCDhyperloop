/* tiva_encoder.ino
 * reads encoder position and speed
 * outputs data to Serial monitor
 * NOT OPTIMIZED - look for ways to improve performance
 */

#include <TimerOne.h>
#include <time.h>

#define CN 80.00 //Number of counts per revolution on the encoder.MAKE SURE IT HAS ENOUGH DIGITS
#define T 0.1 //Velocity calculation time interval in seconds
#define A 5 //Encoder A = Pin 5 PB0
#define B 3 //Encoder B = Pin 3 PB1
#define DIR1 4 //Direction 1 PE5
#define PWM1 6 //PWM1 PB4
#define RAD 3.1 // radius of wheel take wheel radius in cm

//Initialize global variables
//volatile variables can be changed inside interrupts
volatile int a;  //state of encoder signal A
volatile int b;  //state of encoder signal B
volatile long int count;  //Angular Position in encoder counts
volatile long int countOld;  //Previous count (for calculating speed)
volatile double rpm; //Rotations per minute
const double RPMfactor = 60 / (CN * T);
double distance;
double velocity;
int pwmV = 0; //PWM value

//Interrupt service routine for encoder signal A
//Increments or decrements count
void transitionA()
{
  a = digitalRead(A);
  b = digitalRead(B);
  b == 0 ? (a == 0 ? count++ : count--) : (a == 0 ? count-- : count++);
}

//Interrupt service routine for encoder signal B
//Increments or decrements count
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

//Resets count and velocity variables, measures t, a, and b
void resetEncoder()
{
  count = 0;
  countOld = 0;
  rpm = 0;
  distance = 0;
  velocity = 0;
  pwmV = 0;

  a = digitalRead(A);
  b = digitalRead(B);
}

void setup()
{
  //Set encoder pins as inputs
  pinMode(A, INPUT);
  pinMode(B, INPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(PWM1, OUTPUT);
  //Begin serial monitor
  Serial.begin(9600);
  //Set interrupt service routines to run when signals A and B are changed
  attachInterrupt(digitalPinToInterrupt(A), transitionA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(B), transitionB, CHANGE);
  //Set timer interrupt service routines to calculate velocity at certain time intervals
  Timer1.initialize(T*1000000);
  Timer1.attachInterrupt(calcRPM);
  //Initialize PWM and direction
  digitalWrite(DIR1, LOW);
  analogWrite(PWM1, 0);
  //Reset encoder measurements
  resetEncoder();
}

void loop()
{
  if(Serial.available() > 0) {
     pwmV = Serial.parseFloat();
     if(pwmV > 0) {
        digitalWrite(DIR1, LOW);
        if(pwmV > 254){
          pwmV = 254;
        }
     } else if(pwmV < 0) {
        digitalWrite(DIR1, HIGH);
        if(pwmV < -254) {
          pwmV = -254;
        }
     }
  }

  /*if(distance >= 50) {
    while {
      pwmV = pwmV - 10;
      if (pwmV == 0) {
        pwmV = 0; 
        break;
      }
      
    }
  }*/

  analogWrite(PWM1, abs(pwmV));
  
  //Print encoder count and velocity every 500ms
  distance = (2*PI*RAD)*count/(CN*1000.00);
  velocity = (2*PI*RAD)*rpm/6000;
  Serial.print("PWM: ");
  Serial.print(pwmV);
  Serial.print("\tCOUNT: ");
  Serial.print(count);
  Serial.print("\tDISTANCE: ");
  Serial.print(distance);
  Serial.print("\tRPM: ");
  Serial.print(rpm, 4);
  Serial.print("\tVELOCITY: ");
  Serial.println(velocity);
}
