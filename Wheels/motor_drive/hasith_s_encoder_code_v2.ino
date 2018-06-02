/* tiva_encoder.ino
 * reads encoder position and speed
 * outputs data to Serial monitor
 * NOT OPTIMIZED - look for ways to improve performance
 */
 
#define A 5 //Encoder A = Pin 2 PB0
#define B 3 //Encoder B = Pin 3 PB1
#define DIR1 6 //Direction 1 PE5
#define PWM1 7 //PWM1 PB4

//Initialize global variables
//volatile variables can be changed inside interrupts
volatile int a;  //state of encoder signal A
volatile int b;  //state of encoder signal B
volatile long int count;  //Angular Position in encoder counts
volatile long int countOld;  //Previous count (for calculating speed)
volatile long int t;  //Time of measurement (for calculating speed) //ask hasith about overflow problem with time, how to optimize
volatile int tOld; //Time of previous measurement (for calculating speed)
volatile double velocity; // Angular velocity in counts/s
int dir;

//Interrupt service routine for encoder signal A
//Increments or decrements count
//Updates speed
void transitionA()
{
  tOld = t;
  t = micros();
  a = digitalRead(A);
  b = digitalRead(B);
  countOld = count;
  if(!b) { //b = 0
    if(!a) {//a = 0 (old a = 1)
      count--;
      dir = -1;
    }else {//a=1 (old a = 0)
      count++;
      dir = 1;
    }
  }else { //b=1
    if(!a) {//a = 0 
      count++;
      dir = 1;
    } else {//a = 1
      count--;
      dir = -1;
    }
  }
  
  velocity = (double) dir * 750000/(t - tOld); //velocity should update independent of signal changes
}

//Interrupt service routine for encoder signal B
//Increments or decrements count
//Updates speed
void transitionB()
{
  tOld = t;
  t = micros();
  b = digitalRead(B);
  a = digitalRead(A);
  if(!a) { //a = 0
    if(!b) {//b = 0
      count++;
      dir = 1;
    }else {//b = 1
      count--;
      dir = -1;
    }
  } else { //a = 1
    if(!b) {//b = 0
      count--;
      dir = -1;
    }else {//b = 1
      count++;
      dir = 1;
  }
  }
  velocity = (double) dir * 750000/(t - tOld);
}

//Resets count and velocity variables, measures t, a, and b
void resetEncoder()
{
  count = 0;
  countOld = 0;
  velocity = 0;
  t = micros();
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
  
  attachInterrupt(digitalPinToInterrupt(5), transitionA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), transitionB, CHANGE); //could change digitalPinToInterrupt to just pin #(0 and 1), which may or may not be slower
  
  //Reset encoder measurements
  resetEncoder();
  
  //digitalWrite(DIR1, 1);
  //digitalWrite(PWM1, 1);
}

void loop()
{
  //digitalWrite(PWM1, 1);
  //Print encoder count and velocity every 500ms
  Serial.print("TIME: ");
  Serial.print(t);
  Serial.print("\tPOSITION: ");
  Serial.print(count);
  Serial.print("\tRPM: ");
  Serial.println(velocity, 4);
  delay(1000);
  t = micros();
  
  //Serial.print("A: ");
  //Serial.print(a);
  //Serial.print("\tB: ");
  //Serial.println(b);
  
  
}
