#define in1 4
#define pwm1 6
float pwmV;
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
digitalWrite(in1,HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
   if(Serial.available() > 0){
    pwmV = Serial.parseFloat();
    Serial.print("val == ");
    Serial.println(pwmV);
  }
  analogWrite(pwm1,pwmV);
}
