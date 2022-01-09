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

char R2D2[] = {128, 128, 128, 128, 127, 128, 128, 127, 127, 127, 126, 128, 127, 127, 127, 128, 128, 128, 127, 127, 129, 128, 127, 128, 128, 128, 127, 127, 128, 128, 127, 128, 128, 128, 128, 127, 127, 128, 128, 127, 128, 128, 127, 127, 127, 127, 128, 128, 127, 126, 128, 127, 127, 128, 127, 127, 128, 128, 127, 128, 128, 127, 127, 128, 127, 128, 128, 127, 127, 128, 128, 128, 128, 128, 128, 128, 127, 127, 128, 129, 128, 127, 128, 127, 127, 128, 128, 128, 127, 128, 128, 127, 129, 128, 127, 128, 128, 127, 127, 128, 128, 128, 127, 128, 128, 128, 128, 128, 127, 127, 128, 129, 128, 128, 128, 127, 128, 127, 127, 127, 128, 127, 128, 128, 127, 128, 127, 128, 128, 128, 128, 127, 129, 127, 127, 127, 127, 129, 127, 127, 128, 128, 127, 127, 127, 128, 128, 127, 127, 128, 127, 127, 128, 128, 127, 129, 128, 128, 127, 126, 128, 128, 126, 127, 129, 127, 128, 127, 126, 128, 128, 127, 129, 127, 126, 129, 128, 127, 127, 127, 127, 127, 126, 127, 129, 127, 126, 129, 128, 125, 127, 130, 127, 125, 128, 130, 126, 126, 130, 129, 125, 127, 131, 127, 124, 131, 131, 122, 126, 133, 126, 121, 131, 132, 123, 126, 132, 127, 123, 128, 132, 127, 123, 131, 132, 124, 124, 135, 130, 120, 129, 133, 124, 125, 131, 129, 126, 127, 126, 130, 130, 121, 129, 138, 118, 119, 146, 128, 102, 140, 150, 99, 120, 163, 116, 95, 159, 144, 81, 134, 172, 96, 99, 177, 130, 77, 155, 164, 80, 120, 181, 103, 87, 175, 140, 74, 151, 170, 80, 117, 182, 104, 88, 176, 138, 73, 155, 167, 76, 118, 185, 101, 89, 180, 134, 72, 154, 167, 75, 120, 184, 102, 89, 177, 136, 74, 150, 168, 80, 117, 184, 106, 83, 175, 141, 68, 149, 175, 78, 108, 189, 109, 72, 180, 156, 61, 141, 190, 77, 90, 197, 122, 60, 175, 173, 57, 125, 200, 84, 75, 200, 140, 51, 164, 183, 55, 110, 207, 98, 68, 196, 147, 44, 153, 189, 64, 107, 207, 107, 68, 189, 147, 49, 147, 186, 73, 110, 199, 103, 67, 181, 145, 60, 153, 186, 72, 105, 195, 102, 71, 189, 155, 59, 149, 184, 66, 97, 197, 108, 71, 188, 158, 59, 139, 183, 72, 96, 197, 125, 72, 169, 156, 58, 124, 191, 97, 91, 185, 135, 60, 148, 172, 76, 115, 196, 118, 77, 172, 147, 59, 130, 190, 94, 96, 189, 128, 57, 154, 171, 73, 125, 197, 96, 71, 181, 136, 62, 162, 189, 74, 115, 192, 92, 69, 182, 145, 65, 163, 185, 72, 106, 188, 96, 78, 187, 153, 67, 150, 172, 65, 110, 198, 114, 81, 184, 143, 53, 139, 176, 74, 114, 204, 117, 73, 177, 143, 52, 141, 187, 87, 112, 197, 109, 62, 168, 156, 69, 147, 191, 83, 88, 181, 118, 69, 174, 176, 75, 128, 183, 85, 76, 179, 141, 73, 166, 181, 73, 101, 179, 102, 78, 185, 162, 68, 133, 174, 75, 96, 193, 133, 76, 165, 159, 61, 116, 183, 101, 99, 190, 141, 70, 144, 157, 74, 118, 185, 120, 98, 167, 130, 70, 135, 167, 104, 121, 172, 113, 82, 144, 141, 94, 142, 174, 111, 108, 155, 119, 87, 142, 152, 109, 137, 165, 110, 96, 140, 129, 109, 150, 155, 110, 115, 137, 113, 115, 154, 143, 117, 135, 132, 103, 115, 139, 128, 128, 149, 139, 114, 121, 127, 111, 122, 146, 139, 122, 130, 124, 106, 121, 142, 134, 128, 137, 125, 109, 121, 132, 126, 133, 144, 133, 119, 122, 119, 114, 125, 138, 135, 132, 132, 123, 114, 118, 129, 133, 135, 134, 125, 118, 119, 126, 129, 134, 136, 130, 125, 121, 119, 119, 128, 133, 133, 134, 133, 125, 117, 118, 123, 127, 134, 135, 129, 122, 121, 123, 126, 133, 134, 131, 126, 123, 119, 121, 129, 134, 133, 133, 130, 124, 120, 121, 123, 126, 132, 135, 130, 125, 123, 121, 123, 130, 133, 132, 128, 125, 120, 121, 126, 130, 133, 132, 129, 123, 121, 122, 123, 127, 132, 132, 130, 129, 126, 122, 122, 127, 128, 129, 131, 128, 125, 123, 126, 126, 128, 132, 129, 124, 122, 121, 125, 128, 130, 130, 129, 128, 124, 122, 122, 125, 128, 130, 131, 128, 125, 122, 123, 126, 129, 130, 128, 125, 124, 124, 123, 126, 129, 129, 129, 126, 125, 124, 124, 124, 126, 129, 130, 128, 128, 127, 124, 124, 126, 128, 128, 129, 128, 125, 124, 126, 128, 127, 129, 130, 126, 124, 126, 126, 126, 128, 131, 128, 124, 126, 126, 125, 125, 130, 130, 127, 125, 127, 127, 123, 126, 129, 130, 127, 125, 127, 125, 124, 129, 130, 128, 126, 129, 127, 122, 126, 130, 126, 125, 132, 130, 122, 125, 129, 127, 125, 129, 130, 126, 125, 127, 125, 127, 128, 126, 128, 130, 127, 123, 129, 126, 124, 135, 128, 110, 135, 149, 102, 106, 164, 137, 80, 132, 175, 108, 79, 160, 165, 84, 99, 182, 142, 66, 127, 185, 111, 69, 155, 179, 87, 80, 180, 161, 62, 105, 195, 134, 53, 135, 196, 103, 58, 162, 188, 74, 76, 191, 165, 53, 104, 203, 132, 50, 135, 202, 103, 53, 162, 193, 77, 68, 188, 175, 58, 92, 202, 148};
char *sound = R2D2; 
#define soundNsamples 900 //R2D2 900 elementi, sine8 800 elements, sine 100 elements

