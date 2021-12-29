/*
 * Visto che il piccolo TP-LINK router è fuori uso... ho provato ad usare ESP-01 per dare una connessione WIFI a Matty
 * 
 * ESP-01 ha un microcontroller esp8266 e comunica all'esterno con i comandi AT tramite seriale, tramite questi comandi posso:
 *  1. connettermi a WIFI
 *  2. stampare MAC e IP address
 *  3. avviare telnet server
 * 
 * ESP-01 lavora a 3.3V mentre Matty funzionerebbe a 8.4V con arduino a 5V...
 * Ho provato ad alimentare Matty a 5V con una battery Bank che offre i 5V da una uscita USB e il tutto sembra funzionare (a rilento) senza il bisogno di traslatori di livello
 * 
 * Visto che arduino usa la seriale per la connssione a terminale, ho dovuto usare una libreria "SoftwareSerial" per poter avere connessione ad entrambi il terminale e a ESP-01
 * 
 * Per collegarmi ad entrambi:
 *  1. uso "screen /dev/ttyUSB0 9600" per la connessione a terminale 
 *  2. uso "screen //telnet 192.168.1.142 80" per la connessione via WIFI
 *  
 *Questo codice può essere usato come punto di partenza per inviare comandi a Matty  
 */

#include <SoftwareSerial.h>                         //including the SoftwareSerial library will allow you to use the pin no. 2,3 as Rx, Tx.      
SoftwareSerial esp8266(2,3);                        //set the Rx ==> Pin 2; TX ==> Pin3.      
#define serialCommunicationSpeed 9600               // <========= define a constant named "serialCommunicationSpeed" with a value 9600. it referes to the Software and hardware serial communication speed(baud rate).      
#define DEBUG true                                  //make a constant named "DEBUG" and it's value true. we will use it later.      
int redLED =12;                                     //assign a variable named "redLED" with an integer value 12, it refers to the pin which the red LED is connected on.      
int blueLED =11;                                    //assign a variable named "blueLED" with an integer value 11, it refers to the pin which the blue LED is connected on.      
String received = "";                               //buffer usato per costruire il messaggio ricevuto da TCP/IP Socket
String serialread = "";                             //buffer usato per contruire il messaggio inserito da Terminale
char c;
int nLines = 0;
void setup()      
{      
  pinMode(redLED,OUTPUT);                           //set the pin number 12 as an output pin.      
  pinMode(blueLED,OUTPUT);                          //set the pin number 11 as an output pin.      
        
  digitalWrite(redLED,LOW);                         //turn the red LED off at the beginning of the program.      
  digitalWrite(blueLED,HIGH);                       //turn the blue LED on at the beginning of the program.      
  esp8266.begin(serialCommunicationSpeed);          //begin the software serial communication (2, 3) at speed 9600.      
  Serial.begin(serialCommunicationSpeed);           //begin the Hardware serial communication (0, 1) at speed 9600. 
  delay(1000);
  InitWifiModule();                                //call this user-defined function "InitWifiModule()" to initialize a communication between the ESP8266 and your access point (Home Router or even your mobile hotspot).      
//  testWIFImodule();     
  digitalWrite(blueLED,LOW);                         //after finishing the initialization successfully, turn off the blue LED (just an indicator).      
}      
void loop()                                          //our main program, some fun are about to start)      
{ 
  //check if there is anything from TCP/IP Socket
  String recvd = readLine(DEBUG); 
  if(recvd != "") {
    Serial.println("TCP/IP> "+recvd);
  }

  //see if any input from the DEBUG console in which case collect them until Return key is pressed and then send them over TCP/IP Socket
  int l = Serial.available();
  for(int i=0; i < l; i++) {
    char ch = Serial.read();
    Serial.print(ch); 
    
     if(ch == 13) { // Return key was pressed
      Serial.print('\n');
      sendLine(serialread,100);  //send it through TCP/IP Socket
      serialread = "";    
    }
    else serialread += ch;
  }
}    

  /******************************************************************************************************************************************************************************************      
* Name: sendLine      
* Description: send line/string trough softwareSerial connecting to ESP-01 which in instructed to send it through the TCP/IP socket using AT commands.      
*       

*/
String readLine(boolean debug) {

  int nChars = esp8266.available();   
  if(nChars > 0)                                           //if there's any data received and stored in the serial receive buffer, go and excute the if-condition body. If not, dont excute the if-condition body at all.      
  {    
   if(received == "" && esp8266.find('+'))   {  //vedo se trovo il segno di inizio risposta TCP/IP da modulo ESP0WIFI
    delay(100); //wait for some, hopefully all, data will join us
    //start reading the expected characters: +IPD,
    c = esp8266.read(); if(c == 'I') {
      c = esp8266.read(); if(c == 'P') {
        c = esp8266.read(); if(c == 'D') {
          c = esp8266.read(); if(c == ',') if(debug) Serial.print("Found IPD: ");
    }}} else return "";
    //next char is the connection ID (0 - 3)
    int connectionId = esp8266.read()-48;
    //next char is a ','
    esp8266.read(); 
    //calculate the lenght of the message(considering the lenght to be rappresented at most with 2 chars): (1stChar-48)[*10+2ndChar-48]
    c = esp8266.read(); int len = c-48; c = esp8266.read(); if(c != ':') { len *= 10; len += c-48; esp8266.read();}
    if(debug) Serial.print(len); 
    if(debug) Serial.print(" - reading: "); 
    //received="";

    nChars = esp8266.available();
    if(debug) Serial.print(nChars); 
    for(int i=0; i < nChars; i++)   {
      c = esp8266.read();
        //Serial.print(c); 
      if(c == 13) { 
          if(debug) Serial.println(" - Found: EoL");  //trovato il termine della linea
          if(debug) Serial.print("Recv: "+String(nLines++, DEC)+" : ");
          received += '\0';
          if(debug) Serial.println(received);
          String r = received;
          received = "";
          return r;
      } else received += c;
    }
    return ""; //non ho incontrato la terminazione della linea... ci riprovo più tardi...
                
   } } 
   return ""; //nulla nel buffer     
}
/******************************************************************************************************************************************************************************************      
* Name: sendLine      
* Description: send line/string trough softwareSerial connecting to ESP-01 which in instructed to send it through the TCP/IP socket using AT commands.      
*       
* Params: command - the LINE send       
*                 - timeout - the time to wait for a response       
*                       
* Returns: Nothing      
* Depeds on: sendData
*/      
void sendLine(String line, const int timeout)
{
  String len=String(line.length()+2,DEC);
  sendData("AT+CIPSEND=0,"+len+"\r\n",timeout,DEBUG);    // 
  //delay(1000);
  sendData(line+"\r\n",timeout,DEBUG);    //  

}

