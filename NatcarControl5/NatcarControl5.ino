 // UCLA NATCAR PROJECT - TEAM DIZHEL ///// ------> VERSION 0.2 ALPHA
// by Jeffrey Hwang, Nikhil Dua, Danny Li, Genya Zhdanov
//
//
// Version Update Information
// 0.1 Set pins, initialize paramaters, line cam acquisition 2/7/13
// 0.2 implement new camera data acquisition algorithm, brightest pixel 2/15/13
// 0.3 max change/derivative alogirithm (sobel operator) for line tracking 3/8/13
// 0.4 refined sobel operator & try to account for intersections 3/29/13
// 0.5 timing options added


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
   
// Hall Effect Sensor -- Speed Detection    


// CONST PARAMETERS /////////////////////////////////////////////////////////////////////
    const int NPIXELS = 128;  // number of pixels "seen" by line cam
    const int history_size = 5;
    
    const int straight = 71;  // turn range 44 - 98 (Right to Left)
    const int hardLeft = 44;  // straight is about 71
    const int hardRight = 98;

// Adjustable Parameters <<<<<<<<<<<<<<<<<<<-----------------------------------------------------------------------------------
    int diff_thresh = 7;
    double kp=0.5;
    double kd=0;

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
int history[10] = {0};
int last_known_pos = 0;
boolean off_track = false;
boolean left_side = true;

int dp=0;
int pos_diff = 0;
int neg_diff = 0;
int pos_edge = 0;
int neg_edge = 127;
int prev_pos_edge = 0;  // attempt to solve intersection problem
int line_pos = 63;   // describes where the line is, default center
int turnAngle = 71;
int motorSpeed;;

//PD Controller Parameters
    unsigned long lastTime=0;
    double dErr=0;
    double lastErr=0;
    double Input; 
    int Output; int Setpoint=63; int finalOutput;
    unsigned long currTime;
    double dt; 
    double error;

// MAIN CODE //////////////////////////////////////////////////////////////////////////////
void loop (void)
{
  currTime=millis();//Time at the beginning of the loop
  dt = double(currTime - lastTime);  //Time difference between beginning and end of loop  
  error=0;
  dErr=0;
 
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
        Pixel[i] = analogRead (AOpin) >> 2;// analogRead 0 - 1053 && Pixel is byte type 0 - 255
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
    Input=line_pos; //Setpoint is the position of the car we want from what it sees from the camera 
    error=Setpoint-Input;  
    dErr=(error-lastErr)/dt; //Computing Derivative Calculation
}
else {
    Serial.println("Error!");
}

if( (pos_edge == 0) && (neg_edge == 127))
    off_track = true;
else{
    last_known_pos = line_pos;
    off_track = false;
}

//Serial.print(pos_diff);
//Serial.print('\t');
//Serial.print(neg_diff);
//Serial.print('\t');
//Serial.println(line_pos);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////// ------ CONTROL STEERING --------//////////////////////////////////// 
   
   //Computing PD Output//
   Output= -1*( kp * error + kd * dErr);
      
   
//  Serial.print("Error is: ");
//  Serial.print(error);
//  Serial.print('\t');
//  Serial.print("dErr is: ");
//  Serial.print(dErr);
//  Serial.print('\t');
//  Serial.print("Output is: ");
//  Serial.print(Output);
//  Serial.print('\t');
  
  
  turnAngle=abs(Output+71);
  
  if (turnAngle>98)  //Turning Right
  {
    turnAngle=98;
  }
  if (turnAngle<44) //Turning Left 
  {
    turnAngle=44;
  }  
  if ((Output >=-2) && (Output <= 2))
  {
    turnAngle=71;  //Straight Turn
  }
  
  natServo.write(turnAngle);
 
  
//  Serial.print("TurnAngle is: ");
//  Serial.print(turnAngle);
//  Serial.print('\t');
  
  
  
  /////////// ------ CONTROL MOTOR SPEED ------- //////////////////////////////////
  //Algorithm: Constant speed

  //motorSpeed = map(63 - abs(line_pos - 63), 0, 63, 30, 75);    
  if(off_track)
  {
    motorSpeed = 15;
    natServo.write(turnAngle);    
  }
  else {
    for(int i = 1; i < 10; i++){
      history[i-1] = history[i];
      Serial.print(history[i-1]);
      Serial.print("\t");
    }
    history[9] = turnAngle;
    if(turnAngle == 71)
      motorSpeed = 40;
    else
      motorSpeed = 25;
  }
  
  for(int i = 1; i < 10; i++){
      Serial.print(history[i-1]);
      Serial.print("\t");
  }
  Serial.println(history[9]);
  analogWrite(MOTORpin, motorSpeed);
  
 
  lastTime=currTime; //Time at the end of the loop
  lastErr=error;
  
//    Serial.print("Time Difference is: ");
//    Serial.println(dt);
}


