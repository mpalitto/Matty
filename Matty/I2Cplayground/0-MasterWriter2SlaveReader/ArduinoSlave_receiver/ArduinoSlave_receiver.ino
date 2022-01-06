
String serialread = "";

//byte sine[] = {127, 134, 142, 150, 158, 166, 173, 181, 188, 195, 201, 207, 213, 219, 224, 229, 234, 238, 241, 245, 247, 250, 251, 252, 253, 254, 253, 252, 251, 250, 247, 245, 241, 238, 234, 229, 224, 219, 213, 207, 201, 195, 188, 181, 173, 166, 158, 150, 142, 134, 127, 119, 111, 103, 95, 87, 80, 72, 65, 58, 52, 46, 40, 34, 29, 24, 19, 15, 12, 8, 6, 3, 2, 1, 0, 0, 0, 1, 2, 3, 6, 8, 12, 15, 19, 24, 29, 34, 40, 46, 52, 58, 65, 72, 80, 87, 95, 103, 111, 119,};
byte soundBuff[2][100];
byte activeBuff = 0;
int t = 0;
int cnt = 0;

ISR(TIMER0_COMPA_vect){ //40kHz interrupt routine
  //PORTD = sine[t];//send sine wave to DAC, centered around (127/255)*5 = 2.5V
  analogWrite(19,soundBuff[activeBuff][t]);
  t++;//increment t
  if (t > 99){//reset t to zero
    t = 0;
    cnt++;
    if(cnt >= 99) {
      cnt = 0;
      Serial.println("sound was played");
      cli();//disable interrupts
      TIMSK0 &= ~(1 << OCIE0A); //turn off timer interrupt
      sei();//enable interrupts
    }
  }
}

void setup() {
  Serial.begin(115200);           // start serial for output

//8KHz interrupt  for audio processing
  cli();//disable interrupts
  //set timer0 interrupt at 40kHz
  TCCR0A = 0;// set entire TCCR0A register to 0
  TCCR0B = 0;// same for TCCR0B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 40khz increments
  OCR0A = 245; //8KHz sample rate  //49;// = (16*10^6) / (40000*8) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS11 bit for 8 prescaler
  TCCR0B |= (1 << CS11); 
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);
  sei();//enable interrupts

  for(int b = 0; b<2; b++)
    for(int n = 0; n<100;  n++)
      soundBuff[b][n] = 1;
  Serial.print(soundBuff[1][99]);

  
  Serial.println("Device: Ready");
}


void playActiveBuffer() {
  
      cli();//disable interrupts
      TIMSK0 |= (1 << OCIE0A);
      sei();//enable interrupts}
}

boolean ON = true;
boolean recving = false; //when false Arduino is wating for cmd: 1st_byte "cmd" 2nd_byte "N. of byte" 
char cmd = "";
int NoB = 0;
int byteN = 0;

void loop() {
 int l = Serial.available();
 if(l >=2) {
  if(recving == false) { //Arduino is wating for cmd: 1st_byte "cmd" 2nd_byte "N. of byte" 
    cmd = Serial.read();
    NoB = Serial.read();
    Serial.println("Received CMD: "+String(cmd) + " NoBytes: "+String(NoB,DEC));
    Serial.println(" ");
    l -= 2;
    recving = true;
  } else {
    Serial.println("processing nBytes: "+String(l,DEC));
    //see if any input from the DEBUG console in which case collect them until Return key is pressed and then send them over TCP/IP Socket
    for(int i=0; i < l; i++) {
      char ch = Serial.read();
      //Serial.print(ch); 
      
       //if(ch == 13 || ch == '\n') { // Return key was pressed
       if(ch == '\n' | ch == '\r') { // Return key was pressed
        Serial.println("Arduino: "+String(NoB,DEC));
        if(byteN == NoB) playActiveBuffer();
        else Serial.println("Error: got "+String(byteN,DEC)+" bytes, Expecting: "+String(NoB,DEC));
        byteN = 0;
        //serialread = "";
        recving = false;
       } else soundBuff[activeBuff][byteN++] = ch; //serialread += ch;
    } 
  }
 } 


  
//  if(Serial.available()) {
//    Serial.read();
//    Serial.println(ON);
//    ON = !ON;
//    Serial.println(ON);
//    if(ON) {
//      cli();//disable interrupts
//      TIMSK0 |= (1 << OCIE0A);
//      sei();//enable interrupts
//    } else {
//      Serial.println("OFF");
//
//      cli();//disable interrupts
//      TIMSK0 &= ~(1 << OCIE0A);
//      sei();//enable interrupts
//    }
//  }

}
