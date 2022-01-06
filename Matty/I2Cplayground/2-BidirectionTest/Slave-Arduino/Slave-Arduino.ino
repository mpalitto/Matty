// <https://www.arduino.cc/en/Tutorial/MasterWriter>
// <https://www.arduino.cc/en/Tutorial/MasterReader>
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Sends data as an I2C/TWI sub device

// Created 29 March 2006

// This example code is in the public domain.

// #define MICON_ESP32              // undefined if Arduino

#ifdef MICON_ESP32
#   error "I2C Sub functionality is not implemented in ESP32 Arduino Core"
#   error "If you want to implement, see https://www.arduino.cc/reference/en/libraries/esp32-i2c-slave/"
#endif

#define ADDRESS 0x8

#include <Wire.h>

void setup() {
    Wire.begin(ADDRESS);            // join i2c bus with address #8
    Wire.onReceive(receiveEvent);   // register event
    Wire.onRequest(requestEvent);   // register event

    Serial.begin( 115200 );         // start serial for output
    while(!Serial);                 // Leonardo: wait for serial monitor
    Serial.println("\nI2C Sub");
}

void loop(void) {
    delay(100);
}

// function that executes whenever data is received from main
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
    if( howMany == 0 ) {
        return;
    }

    Serial.print("received from ESP-CAM: ");

    while( 1 < Wire.available() ) { // loop through all but the last
        char c = Wire.read();       // receive byte as a character
        Serial.print(c);            // print the character
    }
    int x = Wire.read();            // receive byte as an integer
    Serial.println(x);              // print the integer
}

// function that executes whenever data is requested by main
// this function is registered as an event, see setup()
void requestEvent(void) {
    Serial.println("sending to ESP-CAM: ");

    Wire.write("hello from Arduino\n");          // respond with message of 6 bytes
    // as expected by main
}
