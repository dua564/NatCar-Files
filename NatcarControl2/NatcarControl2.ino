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
int leftPoint;
int rightPoint;
int midPoint;

byte ch;
byte pixVal; 
byte maxpx;
int maxi = 0;
int motorPWM = 0;
int motorSpeed = 0;
int turnAngle = 71;

byte black=0;
byte white=1;

int onLine=0;

byte MinValueActual;
byte MaxValueActual;



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
//        delayMicroseconds (1);
        
          // find the position of the brightest pixel    ---- should change to better algorithm!!
    
    /*      
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
      */
      
          
//                 --------------Threshold Algorithm----------- v0.1       
             
      MinValueActual = Pixel[0], MaxValueActual = Pixel[0]; //set max & min to some value              
        
      // Find the range of pixel values we are working with in the image
      
  /*
       for (i = 0; i < NPIXELS; i++) 
       {
         pixVal=Pixel[i];
         
         if (pixVal < 0)     // takes care of any random negative values, if they somehow popup
         pixVal += 256;              
       
         if(pixVal > MaxValueActual) { MaxValueActual = pixVal; } //find max
         if(pixVal < MinValueActual) { MinValueActual = pixVal; } //find min
       }     
    
   */
   
       // Determine which pixels are black and which are white.
      
      byte Threshold = 130;
      
      for (i = 0; i < NPIXELS; i++)      
      { 
        pixVal=Pixel[i];
        
        if (pixVal < Threshold)
        Pixel[i]= black;        
        else
        Pixel[i] = white;  
                        
      }     
       
       //Finds the Average of the Signal to determine where the line is. 
       
     for (i = 0; i < NPIXELS; i++)      
      { 
        pixVal=Pixel[i];  
       
         if ((pixVal==white) && (onLine==0))
         {
           leftPoint=i;
           onLine=1;
         }
       
         if ((pixVal==black)&& (onLine==1))
         {
           rightPoint=i;
           onLine=0;             
        }
      }  
      
      midPoint= ((rightPoint+leftPoint)/2);
         
          Serial.write ((byte)0);  // cast 0 to byte type variable, use as STARTING PT 
         for (i = 0; i < NPIXELS; i++) {
           if (Pixel[i] == 0)
             Serial.write ((byte)1);  // change 0s to 1s so not confused w/ STARTING PT
           else
             Serial.write ((byte)Pixel[i]);  // writes bytes to serial port
         } 

    
      
          
    ///// CONTROL STEERING ////////////////////////////////////////// 
    
   
    //Algorithm: Car ONLY turns with respect to brighest pixel
    
          if (midPoint < 61) {
              turnAngle = map(maxi, 0, 60, 44, 70); //Car turns left 
              natServo.write(turnAngle);
          }
          else if (midPoint > 67) {    //Car turns right 
              turnAngle = map(maxi, 68, 127, 72, 98);
              natServo.write(turnAngle);
          }
          else {
              natServo.write(straight); //Straight if between threshold of 61 - 67 
          }



    ///// CONTROL MOTOR SPEED/////////////////////////////////////////
    
    //Algorithm: Car Speed is CONSTANT
    
    
              motorPWM = 12; //Duty Cycle Percentage 
              motorSpeed = map(motorPWM, 0, 100, 0, 255);
              analogWrite(MOTORpin, motorSpeed);
 
}

