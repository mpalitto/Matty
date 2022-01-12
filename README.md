# Matty
Arduino based robot

## The Resurrection
Bought in the USA a few years ago... as a personal toy, was left un-used for years (maybe 10 yrs)... and lost all documentation, but I kept it and even took to Italy (where I live now) for the emotional attachment I developed to it.
Recently I decided to use Arduino to teach Digital Systems to my students and Matty came back to my mind.

This is the story of bringing Matty back to life!

## Description
Matty is a little cute robot, originally it was connected to WiFi using a pocket size router made by TP-Link (I think it was a clone of the WR703N... but I m not sure...).
It is propelled by 2 engine wheels and a small pivot wheel for balance. It has a custom board that hosts an Arduino Mini Clone with ATmega168 MCU. The board has the motor H-Bridge for powering the motors and some circutry for power and connectors.
The power was originally provided by 2 x 3.7 LiON batteries which where used to power the engines and to get the 5Volt for the Arduino and the Servos.
2 x Servos are used to give digital camera the Pan & Tilt, while an Ultrasound sensor is used for obstacles detection.

Somehow the router was connected to the Arduino via USB-serial interface and used to control the robot from computer using a web-interface, which will show the buttons to control the robot and the streamed video as well.

I must have lost some cables, since the router was not connected when I took it out from its tomb... the battery were dead and needed replacement....

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

![image](https://user-images.githubusercontent.com/7433768/148441288-25dffbc9-9ef6-4339-bc62-5be37434ac21.png)


### The Mic
I am planning to use the Mic for providing vocal commands to Matty... Matty will be always listening... the Mic I have is INMP441 which gives a 24bit data trhrough an I2S interface... oops... I2S interface in not implemented in Arduino, beside a few models of them... OK then I am restriced in using an analog Mic and use the 8bit ADC embedded into the Arduino. I am planning on using 8KHz sampling rate (same as the speaker)... hope the quality will be sufficient...

### The ESP32 - Arduino data transfer Interface
Mic stream will need 8 bit x 8Khz = 64 Kbit/s (Mic --> Arduino --> ESP-CAM)
Speaker same as Mic but in opposit direction (ESP-CAM --> Arduino --> Speaker)
Other data... whatever is left.
Minimum data rate should be 160,000 bit/sec (which is a multiple of 16MHz which I suspect is the clock frequency of the ArtMEGA128)

Should I use the I2C interface instead?
It looks like all Arduino has the ability of implementing the I2C interface by using the Wire.h library and the pins A4 and A5 (analog pin that are connected to the I2C circuitry)... I am not sure that will work on the custom Arduino on Matty...
I2C has 2 different transmission speeds: standard (100Kbit/s) and fast (400Kbit/s).

Yeah! it does work :) I was successfull in getting the ESP the I2C Master and tha Arduino the Slave. I did setup the clock for 400Kbit/s and I did not have any issue!

it resulted in the ESPmaster_writer and slave_reader(Arduino) into the Matty/I2Cplayground. These 2 files get the Master to send some data to the Slave which will print the data to the Console. 6x8bits every ms, which results in 40kbit/sec. 

Now I am going to try connection both direction...

No, not working... I don't get any data the way back... I am not sure why...

OK... fixed... credit to https://github.com/saka-guchi/I2C_Between_Arduino_and_ESP32 and it works at 40Kbit/sec leaving the serial port available for debugging.

This resulted in the ``Matty/I2Cplayground/2-BidirectionTest`` folder.

If needed, I could use the I2C interface for data transfers from ESP-CAM --> Arduino and Serial Interface for transfers from the Arduino --> ESP-CAM effectevly doubling the data rate... we will see...

On Matty shield, looks like there is a connector for another serial port next to a USB female connector... could it be that there is another HW port? ... no luck in finding out which MCU the clone is using... I am lucky that it is compatible with the ATMEGA128... the only indication I have is that the board is made by www.mjkdz.com and the MCU is a squared uChip with 8pin per side... wait a moment, it could be a AVR128DB32... which in the datasheet has 3 USART... after a google search it looks like the DB series came out in 2020 wich it would not be possible... I probably bought it in 2012/13??? Anyhow, I could do without it.

