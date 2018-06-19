/* tiva_encoder.ino
 * reads encoder position and speed
 * outputs data to Serial monitor
 * NOT OPTIMIZED - look for ways to improve performance
 */

#include <TimerOne.h>
#define CN 2400.00 //Number of counts per revolution on the encoder.MAKE SURE IT HAS ENOUGH DIGITS
#define T 0.500 //Velocity calculation time interval in seconds
#define A 5 //Encoder A = Pin 5 PB0
#define B 3 //Encoder B = Pin 3 PB1

//gear diameters in cm..    original measurements in inch
#define gear_A 3.302 // 1.3"
#define gear_B 1.27 // 0.5"
#define gear_C 3.81 // 1.5"

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

void setup()
{
  //Set encoder pins as inputs
  pinMode(A, INPUT);
  pinMode(B, INPUT);
  

  
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

}

void loop()
{
  lin_speed = rpm*PI*gear_C/6000.0; // linear speed. divide by 60s*100cm to get get meters/sec
  distance = GR_BA*(PI*gear_C)*(count/CN)/100.00; // distaance travelled from start point. divide by 100 to get meters
  Serial.print("\tPOSITION: ");
  Serial.print(count);
  Serial.print("\t");
  Serial.print(distance);
  Serial.print("\t");
  Serial.print(lin_speed);
  Serial.print("\tRPM: ");
  Serial.println(rpm, 4);

}