volatile boolean interruptFired = false;
int totalInterruptCounter;
 
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
 
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptFired = true;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
  
//#ifdef ESP32
//  //Wire.begin( I2C_SDA, I2C_SCL );  //PINs used for data SDA and Clock SCL
  Arduino.begin(I2C_SDA, I2C_SCL, 100000); // join i2c bus (address optional for master)
//    
//#else
//    Arduino.begin();
//#endif

    Serial.begin( 115200 );
    while(!Serial);                 // Leonardo: wait for serial monitor
    Serial.println("\nI2C Main");
delay(3000);
  //8Khz timer interrupt
  timer = timerBegin(0, 10, true);      //80MHz / 10 = 8MHz
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000, true);   //8MHz / 1000 = 8KHz
  timerAlarmEnable(timer);
}

int samplesSent = 0;
boolean intrpt = false;
void loop(void) {
//    writer();
//    reader();
    portENTER_CRITICAL(&timerMux);
    if(interruptFired) {
        intrpt = true;
        interruptFired = false;
    }
    portEXIT_CRITICAL(&timerMux);
        
  if(intrpt) {

    intrpt = false;
    if(samplesSent >= soundNsamples) samplesSent = 0;
    
    if(samplesSent == 0) Serial.print(".");
    else if(samplesSent == 1) Serial.print("1");
    else if(samplesSent == 899) Serial.println("!");
    
    Arduino.beginTransmission(ADDRESS_SLAVE);    // transmit to device #8
    Arduino.write(sound[samplesSent]);
    Arduino.endTransmission();

    samplesSent++;
  }
    //delay(500);
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
