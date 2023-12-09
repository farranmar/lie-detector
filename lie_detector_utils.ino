#include "lie_detector.h"

void resetButtons() {
    noInterrupts();
    baseBut = 0;
    qBut = 0;
    interrupts();
}

void base_isr(){
    baseBut = 1;
}

void q_isr(){
    qBut = 1;
}

void writeToRBuf(char c){
  noInterrupts();
  if((rBufEnd + 1) % rsBufSize != rBufStart){
    rBuf[rBufEnd] = (byte) c;
    rBufEnd = (rBufEnd + 1) % rsBufSize;
  } else {
    Serial.println("RBUF FULL");
  }
  interrupts();
}


#ifndef TESTING
// get the current heart rate and skin values and add them to the global variables: cumulativeHr, cumulativeSkin
// @TODO: implement this
void sampleData(){
    
}
#else
// if testing, assume the heart rate is constantly 60 and the skin value is constantly 5
void sampleData(){
    cumulativeHr += 60;
    cumulativeSkin += 5;
}
#endif