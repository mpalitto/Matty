// <https://www.arduino.cc/en/Tutorial/MasterWriter>
// <https://www.arduino.cc/en/Tutorial/MasterReader>
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI sub device

// Created 29 March 2006

// This example code is in the public domain.

#define ESP32                 // undefined if Arduino

#ifdef ESP32
#   define I2C_SDA  0               // GPIO0
#   define I2C_SCL 16               // GPIO16
#endif

// Check using ../I2C_Scanner/I2C_Scanner.ino
#define ADDRESS_SLAVE 0x8

#include <Wire.h>
TwoWire Arduino = TwoWire(0);
void setup() {
#ifdef ESP32
  //Wire.begin( I2C_SDA, I2C_SCL );  //PINs used for data SDA and Clock SCL
  Arduino.begin(I2C_SDA, I2C_SCL, 400000); // join i2c bus (address optional for master)
    
#else
    Arduino.begin();
#endif

    Serial.begin( 115200 );
    while(!Serial);                 // Leonardo: wait for serial monitor
    Serial.println("\nI2C Main");
}

void loop(void) {
    writer();
    reader();
    delay(500);
}

void writer(void) {

    static byte x = 0;
    Serial.println("sending to Arduino: x is "+String(x,DEC));
    Arduino.beginTransmission(ADDRESS_SLAVE);    // transmit to device #8
    Arduino.write("x is ");            // sends five bytes
    Arduino.write(x);                  // sends one byte
    Arduino.endTransmission();         // stop transmitting
    x++;
}

void reader(void) {
    Serial.print("received: ");

    Arduino.requestFrom(ADDRESS_SLAVE, 19);   // request 1 bytes from sub device #8

    while( Arduino.available() ) {     // sub may send less than requested
        char c = Arduino.read();       // receive a byte as character
        Serial.print(c);            // print the character
    }
}
