const int detect = 10;

int detect_value = 0;

// For no delay timer
int timing = 0;

void setup () {
  
  pinMode(detect, INPUT);
  Serial.begin(9600);
  
  
  

}

void loop() {
  

  detect_value = analogRead(detect);
  Serial.println(detect_value);
  
  // no delay timer
  
  
}
