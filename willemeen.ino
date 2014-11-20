// Herman Kopinga cobbled this together. (herman@kopinga.nl)
// Controller for Datalite  90ies era LED display.
// LED boards are matrixed 7x5 leds, 12 connections each.
// Driver board drives 8 LED boards with:
// Columns: 5 4094 multiplexers with 62083 darlington arrays to sink current.
// Rows: 7 individual lines through a 62083 and TIP122 darlington transistor.

// Stood on the shoulders of:
// http://www.openobject.org/opensourceurbanism/Bike_POV_Beta_4 // Font renderer, jay!
// http://playground.arduino.cc/Code/BitMath // Bit manipulation hurts my brain!

// Challenge: 5*7*56*50*16 = 1568000 bps to the shift registers.
// 800 frame updates aka interrupts / second.

#include "font.h"

int latchPin = 10;
int clockPin = 11;
int dataPin = 12;

unsigned long screenbuffer[2][7][5];

long interval = 1000;           // interval at which to 'hop' (milliseconds)
unsigned long previousMillis = 0;

int incomingByte = 0;   // for incoming serial data
int incomingPos = 0;

byte activebuffer = 0;
byte passivebuffer = 1;

void setup() {
 pinMode(latchPin, OUTPUT);
 pinMode(clockPin, OUTPUT);
 pinMode(dataPin, OUTPUT);
 pinMode(13, OUTPUT);
 pinMode(14, OUTPUT);
 pinMode(15, OUTPUT);
 pinMode(16, OUTPUT);
 pinMode(17, OUTPUT);
 pinMode(18, OUTPUT);
 pinMode(19, OUTPUT); 
 digitalWrite(latchPin, 0); //make sure data isn't latched
 digitalWrite(13, 1);
 digitalWrite(14, 1);
 digitalWrite(15, 1);
 digitalWrite(16, 1);
 digitalWrite(17, 1);
 digitalWrite(18, 1);
 digitalWrite(19, 1);
 Serial.begin(115200);
 /*
  screenbuffer[activebuffer][0][0] = 0xAAAAAAAA;
  screenbuffer[activebuffer][1][0] = 0x55555555;
  screenbuffer[activebuffer][2][0] = 0xAAAAAAAA;
  screenbuffer[activebuffer][3][0] = 0x55555555;
  screenbuffer[activebuffer][4][0] = 0xAAAAAAAA;
  screenbuffer[activebuffer][5][0] = 0x55555555;
  screenbuffer[activebuffer][6][0] = 0xAAAAAAAA;
/*
screenbuffer[activebuffer][0][0] = 0x10011;
screenbuffer[activebuffer][1][0] = 0x1001B;
screenbuffer[activebuffer][2][0] = 0x939D5;
screenbuffer[activebuffer][3][0] = 0x54211;
screenbuffer[activebuffer][4][0] = 0x37BD1;
screenbuffer[activebuffer][5][0] = 0x54631;
screenbuffer[activebuffer][6][0] = 0x97BD1;*/
 /* printChar('M',0);
  printChar('a',5);
  printChar('a',10);
  printChar('k',15);
  printChar('p',20);
  printChar('l',25);
  printChar('e',30);  
  printChar('k',35);    
  printChar('j',40);    
  printChar('e',45);    */ 
  char tekst[29] = "Maakplek";
  printString(tekst, 0, activebuffer);
}

void loop() {
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   
    // if the LED is off turn it on and vice-versa:
    //updatebuffer();

  }  
  updatescreen();

  communicate();
}

void communicate () {
  // send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();
 
    printChar(incomingByte, incomingPos, activebuffer);
    incomingPos = incomingPos + 5;
    if (incomingPos > 28*5) {
              incomingPos = 0;
    }
  }
}
        

void updatescreen() {
  // 7 rows
  for (int i=0 ; i < 7; i++) {

    
/*    shiftOut(dataPin, clockPin, LSBFIRST, screenbuffer[activebuffer][i][1]);    
    shiftOut(dataPin, clockPin, LSBFIRST, screenbuffer[activebuffer][i][0]); 
    shiftOut(dataPin, clockPin, LSBFIRST, (screenbuffer[activebuffer][i][0] >> 8)); 
    shiftOut(dataPin, clockPin, LSBFIRST, (screenbuffer[activebuffer][i][0] >> 16));
    shiftOut(dataPin, clockPin, LSBFIRST, (screenbuffer[activebuffer][i][0] >> 24)); */
    
    // Update row.
    for (int j = 5; j >= 0; j--) {
      shiftOut(dataPin, clockPin, LSBFIRST, screenbuffer[activebuffer][i][j]); 
      shiftOut(dataPin, clockPin, LSBFIRST, (screenbuffer[activebuffer][i][j] >> 8)); 
      shiftOut(dataPin, clockPin, LSBFIRST, (screenbuffer[activebuffer][i][j] >> 16));
      shiftOut(dataPin, clockPin, LSBFIRST, (screenbuffer[activebuffer][i][j] >> 24)); 
    }

    // Turn off row.
    if (i == 0) {
      digitalWrite(19, 1);
    }
    else {   
      digitalWrite(i + 12, 1);
    }    
    // Flick the latch
    digitalWrite(latchPin, 1);
  //  delay(1);
    digitalWrite(latchPin, 0);
    // Turn on next row.
    digitalWrite(i + 13, 0);    
  }
}

void printChar(char ch, byte charPos, byte bufferpointer)
{  
  // The display is 40 bits wide, the buffer only 32. Time to select the correct one.
  int bufferpos = 0;
  
  // make sure the character is within the alphabet bounds (defined by the font.h file)
  // if it's not, make it a blank character
  if (ch < 32 || ch > 126){
    ch = 32;
  }
  
  // subtract the space character (converts the ASCII number to the font index number)
  ch -= 32;
  // step through each byte of the character array
  for (int charCol=0; charCol<5; charCol++) {
    byte b = font[ch][charCol];

    bufferpointer = (charCol+charPos)/32;
    bufferpos = charPos - (32 * bufferpointer);
        
    //  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39
    // 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0 
    //                                                                                                 31 30 29 28 27 26 25 24
    //                                                                                                  7  6  5  4  3  2  1  0

    
    // bit shift through the byte, set it in the buffer
    for (int row=0; row<7; row++) {
      if (b & (1 << row)) {        
        bitSet(screenbuffer[activebuffer][row][bufferpointer], 31-charCol-bufferpos);
      }
      else {
        bitClear(screenbuffer[activebuffer][row][bufferpointer], 31-charCol-bufferpos);
      }
    }
  }
}

void printString(char* source, byte pos, char bufferpointer) {
  int i;
  for (i = 0; i < strlen(source); i++){
    printChar(source[i], pos+i*5, bufferpointer);
  }
}

void updatebuffer() {
  for (int col=0; col<=4; col++) {
    for (int row=0; row<7; row++) {
      screenbuffer[passivebuffer][row][col] = screenbuffer[activebuffer][row][col-1];
      /*if (b & (1 << row)) {        
        bitSet(screenbuffer[activebuffer][row][bufferpointer], 31-charCol-bufferpos);
      }
      else {
        bitClear(screenbuffer[activebuffer][row][bufferpointer], 31-charCol-bufferpos);
      }     */ 
    }
  }
  switchbuffer();
}

void switchbuffer() {
  activebuffer = activebuffer xor passivebuffer;
  passivebuffer = activebuffer xor passivebuffer;
  activebuffer = activebuffer xor passivebuffer;
}
