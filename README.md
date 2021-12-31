# Matty
Arduino based robot

## The Resurrection
Bought in the USA a few years ago... as a personal toy, was left un-used for years (maybe 10 yrs)... and lost all documentation, but I kept it and even took to Italy (where I live now) for the emotional attachment I developed to it.
Recently I decided to use Arduino to teach Digital Systems at my students and Matty came back to my mind.

This is the story on bringing Matty back to life!

## Description
Matty is a little cute robot, originally it was connected to WiFi using a pocket size router made by TP-Link (I think it was a clone of the WR703N... but I m not sure...).
It is propelled by 2 engine wheels and a small pivot wheel for balance. It has an custom board that hosts an Arduino Mini Clone with ATmega168 MCU. The board has the motor H-Bridge for powering the motors and some circutry for power and connectors.
The power was originally provided by 2 x 3.7 LiON batteries which where used to power the engines and to get the 5Volt for the Arduino and the Servos.
2 x Servos are used to give digital camera the Pan & Tilt, while an Ultrasound sensor is used for obstacles detection.

Somehow the router was connected to the Arduino via USB-serial interface and used to control the robot from computer using a web-interface, which will show the buttons to control the robot and the streamed video as well.

I must have lost some cables, since the router was not connected when I took it out from its tomb... the battery were dad and needed replacement....

### power it up
First thing I tryed to connect it to a Power Bank with USB(5V) output, and as a miracle it powered up... it looks there is an LDO on the board and there is about 1V drop, powering the Arduino and Servos with 4V... but it still working.

### Connection
There is a serial interface for Console connection to the Arduino, there is also another connection that looks serial with a USB female connector next to it...
Once plugged the FTDI (USB -to- Serial) cable to the main serial connector I was able to talk to the Arduino, but did not know nothing about the different connections.

I tryed programming the arduino with an empty INO file and everything went with success, but by doing that I also wiped-out the original program stored in there...

### Reverse Engineering
I made a simple plan on how to tackle the issue of figuring out how things were connected...
1. start with the Servos 
2. the Motors
3. the Ultrasonic sensor

I am going to replace the camera with an ESP-Cam which will replace the router for WiFi connection as well.

### Servos
A Servo is an electric engine that receives power and a control (3 wires total). The control is a voltage pulse which the duration will determine tha position.
This was pretty straight forward, tracing the wires I figured the Pan Servo was conncted to IOpin 10 and the Tilt Servo to IOpin 11.
Got a Servo code example and modified to control the Servos.
(see the MattySweepServo project folder)

### Motors
Electric Motors receive power and they are controlled by the power polarity which determins the spin direction, and the power voltage the Motor speed.

Also this was pretty straight forward, and resulted into the MattyMotors project folder

### Ultrasound sensor
The sensor has 4 wires, 2 wires for power, and a Ping input and a Echo output.
When a positive pulse is sent to the Ping Input, the sensor sends an ultrasound signal and at the same time rises the Echo output to a logic 1, when echo is received back the sensor lowers the Echo output to a logic 0. The duration of the pulse on the Echo output is the time for the signal to go from sensor to detected object ad back to sensor from which we can detemine the detected object distance.

This resulted into the MattyDistance project folder.

### Temporary WiFi Connection
Since I had in my drawer a ESP-01 which I bought long ago but never had the chance to use, I gave it a try while I was researching about the ESP-Cam that will arrive soon...
The ESP-01, which is a very cheap module ($2/each), has a 8266 MCU that can handle WiFi connection.
Also it has a PCB WiFi antenna and a serial interface that can be used for connection to the Arduino.

On the WiFi side the ESP-01 provides a TCP/IP Socket Server that can allow up to 4 simultaneos connections (I am interested in only one anyhow)
On the Serial Interface side the ESP-01 provides a Console that can understand AT commands used for configuration and for Tx/Rx functionalities.

Since I was already using the Arduino Interface for connection to my PC, I used the "softwareSerial" library which allows to use generic IOs for an extra Serial Interface that I could use for the connection to the ESP-01.

This was a little more trickier than I expected, also because tha last time I used an Arduino was a few years back...

Power issue: The ESP01 is a 3.3V module and I don't have that available... but I have the 4V... I tried connecting things together and everything worked... feew!

But this effort resulted into the MattyESP01WIFI project folder.

I can, now, reach the Robot through WiFi. I use "screen //telnet 192.168.1.142 80" for the connection. The IP address is printed out on Console during power up, fortunatly the address stays the same between power up... I will figure out a way to signal me the eventuality it changes IP address.

## Putting all things together
What was left to do was taking all the codes amd merge them in an Arduino code that would allow me to control the robot remotley through WiFi connection.

