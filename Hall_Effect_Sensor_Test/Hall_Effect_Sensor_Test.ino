
void setup() {
  pinMode(6,OUTPUT);
  Serial.begin(9600);
}

int sensorValue;
int last_sensorValue = 0;
int num_of_rot = 0;        // half rotations
unsigned long curr_time = 0;
unsigned long last_time = 0;
unsigned long half_rot_time = 0;

boolean half_cycle = false;

void loop() {
  
//  if(num_of_rot < 10)
//    analogWrite(6,5);
//  else if ((num_of_rot > 100) &&(num_of_rot < 200))
//    analogWrite(6,10);
//  else if ((num_of_rot > 200) &&(num_of_rot < 300))
//    analogWrite(6,15);
//  else
    analogWrite(6,20);
   
  sensorValue = analogRead(A1);
  
  if(sensorValue == 0){
      if(last_sensorValue != 0){
          half_cycle = !half_cycle;
          last_sensorValue = 0;
          num_of_rot++;               // count # of rot per revolution
                 
            curr_time = millis();    // timing analysis
            half_rot_time = curr_time - last_time;
            last_time = curr_time; 
    } 
  }
  else{
     if(last_sensorValue == 0){
        half_cycle = !half_cycle;
        last_sensorValue = 1023;
        
            //num_of_rot++;               // count # of rot per revolution      
            curr_time = millis();       // timing analysis
            half_rot_time = curr_time - last_time;
            last_time = curr_time; 
     }
  }
  Serial.print("Half Rotation Time: "); 
  Serial.print(half_rot_time);
  Serial.print("\t");
  Serial.print("num_of_rot: "); 
  Serial.print(num_of_rot);
  Serial.print("\t");
  Serial.print("sensorValue: "); 
  if(sensorValue == 0)
      Serial.println("0");
  else
      Serial.println("1");
  
  delay(1);        // delay in between reads for stability
  
}
