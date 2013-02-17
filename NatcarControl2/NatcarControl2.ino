// UCLA NATCAR PROJECT - TEAM DIZHEL ///// ------> VERSION 0.2 ALPHA
// Version Update Information
// 0.1 Set pins, initialize paramaters, line cam acquisition 2/7/13
// 0.2 implement new camera data acquisition algorithm, brightest pixel 2/15/13


// Line Camera
#define AOpin A0  // Collect data from Linecam
#define CLKpin 2  // Ctrl Linecam CLOCK
#define SIpin 4   // Ctrl Linecam

// Motor Controller 
#define MOTORpin 6  // output PWN to ctrl speed
#define STOPpin 7    // output to ctrl fast stop switch

// Servo
#include <Servo.h> 
#define SERVOpin 9   //output to control servo 
Servo natServo;

////////////////////////////////////////////////////////////////////////
const int NPIXELS = 128;  
const int straight = 71;
const int hardLeft = 98;
const int hardRight = 44;
const int memorySize = 5;
// turn range 44 - 98 (right to left)
// straight 71

///////////////////////////////////////////////////////////////////////
byte Pixel[NPIXELS];    // storage var for incoming data from cam
int history[memorySize] = { 71 }; // store all 71s in history initially
                                    // assume car on track in beginning
                                    
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
byte ch;
byte maxpx;
int maxi = 0;
int motorPWM = 0;
int motorSpeed = 0;
int turnAngle = 71;

int debug[memorySize];

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop (void)
{
    ///// LINESCAN CAM DATA ACQUISITION //////////////////////////////////////////   
          maxpx = (byte)0;  // reset maxpx to 0 every time
          
          // pulse SI to capture the image
          digitalWrite (CLKpin, LOW);
          delayMicroseconds (1);
          digitalWrite (SIpin, HIGH);
          digitalWrite (CLKpin, HIGH);
          digitalWrite (SIpin, LOW);
  
          // pulse CLK and acquire image into Arduino
          for (i = 0; i < NPIXELS; i++) {
            Pixel[i] = analogRead (AOpin) / 4;// analogRead 0 - 1053 && Pixel is byte type 0 - 255
            digitalWrite (CLKpin, LOW);
            delayMicroseconds (1);
            digitalWrite (CLKpin, HIGH);
          }
//          delayMicroseconds (1);
        
          // find the position of the brightest pixel    ---- should change to better algorithm!!
          for (i = 0; i < NPIXELS; i++) 
          {
            ch = Pixel[i];
            if (ch < 0)     // takes care of any random negative values, if they somehow popup
              ch += 256;
            if (ch > maxpx) {   // Look for brightest pixel
              maxi = i;
              maxpx = ch;
            }
          }          
         
          Serial.write ((byte)0);  // cast 0 to byte type variable, use as STARTING PT 
         for (i = 0; i < NPIXELS; i++) {
           if (Pixel[i] == 0)
             Serial.write ((byte)1);  // change 0s to 1s so not confused w/ STARTING PT
           else
             Serial.write ((byte)Pixel[i]);  // writes bytes to serial port
         } 

//          if(  abs(maxi - history[memorySize-1]) < 15)    // try to account for sharp turns
//          {                                               // ignore new brightest if change is too large
//                
//                // debug
//                for(i = 0; i < memorySize; i++){
//                    debug[i] = int(history[i]);
//                    Serial.print(debug[i]);
//                    Serial.print("\t"); 
//                }
//                Serial.println("");
//                
//                for(i = 1; i < memorySize; i++){
//                history[i - 1] = history[i];
//            }
//            history[memorySize - 1] = maxi;
//          }
//          else
//            maxi = history[memorySize - 1];
   
      
      
      
      
          
    ///// CONTROL STEERING ////////////////////////////////////////// 
          if (maxi < 61) {
              turnAngle = map(maxi, 0, 60, 44, 70);
              natServo.write(turnAngle);
          }
          else if (maxi > 67) {
              turnAngle = map(maxi, 68, 127, 72, 98);
              natServo.write(turnAngle);
          }
          else {
              natServo.write(straight);
          }

    ///// CONTROL MOTOR SPEED/////////////////////////////////////////
              motorPWM = 12;
              motorSpeed = map(motorPWM, 0, 100, 0, 255);
              analogWrite(MOTORpin, motorSpeed);
 
}

