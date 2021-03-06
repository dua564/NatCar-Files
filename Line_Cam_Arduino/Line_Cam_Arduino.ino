#define AOpin A0  // Analog pin 0
#define CLKpin 3
#define SIpin 5

#define NPIXELS 128

byte Pixel[NPIXELS];
//int ch = 0;
//int maxi;
//int maxpx = 0;


void setup (void)
{
   pinMode (SIpin, OUTPUT);
   pinMode (CLKpin, OUTPUT);
   //pinMode (AOpin, INPUT);

   digitalWrite (SIpin, LOW);
   digitalWrite (CLKpin, HIGH);

   Serial.begin (115200);
}

int i;

void loop (void)
{

   // SI pulse to capture the image
   digitalWrite (CLKpin, LOW);
   delayMicroseconds (1);
   digitalWrite (SIpin, HIGH);
   digitalWrite (CLKpin, HIGH);
   digitalWrite (SIpin, LOW);

   // pulse CLK and acquire image into Arduino
   for (i = 0; i < NPIXELS; i++) {
//     Serial.print(analogRead (AOpin) / 4);
//     Serial.print(", ");
     
     Pixel[i] = analogRead (AOpin) / 4;// analogRead 0 - 1053 && Pixel is byte type 0 - 255
     digitalWrite (CLKpin, LOW);
     delayMicroseconds (1);
     digitalWrite (CLKpin, HIGH);
   }
   delayMicroseconds (1);

//   Serial.println("");
//   for (i = 0; i < NPIXELS; i++) {
//     println(Pixel[i]);
//     println("NEXTSECTIONOFDATA");
//   }

   Serial.write ((byte)0);  // cast 0 to byte type variable, use as STARTING PT 
   for (i = 0; i < NPIXELS; i++) {
     if (Pixel[i] == 0)
       Serial.write ((byte)1);  // change 0s to 1s so not confused w/ STARTING PT
     else
       Serial.write ((byte)Pixel[i]);  // writes bytes to serial port
   }
}
