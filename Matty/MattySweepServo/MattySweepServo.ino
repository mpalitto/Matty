/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 https://www.arduino.cc/en/Tutorial/LibraryExamples/Sweep
*/

#include <Servo.h>

Servo myservoUpDown;  // create servo object to control a servo for Up Down
Servo myservoLR;  // create servo object to control a servo for Left-Right 
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  myservoLR.attach(10);  // attaches the servo Left on pin 9 to the servo object
  myservoUpDown.attach(11);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservoLR.write(pos);              // tell servo to go to position in variable 'pos'
    myservoUpDown.write(pos);              // tell servo to go to position in variable 'pos'
    //delay(15);                       // waits 15 ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservoLR.write(pos);              // tell servo to go to position in variable 'pos'
    myservoUpDown.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
}
