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

int t;

void setup()
{
   Serial.begin(9600);
   t=0; 
}

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

void loop()
{
  
  
  
  // WRITE CODE HERE
  
  
  
  
  t+=10;
  String packet = generatePacket(45, 9.3, 500 + 200*sin(t/20));
  sendPacket(packet);
}
