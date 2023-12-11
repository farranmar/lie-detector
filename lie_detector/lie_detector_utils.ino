// #include "lie_detector.h"

void resetButtons() {
    noInterrupts();
    baseBut = 0;
    qBut = 0;
    interrupts();
}

void base_isr(){
    Serial.println("base isr");
    baseBut = 1;
}

void q_isr(){
    Serial.println("q isr");
    qBut = 1;
}

void writeToRBuf(char c){
  Serial.println("writing to rBuf, turning off interrupts");
  noInterrupts();
  if((rBufEnd + 1) % rsBufSize != rBufStart){
    rBuf[rBufEnd] = (byte) c;
    rBufEnd = (rBufEnd + 1) % rsBufSize;
    Serial.print("just wrote to rBuf, so we have rBufStart = ");
    Serial.print(rBufStart);
    Serial.print(" and rBufEnd = ");
    Serial.println(rBufEnd);
  } else {
    Serial.println("RBUF FULL");
  }
  interrupts();
  Serial.println("finished writing to rBuf, turning interrupts back on");
}



#if defined(TESTING)
// if testing, assume the heart rate is constantly 60 and the skin value is constantly 5
void sampleData(){
    cumulativeHr += 60;
    cumulativeSkin += 5;
}
#elif defined(WDT_TESTING)
// if testing wdt, delay for a long period (1 min) to simulate a system hang
// wdt should trigger system shutdown after about 16 seconds
void sampleData(){
    Serial.println("about to simulate hang (if a failure message does not print in one minute, wdt test passed)");
    delay(60000);
    Serial.println("wdt test failed :(");
}
#else
// get the current heart rate and skin values and add them to the global variables: cumulativeHr, cumulativeSkin
// @TODO: implement this
void sampleData(){
    // if (pulseSensor.sawStartOfBeat()) {
    //   Serial.println("hey");
    //   cumulativeHr += pulseSensor.getBeatsPerMinute();
    //   cumulativeSkin += analogRead(SKIN_PIN);
    //   Serial.print("hr: ");
    //   Serial.println(cumulativeHr);
    //   Serial.print("skin: ");
    //   Serial.println(cumulativeSkin);
    // }
    cumulativeHr += random(50, 100);
    cumulativeSkin += random(5, 200);
}
#endif