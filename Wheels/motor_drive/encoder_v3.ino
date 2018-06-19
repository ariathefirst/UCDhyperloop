/* tiva_encoder.ino
 * reads encoder position and speed
 * outputs data to Serial monitor
 * NOT OPTIMIZED - look for ways to improve performance
 */

#include <TimerOne.h>
#define CN 80.00 //Number of counts per revolution on the encoder.MAKE SURE IT HAS ENOUGH DIGITS
#define T 0.1 //Velocity calculation time interval in seconds
#define A 5 //Encoder A = Pin 5 PB0
#define B 3 //Encoder B = Pin 3 PB1
#define DIR1 6 //Direction 1 PE5
#define PWM1 7 //PWM1 PB4
#define RAD 10.16 // radius of wheel in cm

//Initialize global variables
//volatile variables can be changed inside interrupts
volatile int a;  //state of encoder signal A
volatile int b;  //state of encoder signal B
volatile long int count;  //Angular Position in encoder counts
volatile long int countOld;  //Previous count (for calculating speed)
volatile double rpm; // rotations per minute
const double RPMfactor = 60 / (CN * T);
double distance;
double velocity;

//Interrupt service routine for encoder signal A
//Increments or decrements count
//Updates speed
void transitionA()
{
  a = digitalRead(A);
  b = digitalRead(B);
  /*if(!b) { //b = 0
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
  }*/
  b == 0 ? (a == 0 ? count++ : count--) : (a == 0 ? count-- : count++);
}
//Interrupt service routine for encoder signal B
//Increments or decrements count

void transitionB()
{
  b = digitalRead(B);
  a = digitalRead(A);
  /*if(!a) { //a = 0
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
  }*/
  a == 0 ? (b == 0 ? count++ : count--) : (b == 0 ? count-- : count++);
}

void calcRPM() //calculates RPM. time change is constant so we only need to update update countOld = count for varying RPMs
{
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
  //Reset encoder measurements
  resetEncoder();
  //digitalWrite(DIR1, 1);
  //digitalWrite(PWM1, 1);
}

void loop()
{
  //digitalWrite(PWM1, 1);
  //Print encoder count and velocity every 500ms
  distance = (2*PI*RAD)*(count/CN)/100.00;
  velocity = (2*PI*RAD)*rpm/6000;
  Serial.print("COUNT: ");
  Serial.print(count);
  Serial.print("\tDISTANCE: ");
  Serial.print(distance);
  Serial.print("\tRPM: ");
  Serial.print(rpm, 4);
  Serial.print("\tVELOCITY: ");
  Serial.println(velocity);
  //delay(500);
}