Next will be to test I2C interface by sending a wave samples from the ESP-CAM --> Arduino and play it with the speaker.

The issue is that the Arduino is very limited as far as memory (14 KBytes)!

~~This is the strtategy I will implement: thus I would need to send and buffer a few samples (maybe 100 bytes) and as it starts playing the buffer(active) fillup another buffer(backup) and as long the backup buffer is filled before the active one has done playing, rotate the 2 buffers.

~~In order to limit the ESP-CAM to send too mach data, The Arduino will send a (needMore) message to ESP-CAM whenever the Arduino start sending the samples from a buffer. The ESP-CAM will then send another bulk of 100samples to the Arduino, which will use to fillUp the current backup buffer. 

~~The Arduino doesn't know how many samples are part of the same sound... thus it will stop playing, if by the time it finishes playing the ACTIVE buffer, the BACKUP buffer is still not been filled.

~~This will require that the handshake between the Arduino and the ESP-CAM is fast enogh to allow the BACKUP buffer to be filled in time...

~~In case there are 100 Samples, 100/8k = 0.0125 secs = 12.5ms is that going to be enough?... we will see.

~~Anyway, I found out that on each I2C message it can be sent up to 32 bytes of data... I will try limiting the buffer size to 30....

The previous strategy was a non sense (and it made me waste a lot of time trying implementing it)... What the architecture is going to be is to send data from ESP-CAM --> Arduino one byte at the time at a 8KHz rate, as soon as the Arduino receives the data will send it to the Speaker... as easy as that!!!

That resulted on the `Matty/I2Cplayground/streaming2speaker` folder.

Next it would be sending sound file from PC, and play it through the arduino..

With ESP-CAM as a TCP/IP Socket Server, I send a RAW file using the following command:
```
cat filename.raw eof.txt | nc -w 1 192.168.X.YYY port
```

NOTA: -w 1 option will close the connection after 1s of STDIN inactivity

The ESP-CAM will receive the RAW file and store it in Array  of bytes. EOF'\n' will be appended to the file (that's what "eol.txt" contains), WhenESP-CAM receives the EOF'\n', will start sending the samples to the Arduino and then to the Speaker.

#### I2C interface issue
This is getting annoing... it is taking me too long to figure this out...

PROBLEM: It looks like I2C has a stability issue on the ESP-CAM... it came out when trying to transfer a big chunk of data(audio file) across it... maybe I am doing something wrong but I am not sure what is it...

POSSIBLE SOLUTION1 (investigating): replace the I2C with the serial interface, which is annoying since every time I need to burn the firmware I need to disconnect and re-connect the serial connection... and I will not be able to use the serial Monitor for debugging (I am planning to use the WiFi connection)... and, maybe it is not gonna work anyhow... feeling disconfort right now.

POSSIBLE SOLUTION2 (will investigate): reading around the internet it looks like it could be caused by weak pull-up resistor in the ESP32... suggesting to insert 1K external pull-up resistor to make connection relible... 

POSSIBLE SOLUTION3 (already tryed, but resulted in other issues): by using ``Wire.endTransmission(false);``, By passing the "false" will instruct the ESP32 not to put the I2C bus into High-Impedence beteen I2C bus transmitions by the same master... this was done for keeping ownership of the bus by the same master in case a multi-master connection sharing the same bus... but somehow the Arduino stop firing the onReceiving() method...

## What is next
1. adding a speaker and the ability to play sounds
2. When the ESP-Cam arrives, replace the ESP-01 and allow video streaming
3. Add a mic Array for sound direction of arrival
4. Add AI for object recognition and tracking
5. House mapping based on object recognition
6. Making Matty interactive by using his ability for house patroling
7. more...
