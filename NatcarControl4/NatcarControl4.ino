// UCLA NATCAR PROJECT - TEAM DIZHEL ///// ------> VERSION 0.2 ALPHA
// Version Update Information
// 0.1 Set pins, initialize paramaters, line cam acquisition 2/7/13
// 0.2 implement new camera data acquisition algorithm, brightest pixel 2/15/13
// 0.3 max change/derivative alogirithm (sobel operator) for line tracking 3/8/13
// 0.4 refined sobel operator & try to account for intersections 3/29/13


// Line Camera
    #define AOpin A0  // Collect data from Linecam
    const int CLKpin = 3;  // Ctrl Linecam CLOCK
    const int SIpin = 5;   // Ctrl Linecam

// Motor Controller 
    const int MOTORpin = 6;   // output PWN to ctrl speed
    const int STOPpin = 7;    // output to ctrl fast stop switch

// Servo
    #include <Servo.h>
    Servo natServo;
    const int SERVOpin = 10;   //output to control servo 


// CONST PARAMETERS /////////////////////////////////////////////////////////////////////
    const int NPIXELS = 128;  // number of pixels "seen" by line cam
    
    const int straight = 71;  // turn range 44 - 98 (Right to Left)
    const int hardLeft = 98;  // straight is about 71
    const int hardRight = 44;

// SETUP /////////////////////////////////////////////////////////////////////////////////
    void setup (void)
    {
      pinMode (AOpin, INPUT);    
      pinMode (SIpin, OUTPUT);
      pinMode (CLKpin, OUTPUT);
      pinMode (MOTORpin, OUTPUT);
      pinMode (STOPpin, OUTPUT);
      natServo.attach(SERVOpin);  // attach servo pin to the servo object, basically pinMode
      natServo.write(straight);   // initialize servo to turn straight in the beginning 
    
      //Initialize Camera Paramters
      digitalWrite (SIpin, LOW);
      digitalWrite (CLKpin, HIGH);
 
//      Serial.begin(9600);   
      Serial.begin (115200);   // doesn't really do much for control
                               // but needs to be this fast for real time camera plotting
    }

// SOME OTHER CODE ////////////////////////////////////////////////////////////////////////
byte Pixel[NPIXELS];    // storage array for incoming data from cam


int dp=0;
int diff_thresh = 5;  // atleast this size to be considered an edge !!!!(5 works)
int pos_diff = 0;
int neg_diff = 0;
int pos_edge = 0;
int neg_edge = 127;
int prev_pos_edge = 0;  // attempt to solve intersection problem

int line_width = 18 ;   // dependent on height of camera
int line_pos = 63;   // describes where the line is, default center

int turnAngle = 71;
int motorSpeed = 0;


// MAIN CODE //////////////////////////////////////////////////////////////////////////////
void loop (void)
{
  pos_edge = 0;
  prev_pos_edge = 0;
  neg_edge = 127;
  pos_diff = 0; 
  neg_diff = 0;
  
  /////////////////////// LINESCAN CAM DATA ACQUISITION ////////////////////////////////////   
      digitalWrite (CLKpin, LOW);
      delayMicroseconds (1);          // pulse SI to capture the image
      digitalWrite (SIpin, HIGH);
      digitalWrite (CLKpin, HIGH);
      digitalWrite (SIpin, LOW);
    
      
      for (int i = 0; i < NPIXELS; i++) {    
        Pixel[i] = analogRead (AOpin) / 4;// analogRead 0 - 1053 && Pixel is byte type 0 - 255
        
//        Serial.println(analogRead (AOpin) / 4);// analogRead 0 - 1053 && Pixel is byte type 0 - 255
        
        digitalWrite (CLKpin, LOW);
        delayMicroseconds (1);            // pulse CLK and acquire image into Arduino
        digitalWrite (CLKpin, HIGH);
      }
      delayMicroseconds (1);
    
//      Serial.write ((byte)0);  // cast 0 to byte type variable, use as STARTING PT 
//      for (int i = 0; i < NPIXELS; i++) {
//        if (Pixel[i] == 0)
//          Serial.write ((byte)1);  // change 0s to 1s so not confused w/ STARTING PT
//        else
//          Serial.write ((byte)Pixel[i]);  // writes bytes to serial port
//      }

//      Serial.println("DELIN");
      
      
///////////////////////SOBEL OPERATOR EDGE DETECTION ALGORITHM//////////////////////// v0.1 (CAMERA MUST BE FOCUSED!)
for (int i = 2; i < NPIXELS; i++) // we must assume there isnt significant noise that can mimic an edge
{
  dp =  Pixel[i] - Pixel[i-2]; //Finds the change between two adjacent pixels.
                               // [-1 0 1] Sobel Operator 
  
  if(abs(dp) < diff_thresh)  // atleast diff_thresh to be considered an edge
      continue;
  
  if(dp >= pos_diff) //compares if change is greater than max change
  {
     pos_diff = dp;
     prev_pos_edge = pos_edge;
     pos_edge = i - 1;
  }   
  if(dp <= neg_diff)  //compares if change is the most negative change 
  {
     neg_diff = dp;
     neg_edge = i - 1;
  }  
}

if (neg_edge > pos_edge) {
    line_pos = (neg_edge + pos_edge)/2;
}
else {
  line_pos = (neg_edge + prev_pos_edge)/2;  // DOESNT EXACTLY WORK YET   
}

//Serial.print(pos_edge);
//Serial.print('\t');
//Serial.print(neg_edge);
//Serial.print('\t');
//Serial.println(line_pos);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////// ------ CONTROL STEERING --------//////////////////////////////////// 

  if (line_pos < 61) {
    turnAngle = map(line_pos, 0, 60, 44, 70); //Car turns left 
    natServo.write(turnAngle);
  }
  else if (line_pos > 65) {    //Car turns right 
    turnAngle = map(line_pos, 66, 127, 72, 98);
    natServo.write(turnAngle);
  }
  else {
    natServo.write(straight); //Straight if between threshold of 61 - 65 
  }
  
  /////////// ------ CONTROL MOTOR SPEED ------- //////////////////////////////////
  //Algorithm: Constant speed

  //motorSpeed = map(63 - abs(line_pos - 63), 0, 63, 30, 75);    
  motorSpeed = 20;
  analogWrite(MOTORpin, motorSpeed);

}