/******************************************************************************************************************************************************************************************      
* Name: sendData      
* Description: this Function regulates how the AT Commands will ge sent to the ESP8266.      
*       
* Params: command - the AT Command to send       
*                 - timeout - the time to wait for a response       
*                 - debug - print to Serial window?(true = yes, false = no)      
*                       
* Returns: The response from the esp8266 (if there is a reponse)      
*/      


String sendData(String command, const int timeout, boolean debug)      
{      
    String response = "";                                             //initialize a String variable named "response". we will use it later.      
          
    if(debug) Serial.println(command); 
    esp8266.print(command);                                           //send the AT command to the esp8266 (from ARDUINO to ESP8266).      
    long int time = millis();                                         //get the operating time at this specific moment and save it inside the "time" variable.      
    while( (time+timeout) > millis())                                 //excute only whitin 1 second.      
    {            
      while(esp8266.available())                                      //is there any response came from the ESP8266 and saved in the Arduino input buffer?      
      {      
        char c = esp8266.read();                                      //if yes, read the next character from the input buffer and save it in the "response" String variable.      
        response+=c;                                                  //append the next character to the response variabl. at the end we will get a string(array of characters) contains the response.      
      }        
    }          
    if(debug)                                                         //if the "debug" variable value is TRUE, print the response on the Serial monitor.      
    {      
      Serial.print(response);      
    }          
    return response;                                                  //return the String response.      
}      

/******************************************************************************************************************************************************************************************      
* Name: testWIFImodule      
*  Description: sends a simple AT command, if the module is working and connected it will answer with an OK
*/
void testWIFImodule()
{
  sendData("AT\r\n", 200, true);         //check the ESP8266 module.   
  sendData("AT+CIFSR\r\n", 500, true);  //Show IP Address, and the MAC Address.      
 
}

/******************************************************************************************************************************************************************************************      
* Name: InitWifiModule      
* Description: this Function gives the commands that we need to send to the sendData() function to send it.      
*       
* Params: Nothing.      
*                       
* Returns: Nothing (void).      
*/      
void InitWifiModule()      
{      
  sendData("AT+RST\r\n", 2000, DEBUG);                                                  //reset the ESP8266 module.      
  delay(1000);      
  sendData("AT+CWMODE=1\r\n", 1500, DEBUG);                                             //set the ESP8266 WiFi mode to station mode.      
  delay (1000);      
  //sendData("AT+CWJAP=\"dd-wrt\",\"\"\r\n", 2000, DEBUG);        //connect to the WiFi network.      
  delay (3000);      
  sendData("AT+CIFSR\r\n", 1500, DEBUG);                                                //Show IP Address, and the MAC Address.      
  delay (1000);      
  sendData("AT+CIPMUX=1\r\n", 1500, DEBUG);                                             //Multiple conections.      
  delay (1000);      
  sendData("AT+CIPSERVER=1,80\r\n", 1500, DEBUG);                                       //start the communication at port 80, port 80 used to communicate with the web servers through the http requests.      
  delay (100);      
 }      
