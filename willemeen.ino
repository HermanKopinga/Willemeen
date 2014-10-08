// Herman Kopinga cobbled this together. (herman@kopinga.nl)


int latchPin = 10;
int clockPin = 11;
int dataPin = 12;

unsigned long screenbuffer[2][7][2];

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
 
  screenbuffer[activebuffer][0][0] = 2863310165;
  screenbuffer[activebuffer][1][0] = 1431653034;
  screenbuffer[activebuffer][2][0] = 2863310165;
  screenbuffer[activebuffer][3][0] = 1431653034;
  screenbuffer[activebuffer][4][0] = 2863310165;
  screenbuffer[activebuffer][5][0] = 1431653034;
  screenbuffer[activebuffer][6][0] = 2863310165;

}

void loop() {
  updatescreen();

//  if (millis() %10 == 0) {
//    updatebuffer();
//  }
}

void updatescreen() {
  // 7 rows
  for (int i=0 ; i < 7; i++) {

    // Update row.
    shiftOut(dataPin, clockPin, MSBFIRST, (screenbuffer[activebuffer][i][0] >> 24)); 
    shiftOut(dataPin, clockPin, MSBFIRST, (screenbuffer[activebuffer][i][0] >> 16)); 
    shiftOut(dataPin, clockPin, MSBFIRST, (screenbuffer[activebuffer][i][0] >> 8)); 
    shiftOut(dataPin, clockPin, MSBFIRST, screenbuffer[activebuffer][i][0]); 
    
    // Turn off row.
    if (i == 0) {
      digitalWrite(19, 1);
    }
    else {   
      digitalWrite(i + 12, 1);
    }    
    // Flick the latch
    digitalWrite(latchPin, 1);
    delay(1);
    digitalWrite(latchPin, 0);
    // Turn on next row.
    digitalWrite(i + 13, 0);    
    
    //delay(100);        
  }
}


void updatebuffer() {
 // if (screenbuffer[activebuffer][0][0] > 0)
 // {
 //   screenbuffer[activebuffer][0][0] = screenbuffer[activebuffer][0][0] <<1; // shift the outputpattern left by one bit
 // } 
 // else {
//    screenbuffer[activebuffer][0][0] = 2863310165;
//  }
 
}
