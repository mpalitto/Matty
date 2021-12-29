int M1forward = 5;   // MotorDirectionPin set to pin3
int M1backward = 4;   // MotorDirectionPin set to pin3

int M2forward = 6;   // MotorDirectionPin set to pin3
int M2backward = 9;   // MotorDirectionPin set to pin3



void setup()

{
  Serial.begin(9600); // start serial port at 9600 bps:
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
//  pinMode(MotorPin, OUTPUT);  // sets the pin as output
//M1 - motor 1
  pinMode(M1forward, OUTPUT); // sets the pin as output
  pinMode(M1backward, OUTPUT);

//M2 - motor 2
  pinMode(M2forward, OUTPUT); // sets the pin as output
  pinMode(M2backward, OUTPUT);
 
  establishContact(); 
}



void loop()

{
  //digitalWrite(MotorPin, HIGH);
  
  if (Serial.available() > 0) {
    char serialread = Serial.read();
    switch(serialread){
      case 'w': // forward slow
      Serial.print(serialread);
      analogWrite(M2forward, 130);
      analogWrite(M2backward, 0);
      break;
      case 's': //backward slow
      Serial.print(serialread);
      analogWrite(M2forward, 0);
      analogWrite(M2backward, 130);
      break;
      case 'a': //stop
      Serial.print(serialread);
      analogWrite(M2forward, 0);
      analogWrite(M2backward, 0);
      break;
      case 'e': //forward fast
      Serial.print(serialread);
      analogWrite(M2forward, 255);
      analogWrite(M2backward, 0);
      break;
      case 'd': //backward fast
      Serial.print(serialread);
      analogWrite(M2forward, 0);
      analogWrite(M2backward, 255);
      break;
      
    }
    
  }
}

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.print('A');   // send a capital A
    delay(300);
  }
}
