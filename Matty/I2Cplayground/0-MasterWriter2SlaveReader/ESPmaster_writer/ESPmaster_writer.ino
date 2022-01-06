// Wire Master Writer

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>
#define I2C_SDA 0
#define I2C_SCL 16    


TwoWire Arduino = TwoWire(0);
byte cnt = 0;

void setup() {
  Serial.begin(115200);
  Arduino.begin(I2C_SDA, I2C_SCL, 400000); // join i2c bus (address optional for master)
}

byte x = 0;

void loop() {
  Arduino.beginTransmission(8);  // transmit to device #8
  Arduino.write("x is ");        // sends five bytes
  Arduino.write(x);              // sends one byte
  Arduino.endTransmission();     // stop transmitting

  x++;
  delay(1);

    Arduino.requestFrom(8,10);
    
  while (Arduino.available() > 1) { // loop through all but the last
    char c = Arduino.read();          // receive byte as a character
    //if(! cnt++)
      Serial.print(c);               // print the character
      cnt++;
  }

}
