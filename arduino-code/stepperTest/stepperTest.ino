const int enPin = 1;
const int dirPin = A5;
const int stepPin = A4; 
 
void setup() {
  Serial.begin(9600);
  // Sets the two pins as Outputs
  pinMode(enPin,OUTPUT); 
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
}

void loop() {
  Serial.println("SPIN ONE");
  digitalWrite(enPin, LOW);
  digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
  // Makes 200 pulses for making one full cycle rotation
  for(int x = 0; x < 200; x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(1000); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(1000); 
  }
  
  digitalWrite(enPin, HIGH);
  Serial.println("STOP");
  delay(2000); // One second delay
  Serial.println("SPIN TWO");
  
  digitalWrite(enPin, LOW);
  digitalWrite(dirPin,LOW); //Changes the rotations direction
  // Makes 400 pulses for making two full cycle rotation
  for(int x = 0; x < 200; x++) {
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(1000);
  }
  digitalWrite(enPin, HIGH);
  Serial.println("STOP");
  delay(2000);
}
