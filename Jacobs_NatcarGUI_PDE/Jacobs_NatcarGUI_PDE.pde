/*

******************************
*    Natcar debugging GUI    *
******************************
                     
  |\   |    /\   ------- |-----    /\    |---\
  | \  |   /  \     |    |        /  \   |___/  
  |  \ |  /    \    |    |       /    \  | \
  |   \| /      \   |    |----- /      \ |  \
  
   /\_____/\
  |         |
 _|  O   O  |_
 -|    U    |-
  |_________|
     |    |
     |     \    \
     |w___w_\___/

 Version 1.0 12/26/2012  -  Jacob Sharf <jacobsharf@gmail.com>
   -Finished packet specifications
   -Finished packet functions
   -Added functions to parse packets and retrieve information such as speed, sensor value, and steering
   -First test communicating with an Arduino SUCCESSFUL :D
 Version 0.5 11/25/2012  -  Jacob Sharf <jacobsharf@gmail.com>
   -Added initial GUI
   -Begin packet protocol specification
   -Some packet functions

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

import processing.serial.*;

Serial myPort;
final int MAXSPEED=15;
final int UPPERSPEED=10;
final int MAX_PACKET_SIZE = 11;
final int graphsize = 150;
final int logsize = 11;
final String prefix = "Now Processing... ";
int deg, steering, incr, percent;
float speed, incrf;
float[] svals;
String[] logs;

void setup()
{
   size(620, 550, P3D); 
   background(0);
   
   //Serial port stuffs
   println(Serial.list());
   if(Serial.list().length>0)
     myPort = new Serial(this, Serial.list()[0], 115200);

   
   //create font
   textFont(createFont("Arial", 30));
   textAlign(LEFT);
   deg = 0;
   steering = -45;
   incr = 1;
   incrf = 0.1;
   speed = 7;
   
   //graph initialization
   svals = new float[graphsize];
   for(int i=0; i<graphsize; i++)
   {
      svals[i] = 82.5; 
   }
   logs = new String[logsize];
   for(int i=0; i<logsize; i++)
   {
      logs[i] = prefix;
   }
   percent = 0;
}

void drawTemplate()
{
   background(0);
   fill(0);
   stroke(75, 200, 75);
   //Log
   //rect(325, 225, 250, 295);
   //rect(325, 275, 250, 245);
   //steering
   rect(100, 80, 200, 200);
   //car body
   rect(160, 150, 50, 50);
   //wheels
   fill(0, 255, 0);
   pushMatrix();
     translate(160, 150);
     rotate(radians(steering));
     rect(-7.5, -15, 15, 30);
   popMatrix();
   pushMatrix();
     translate(210, 150);
     rotate(radians(steering));
     rect(-7.5, -15, 15, 30);
   popMatrix();
   rect(155, 190, 15, 30);
   rect(205, 190, 15, 30);
   fill(0);
   //speed
   rect(325, 80, 250, 125);
   //bar
   strokeWeight(2);
   rect(340, 140, 215, 15);
   //filled bar
   if(speed>=UPPERSPEED)
     fill(255, 0, 0);
   else
     fill(0, 255, 0);
   rect(340, 140, 215*(speed/MAXSPEED), 15);
   fill(0);
   
   //Sensor
   strokeWeight(1);
   rect(100, 300, 200, 220);
   strokeWeight(2);
   //graph
   stroke(0, 255, 0);
   rect(115, 350, 165, 152);
   for(int i=0; i<graphsize-1; i++)
   {
      stroke(255);
      strokeWeight(1);
      line(svals[i]+115, i+351, svals[i+1]+115, i+352); 
   }
   strokeWeight(1);
   fill(75, 200, 75);
   textAlign(LEFT);
   textFont(createFont("Arial", 30));
   text("[NATCAR] debug GUI", 100, 60);
   textFont(createFont("Arial", 25));
   text("Steering", 110, 105);
   text("Speed", 335, 105);
   text("Sensor", 110, 325);
   //text("Log", 340, 255);
   textFont(createFont("Arial", 20));
   text("Angle: ", 180, 260);
   text("Ft/s:", 452, 190);
   /* log slows everything down. temporarily taken out until I optimize code
   textFont(createFont("Ti89Pc", 19, true));
   for(int i=0; i<logsize; i++)
   {
      text(logs[i], 335, 300+i*21.5); 
   }
   textFont(createFont("Arial", 20));
   */
   textAlign(RIGHT);
   text(steering, 270, 260);
   text(speed, 557, 190);
   //rotating cube
   pushMatrix();//indented to show matrix translation levels
     translate(50, 50, 0);
     rotateY(radians(deg));
     rotateX(radians(deg));
     noFill();
     scale(0.8);
     stroke(75, 200, 75);
     box(30);
   popMatrix();
   //spinning cube animation
   deg+=3;
}

void rshift(float a[])
{
  for(int i=graphsize-1; i>0; i--)
  {
     a[i]=a[i-1];
  } 
}

void rshift(String a[])
{
  for(int i=logsize-1; i>0; i--)
  {
     a[i]=a[i-1];
  } 
}

int getPacket(char packet[])
{
  char a;
  int count;
  count = 0;
  a = myPort.readChar();
  if(a=='Y')
  {
    packet[count++] = a;
    while(a!='Z')
    {
      a = myPort.readChar();
      packet[count++] = a;
      if(count>=MAX_PACKET_SIZE && a!='Z')
         return 0; 
    }
    for(int i=0; i<MAX_PACKET_SIZE; i++)
      print(packet[i]);
    println("");
    return 1; 
  }
  return 0;
}

float getSpeed(char packet[])
{
  float speed;
  String strspeed="";
  for(int i=4; i<=6; i++)
      strspeed+=packet[i];
  speed = Integer.parseInt(strspeed);
  //speed is 1/10th what is sent in packet, because of packet specifications.
  return (speed/10);
}

int getSteering(char packet[])
{
  int steer;
  String strsteer="";
  for(int i=1; i<=3; i++)
      strsteer+=packet[i];
  steer = Integer.parseInt(strsteer);
  //steering is in the range 0 to 180, but we want it to be in the range -90 to 90
  return steer-90;
}

float getSensor(char packet[])
{
  float sensor;
  String strsensor="";
  for(int i=7; i<=9; i++)
      strsensor+=packet[i];
  sensor = Integer.parseInt(strsensor);
  sensor = (sensor/999)*graphsize;
  return sensor;
}

void draw()
{    
  char[] packet = new char[MAX_PACKET_SIZE];  //dynamically allocated array
  if(getPacket(packet) == 1)
  {
    speed = getSpeed(packet);      //pass by
    steering = getSteering(packet);
    //rightshift graph
    rshift(svals);
    //set newest value of graph
    svals[0] = getSensor(packet);
    //BIG IMPORTANT LINE WHICH DRAWS THE GUI
    drawTemplate();
  }
}

//learn how to make a graph: http://www.learningprocessing.com/examples/chapter-13/example-random-graph/
