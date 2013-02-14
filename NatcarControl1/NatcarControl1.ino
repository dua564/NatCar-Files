                                                                                 
// UCLA NATCAR PROJECT - TEAM DIZHEL ///// ------> VERSION 0.1
// Version Update Information
// 0.1 Set pins, initialize paramaters, line cam acquisition 2/17/13



// Line Camera
#define AOpin A0  // Collect data from Linecam
#define CLKpin 2  // Ctrl Linecam CLOCK
#define SIpin 4   // Ctrl Linecam

#define NPIXELS 128

// Motor Controller 
#define MOTORpin 6  // output PWN to ctrl speed
#define STOPpin 7    // output to ctrl fast stop switch

// Servo
#include <Servo.h> 
#define SERVOpin 9   //output to control servo 
Servo natServo;

const int straight = 71;
const int hardLeft = 98;
const int hardRight = 44;
  // turn range 44 - 98 (right to left)
  // straight 71

///////////////////////////////////////////////////////////////////////
byte Pixel[NPIXELS];

void setup (void)
{
  //SETPUT PINS
       //pinMode (AOpin, INPUT);
       pinMode (SIpin, OUTPUT);
       pinMode (CLKpin, OUTPUT);
       
       pinMode (MOTORpin, OUTPUT);
       pinMode (STOPpin, OUTPUT);
    
       natServo.attach(SERVOpin);  // attaches the servo pin to the servo object, basically pinMode
       natServo.write(71);
   //Initialize Camera Paramters
       digitalWrite (SIpin, LOW);
       digitalWrite (CLKpin, HIGH);

       Serial.begin (115200); 
}


      
int i;
int maxi = 0;
int maxpx = 0;
int motorPWM = 0;
int motorSpeed = 0;
int turnLeft = 71;
int turnRight = 71;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop (void)
{
   maxpx = 0;
   ///// ACQUIRE DATA FROM LINECAM ////////////////////////////////////////
           // SI pulse to capture the image
           digitalWrite (CLKpin, LOW);
           delayMicroseconds (1);
           digitalWrite (SIpin, HIGH);    
           digitalWrite (CLKpin, HIGH);
           digitalWrite (SIpin, LOW);
         
           // pulse CLK and acquire image into Arduino && Look for CENTER pixel
           for (i = 0; i < NPIXELS; i++) {
             Pixel[i] = analogRead (AOpin) / 4;  // analogRead 0 - 1053 && Pixel is byte type 0 - 255
             digitalWrite (CLKpin, LOW);        
             delayMicroseconds (1);
             
             if (Pixel[i] > maxpx) 
             {
                 maxi = i;
             }
             
             digitalWrite (CLKpin, HIGH);
           }
           
           delayMicroseconds (1);
           
             Serial.write ((byte)0);
             for (i = 0; i < NPIXELS; i++) {
               if (Pixel[i] == 0)
                 Serial.write ((byte)1);
               else
                 Serial.write ((byte)Pixel[i]);
             }
          
    ///// CONTROL STEERING //////////////////////////////////////////   
           if (maxi < 61)
           {
                 //turnLeft = map(maxi, 0, 59, 44, 70);
                 natServo.write(hardLeft);
           }
           else if (maxi > 67)  
           {
                 //turnRight = map(maxi, 68, 127, 72, 98); 
                 natServo.write(hardRight);
           }
           else
           {
               natServo.write(straight);
           }
          
    ///// CONTROL MOTOR SPEED/////////////////////////////////////////
           motorPWM = 3;
           motorSpeed = map(motorPWM, 0, 100, 0, 255);
           
           analogWrite(MOTORpin, motorSpeed);

}
