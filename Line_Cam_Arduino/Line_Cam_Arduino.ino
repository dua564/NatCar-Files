#define AOpin A0  // Analog pin 0
#define CLKpin 2
#define SIpin 3

#define NPIXELS 128

byte Pixel[NPIXELS];

void setup (void)
{
   pinMode (SIpin, OUTPUT);
   pinMode (CLKpin, OUTPUT);
   //pinMode (AOpin, INPUT);

   digitalWrite (SIpin, LOW);
   digitalWrite (CLKpin, HIGH);

   Serial.begin (115200);
}


void loop (void)
{
   int i;

  // SI pulse to capture the image
     digitalWrite (CLKpin, LOW);
     delayMicroseconds (1);
     digitalWrite (SIpin, HIGH);    
     digitalWrite (CLKpin, HIGH);
     digitalWrite (SIpin, LOW);
   
   // pulse CLK and acquire image into Arduino
       for (i = 0; i < NPIXELS; i++) {
         Pixel[i] = analogRead (AOpin) / 4;  // analogRead 0 - 1053 && Pixel is byte type 0 - 255
         digitalWrite (CLKpin, LOW);        
         delayMicroseconds (1);
         digitalWrite (CLKpin, HIGH);
       }
       delayMicroseconds (1);

//   for (i = 0; i < NPIXELS; i++) {
//     Serial.print((byte)Pixel[i]);
//     Serial.print(" ")
//   }
//   
//   Serial.println("");

  // output data to serial port
   Serial.write ((byte)0);
   for (i = 0; i < NPIXELS; i++) {
     if (Pixel[i] == 0)
       Serial.write ((byte)1);
     else
       Serial.write ((byte)Pixel[i]);
   }

/// val = map(val, 0, 1023, 0, 179);       // scale it to use it with the servo (value between 0 and 180) 


}
