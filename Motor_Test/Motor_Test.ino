// Motor Controller 
const int MOTORpin = 6;  // output PWN to ctrl speed
int motorPWM = 0;
int motorSpeed = 0;

// nikhil is likes fat chix

void setup()
{
    pinMode (MOTORpin, OUTPUT);
}

void loop()
{
        motorPWM = 10;
     //   motorSpeed = map(motorPWM, 0, 100, 0, 255);
           
        analogWrite(MOTORpin, 10);
}
