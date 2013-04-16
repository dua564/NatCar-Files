// UCLA NATCAR PROJECT - TEAM DIZHEL ///// ------> VERSION 0.2 ALPHA
// Version Update Information
// 0.1 Set pins, initialize paramaters, line cam acquisition 2/7/13
// 0.2 implement new camera data acquisition algorithm, brightest pixel 2/15/13
// 0.3 max change/derivative alogirithm (sobel operator) for line tracking 3/8/13

// Line Camera
#define AOpin A0  // Collect data from Linecam
#define CLKpin 3  // Ctrl Linecam CLOCK
#define SIpin 5   // Ctrl Linecam

// Motor Controller 
#define MOTORpin 6  // output PWN to ctrl speed
#define STOPpin 7    // output to ctrl fast stop switch

// Servo
#include <Servo.h> 
#define SERVOpin 10   //output to control servo 
Servo natServo;

////////////////////////////////////////////////////////////////////////
const int NPIXELS = 128;
const int memorySize = 10;

const int straight = 71;
const int hardLeft = 98;
const int hardRight = 44;
// turn range 44 - 98 (right to left)
// straight 71

///////////////////////////////////////////////////////////////////////
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

byte Pixel[NPIXELS];    // storage var for incoming data from cam
int history[memorySize] = {71}; // store all 71s in history initially
// assume car on track in beginning

//Motor Control Varaibles
int motorPWM = 0;
int motorSpeed = 0;

//Servo Control Variables
int turnAngle = 71;

//Image Processing Variables
int midPoint=63;
byte pixVal; 
byte maxpx;
int maxi = 0;

int onLine = 0;
byte MinValueActual = 255;
byte MaxValueActual = 0;

int line_center=63;
int line_width=32;

byte pix_threshold=15; 

int kernelSize=3; 
int dataSize=NPIXELS;
byte sobel_output[NPIXELS];
byte sobel_filter[3]={-1,0,1};

int pos_edge=0;
int neg_edge=127; 

/////Second Version (Sobel Oberator) for finding edges//////   
int max_diff = 0;
int min_diff = 0;
int dp=0;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop (void)
{
  // Varaible Resets
  pos_edge=0;
  neg_edge=127;
  max_diff=0; 
  min_diff = 0;
  
  /////////-------- LINESCAN CAM DATA ACQUISITION --------////////////////////////////////////   
      // pulse SI to capture the image
      digitalWrite (CLKpin, LOW);
      delayMicroseconds (1);
      digitalWrite (SIpin, HIGH);
      digitalWrite (CLKpin, HIGH);
      digitalWrite (SIpin, LOW);
    
      // pulse CLK and acquire image into Arduino
      for (int i = 0; i < NPIXELS; i++) {
        Pixel[i] = analogRead (AOpin) / 4;// analogRead 0 - 1053 && Pixel is byte type 0 - 255
        digitalWrite (CLKpin, LOW);
        delayMicroseconds (1);
        digitalWrite (CLKpin, HIGH);
      }
      delayMicroseconds (1);
    
      Serial.write ((byte)0);  // cast 0 to byte type variable, use as STARTING PT 
      for (i = 0; i < NPIXELS; i++) {
        if (Pixel[i] == 0)
          Serial.write ((byte)1);  // change 0s to 1s so not confused w/ STARTING PT
        else
          Serial.write ((byte)Pixel[i]);  // writes bytes to serial port
      }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
 
///////////////////////SOBEL OPERATOR EDGE DETECTION ALGORITHM//////////////////////// v0.1 (CAMERA MUST BE FOCUSED!) (jacob's edition)

for (int i = 1; i < NPIXELS; i++)    // For this to work, we must assume there isnt significant noise that can mimic an edge
{
  dp =  Pixel[i] - Pixel[i-1]; //Finds the change between two adjacent pixels. 
  
  if(dp >= max_diff) //compares if change is greater than max change
  {
     max_diff = dp;
     pos_edge = i;
  }   
  if(dp <= min_diff)  //compares if change is the most negative change 
  {
     min_diff = dp;
     neg_edge = i;
  }  
}

if((neg_edge - pos_edge) < line_width)
    midPoint=(neg_edge+pos_edge)/2;   

 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
/////Debugging Code///
/*
Serial.println(pos_edge);
Serial.println(neg_edge);
*/ 


//////Define Center Of Line//////////   Code is valid for old differentiation theorem
/*
if ((pos_edge_found) && (neg_edge_found))  //Normal Line
{
    midPoint=(neg_edge+pos_edge)/2;   
}

if ((pos_edge_found) && (!neg_edge_found) && (neg_edge-pos_edge>line_width)) //Line is Short Right
{
  midPoint=(neg_edge+pos_edge)/2; 
}

if ((pos_edge_found) && (!neg_edge_found) && (neg_edge-pos_edge<line_width)) //Line is Hard Right
{
    midPoint=neg_edge;    
}
if ((!pos_edge_found) && (neg_edge_found) && (neg_edge-pos_edge>line_width)) //Line is Short Left
{
  midPoint=(neg_edge+pos_edge)/2;
}

if ((!pos_edge_found) && (neg_edge_found) && (neg_edge-pos_edge<line_width)) //Line is Hard Left
{
    midPoint=pos_edge;     
}
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////// ------ CONTROL STEERING --------//////////////////////////////////// 
  

  if (midPoint < 61) {
    turnAngle = map(midPoint, 0, 60, 44, 70); //Car turns left 
    natServo.write(turnAngle);
  }
  else if (midPoint > 67) {    //Car turns right 
    turnAngle = map(midPoint, 68, 127, 72, 98);
    natServo.write(turnAngle);
  }
  else {
    natServo.write(straight); //Straight if between threshold of 61 - 67 
  }
  /////////// ------ CONTROL MOTOR SPEED ------- //////////////////////////////////
  //Algorithm: Car Speed is CONSTANT  

  motorPWM = 12; //Duty Cycle Percentage 
  motorSpeed = map(motorPWM, 0, 100, 0, 255);
  analogWrite(MOTORpin, motorSpeed);

}


