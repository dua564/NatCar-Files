import processing.serial.*;

final int LINELEN = 128;
final int PLOTHT = 260;
PImage img;
Serial duino;
boolean Synced = false;

void setup ()
{
   println ("<START>");
   println (Serial.list());
   println ("<END>");

   // Open serial port to Arduino at 115200 baud
   duino = new Serial (this, Serial.list()[0], 115200 );

   // Make window high enough for the plots, and wide enough to fit the pixels
   size (LINELEN, PLOTHT);

   // Image is same size as window
   img = createImage (LINELEN, PLOTHT, RGB);
   
   // Initialise image to a shade of blue
   img.loadPixels ();  // tells the compiler "i'm going to modify the pixels"
   for (int i = 0; i < img.pixels.length; i++) {
     img.pixels[i] = color (0, 0, 0);
   }
   img.updatePixels ();  // tells compiler to update the pixels, I'm done modifying them
  
   // Choose image update rate
   frameRate (30);
}



void draw ()
{
   int i;
   int ch;
   int nbufs;
   int b;
   int maxi;
   int maxpx;
   byte[] inbuf = new byte[LINELEN + 1];

   // Synchronise
   if (Synced) {
       nbufs = duino.available () / (LINELEN + 1);  //check if 
   }
   else {
           do {
               while (duino.available () == 0)
               ; 
               ch = duino.read ();
      
           } while (ch != 0);
           
           nbufs = 0;
           Synced = true;
   }


   // Load the image pixels in preparation for next row(s)
   img.loadPixels ();
   
     for (b = 0; b < nbufs; b++) {
       // Scroll the old image data down the window
       for (i = img.pixels.length - LINELEN - 1; i >= 0; i--) {
       img.pixels[i + LINELEN] = color(0,0,0);
       }
      
         // Read 128 pixels from image sensor, via Arduino
         duino.readBytes (inbuf);
    
               // Check we're still in sync
               if (inbuf[128] != 0) {
                 print ("UNSYNC ");
                 Synced = false;
               }

           maxi = 0;
           maxpx = 0;

     // Transfer incoming pixels to image
           for (i = 0; i < LINELEN; i++) 
           {
             ch = inbuf[i];
             if (ch < 0)
               ch += 256;
               
             ch = 255 - ch;
             
             img.pixels[i+ch*LINELEN] = color (255, 255, 255);
           }
   }
   

   // We're done updating the image, so re-display it
   img.updatePixels ();
   image (img, 0, 0);
}
