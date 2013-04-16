/*
******************************
*    Natcar debugging API    *
*         For Arduino        *
******************************
                     
  |\   |    /\   ------- |-----    /\    |---\
  | \  |   /  \     |    |        /  \   |___/  
  |  \ |  /    \    |    |       /    \  | \
  |   \| /      \   |    |----- /      \ |  \
  
   /\_____/\
  |         |
  |  O   O  |
  |    U    |
  |_________|
     |    |
     |     \    \
     |w___w_\___/

 Version 0.8 12/26/2012  -  Jacob Sharf <jacobsharf@gmail.com>
   -Created packet generation and sending functions
   
 Todo for Version 1.0:
     - Wrap this up into an actual API instead of an arduino code file
     - Lots of testing
   
   Packet Protocol description:
    
   Y[steering][speed][sensor]Z
    
   Y&Z - just two ascii characters which signify the start and end of the packet
   steering - 3-digit number ranging from 0 to 180
   speed - 3-digit number. first digit is tens place. second digit is ones place. third digit is tenths place
   sensor - 3-digit number. between 0 and 999
   
   total packet length: 11 bytes.
   
   Just to be explicit, here's an example packet:
   
   Y090103672Z

   This is transmitting the information: steering = 90 degrees, speed = 10.3 ft/s, sensor = 672 (out of 1000)
*/

// Line Camera
#define AOpin A0  // Collect data from Linecam
#define CLKpin 3  // Ctrl Linecam CLOCK
#define SIpin 5   // Ctrl Linecam

// Servo
#include <Servo.h> 
#define SERVOpin 10   //output to control servo 
Servo natServo;

// Motor Controller 
#define MOTORpin 7  // output PWN to ctrl speed
#define STOPpin 9    // output to ctrl fast stop switch

String generatePacket(int steering, float carSpeed, int sensor);
void sendPacket(String packet);

////////////////////////////////////////////////////////////////////////
const int NPIXELS = 128;
const int memorySize = 5;

const byte BLACK=0;
const byte WHITE=100;

const int straight = 71;
const int hardLeft = 98;
const int hardRight = 44;
// turn range 44 - 98 (right to left)
// straight 71

byte Pixel[NPIXELS];    // storage var for incoming data from cam

void setup() {
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

byte pixVal; 
byte maxpx;
int maxi = 0;
int motorPWM = 0;
int motorSpeed = 0;
int turnAngle = 71;

int onLine = 0;
byte MinValueActual = 255;
byte MaxValueActual = 0;


void loop() {  
  // WRITE CODE HERE @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  
/////////-------- LINESCAN CAM DATA ACQUISITION --------////////////////////////////////////   
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
        delayMicroseconds (1);  

/////////////////////--------------Threshold Algorithm----------- v0.1  ////////////////////
       // Determine which pixels are BLACK and which are WHITE.
      byte Threshold = 80;
      for (i = 0; i < NPIXELS; i++) { 
        pixVal=Pixel[i];
        if (pixVal < Threshold)
          Pixel[i]= BLACK;        
        else
          Pixel[i] = WHITE;  
      }     
       
     //Finds the Average of the Signal to determine where the line is.    //issue!!! prob if not just one square wave 
     for (i = 0; i < NPIXELS; i++){ 
        pixVal=Pixel[i];  
         if ((pixVal==WHITE) && (onLine==0)) {
           leftPoint=i;
           onLine=1;
         }
         if ((pixVal==BLACK)&& (onLine==1)) {
           rightPoint=i;
           onLine=0;             
        }
      }  
      midPoint = ((rightPoint+leftPoint)/2);

//////////// ------ CONTROL STEERING --------//////////////////////////////////// 
    //Algorithm: Car ONLY turns with respect to brighest pixel
    
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
    
              motorPWM = 8; //Duty Cycle Percentage 
              motorSpeed = map(motorPWM, 0, 100, 0, 255);
              analogWrite(MOTORpin, motorSpeed);     

      int packetPos = map(midPoint, 0, 127, 0, 999);
      int packetSteering = map(turnAngle, 44, 98, -26, 26);
      int packetSpeed = 10.0;      // not accurate dont know speed, wait for rotary encoder
      
  String packet = generatePacket(packetSteering, packetSpeed, packetPos);
  sendPacket(packet);
}



//// IMPLEMENTATION ///////////////////////////////////////////////////////////////////////

//encodes information passed via arguments into packet
//carSpeed is in feet/sec
//steering is from -90 to 90 degrees
String generatePacket(int steering, float carSpeed, int sensor)
{
     String packet = "";
     String prefix = "";
     packet += "Y";
     //concert range from [-90 to 90] to [0 to 180] for transmission
     steering+=90;
     if(steering<10)
     {
       prefix = "00";
       packet += (prefix + steering);
     }
     else if(steering<100)
     {
       prefix = "0";
       packet += (prefix + steering);
     }
     else
       packet += steering;
     //speed is divided by 10 on the other side, so the next step is okay. 
     //it's a dirty trick to truncate float carSpeed to 1 decimal place
     int tempSpeed = (carSpeed*10);
     if(tempSpeed<10)
     {
       prefix = "00";
       packet += (prefix + tempSpeed);
     }
     else if(tempSpeed<100)
     {
       prefix = "0";
       packet += (prefix + tempSpeed);
     }
     else
       packet += tempSpeed;
     if(sensor<10)
     {
       prefix = "00";
       packet += (prefix + sensor);
     }
     else if(sensor<100)
     {
       prefix = "0";
       packet += (prefix + sensor);
     }
     else
       packet += sensor;
     packet += "Z";
     return packet;
}

//sends a packet over serial communications
//prereq: MUST call Serial.begin([baudrate]) in setup()
//if you don't know what to put for baudrate, try 9600
void sendPacket(String packet)
{
    Serial.print(packet);
}