Matty would connect to WiFi, I would connect using my PC to Matty using the "screen //telnet 192.168.1.142 80" command.

Once connection is established I can control the robot with the following commands:
  case 's': //stop
  case 'f': //forward fast
  case 'b': //backward fast
  case 'r': //Right
  case 'l': //left
  
  If an obstacle is detected closer than a Threshold a Warning message is displayed...
  Still in the prototoping stage but it is working!!!
## Matty wants to speak
It look like the Arduino clone it came with Matty has 6 Analog pins... I would like to use one of them for driving a speaker
My starting point is how to use interrupts and timers for playing sounds: https://www.instructables.com/Arduino-Audio-Output/
This resulted to the MattyPlayer_tone prject folder.
To test it I connected a 4 Ohms speaker to pin 19(Analog) and ground... it worked perfectly and with good volume :)

Next step is to play a sound from a audio file

R2D2 A-Z sounds: http://www.optimiced.com/en/2021/04/08/r2-d2-sounds-download/

I need to convert this mp3 files to a vector of integers...

1st step: used audocity to convert file to WAVe file with 8bits @ 8KHz
2nd step: used sox for converting WAV file to RAW format with the following command:
``sox R2D2-sample-message.wav -r 8000 -c 1 --bits 8 --encoding unsigned-integer R2D2-sample-message.raw``
3rd step: convert the binary file into CSV using the following python script:

```
python3 -c '
f=open("R2D2-sample-message.raw","rb")
byte = f.read(1)
max = ord(byte)
n=0
while byte:
        n += 1
        i = ord(byte)
        if i > max:
                max = i
        print(str(i)+",",end=" ")
        byte = f.read(1)
        if n == 900:
                break
    
f.close()
print("")
print("max: ", max)
print("n: ", n)
'
```

That worked well, even though the Arduino can handle only about 800 samples which is about 800/8000 = 0.1sec of sound...

the idea is to stream the data from PC to Matty...
here is what I would do:
send 100 samples at the time from PC
on the Arduino side, I would have an active buffer, a ready buffer and a receiving buffer, the active buffer is used for playing the sound, the receiving buffer is used to store the incoming data, the ready buffer is a buffer ready to be played.
When the data starts coming, 1st the active buffer is filled, then the ready buffer is filled, then the ready buffer starts being played and a request for new data is sent to PC, when the data is received it is going to fill the receiving buffer, when the active buffer is played, a new request gets sent to PC for new data, the active becomes --> the receiving, which becomes --> the ready, which becomes the active, creating a cirular buffer.
With this algorithm there is a 200 samples backup which can account for a 100/8 = 12.5ms jitter in the communication channel for the streaming.
We will see how thi works...
...
Well, the plans have chaged..., with the arrival of the ESP32-CAM...

## The New architecure
Since the arrival of the ESP32-CAM, which has much more power than the Arduino, The ESP32-CAM will be the BRAIN of Matty and the Arduino will simply be an interface with the motors, the servos and the distance sensor. I am palnning also to connect a speaker directly to the Arduino DAC, and a MIC through a I2S interface.
The ESP32-CAM will be responible for WiFi connection, SDcard for file strorage... planning on using both the CAM and the SDcard, only 4 pins will be avaiable, 2 can be used for the serial interface and 2 can be used for an evenatual additional I2C interface.

The connection between the ESP32-CAM and the Arduino will be by the Serial interface.

### The Mic
I am planning to use the Mic for providing vocal commands to Matty... Matty will be always listening... the Mic I have is INMP441 which gives a 24bit data trhrough an I2S interface... oops... I2S interface in not implemented in Arduino, beside a few models of them... OK then I am restriced in using an analog Mic and use the 8bit ADC embedded into the Arduino. I am planning on using 8KHz sampling rate (same as the speaker)... hope the quality will be sufficient...

### The ESP32 - Arduino Serial Interface
Mic stream will need 8 bit x 8Khz = 64 Kbit/s (Mic --> Arduino --> ESP-CAM)
Speaker same as Mic but in opposit direction (ESP-CAM --> Arduino --> Speaker)
Other data... whatever is left.
Minimum data rate should be 160,000 bit/sec (which is a multiple of 16MHz which I suspect is the clock frequency of the ArtMEGA128)

Should I use the I2C interface instead?

## What is next
1. adding a speaker and the ability to play sounds
2. When the ESP-Cam arrives, replace the ESP-01 and allow video streaming
3. Add a mic Array for sound direction of arrival
4. Add AI for object recognition and tracking
5. House mapping based on object recognition
6. Making Matty interactive by using his ability for house patroling
7. more...
