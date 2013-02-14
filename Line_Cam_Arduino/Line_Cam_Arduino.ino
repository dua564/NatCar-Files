#define AOpin A0  // Analog pin 0
#define CLKpin 2
#define SIpin 4

//test comment

#define NPIXELS 128

// byte Pixel[NPIXELS];
int ch = 0;
int maxi;
int maxpx = 0;
int Pixel[NPIXELS];

void setup (void)
{
   pinMode (SIpin, OUTPUT);
   pinMode (CLKpin, OUTPUT);
   //pinMode (AOpin, INPUT);

   digitalWrite (SIpin, LOW);
   digitalWrite (CLKpin, HIGH);

   Serial.begin (9600);
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
         //Pixel[i] = analogRead (AOpin) / 4;  // analogRead 0 - 1053 && Pixel is byte type 0 - 255
         Pixel[i] = analogRead (AOpin);  // analogRead 0 - 1053 && Pixel is byte type 0 - 255
         digitalWrite (CLKpin, LOW);        
         delayMicroseconds (1);
         digitalWrite (CLKpin, HIGH);
         Serial.print(Pixel[i]);
         Serial.print(" ");
       }
       
       Serial.println("");
       
       delayMicroseconds (1);


//      for (i = 0; i < NPIXELS; i++)
//      {
//        
//         ch = Pixel[i];
//         if (ch < 0)
//             ch += 256;              
//         if (Pixel[i] > maxpx) 
//         {
//             maxi = i;
//         }
//      }
//
//      Serial.println(maxi);

//   for (i = 0; i < NPIXELS; i++) {
//     Serial.print((byte)Pixel[i]);
//     Serial.print(" ")
//   }
//   
//   Serial.println("");

  // output data to serial port
//   Serial.write ((byte)0);
//   for (i = 0; i < NPIXELS; i++) {
//     if (Pixel[i] == 0)
//       Serial.write ((byte)1);
//     else
//       Serial.write ((byte)Pixel[i]);
//   }

/// val = map(val, 0, 1023, 0, 179);       // scale it to use it with the servo (value between 0 and 180) 


}
