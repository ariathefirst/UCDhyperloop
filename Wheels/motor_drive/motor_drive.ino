/* tiva_encoder.ino
 * reads encoder position and speed
 * outputs data to Serial monitor
 * NOT OPTIMIZED - look for ways to improve performance
 */
 
#define A 3 //Encoder A = Pin 3 PB0
#define B 4 //Encoder B = Pin 4 PB1
#define DIR1 6 //Direction 1 PE5
#define PWM1 7 //PWM1 PB4

//Initialize global variables
//volatile variables can be changed inside interrupts
volatile int a;  //state of encoder signal A
volatile int b;  //state of encoder signal B
volatile int count;  //Angular Position in encoder counts
volatile int countOld;  //Previous count (for calculating speed)
volatile int t;  //Time of measurement (for calculating speed)
volatile int tOld; //Time of previous measurement (for calculating speed)
volatile double velocity; // Angular velocity in counts/s

//Interrupt service routine for encoder signal A
//Increments or decrements count
//Updates speed
void transitionA()
{
  tOld = t;
  t = micros();
  a = digitalRead(A);
  countOld = count;
  if(!b) {
    if(!a)
      count--;
    else
      count++;
  } else {
    if(!a)
      count++;
    else
      count--;
  }
  
  velocity = (double)1000000/(t - tOld);
}

//Interrupt service routine for encoder signal B
//Increments or decrements count
//Updates speed
void transitionB()
{
  tOld = t;
  t = micros();
  b = digitalRead(B);
  if(!a) {
    if(!b)
      count++;
    else
      count--;
  } else {
    if(!b)
      count--;
    else
      count++;
  }
  
  velocity = (double)1000000/(t - tOld);
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
  attachInterrupt(A, transitionA, CHANGE);
  attachInterrupt(B, transitionB, CHANGE);
  //Reset encoder measurements
  resetEncoder();
  
  digitalWrite(DIR1, 1);
  digitalWrite(PWM1, 1);
}

void loop()
{
  digitalWrite(PWM1, 1);
  
  //Print encoder count and velocity every 500ms
  Serial.print("POSITION: ");
  Serial.print(count);
  Serial.print("\tRPM: ");
  Serial.println(velocity);
  delay(500);
}
