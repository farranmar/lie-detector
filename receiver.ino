#include "lie_detector.h"

void setup() {
  // put your setup code here, to run once:
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);

  // set up uart
  pinMode(uartInPin, INPUT);
  attachInterrupt(uartInPin, uartReceive, CHANGE);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(rBufStart != rBufEnd){
      noInterrupts();
      char received = rBuf[rBufStart];
      rBufStart = (rBufStart + 1) % rsBufSize;
      interrupts();
      // Serial.print("received :");
      // Serial.println(received);
      if(received == 'r'){
        digitalWrite(GREEN_PIN, LOW);
        digitalWrite(RED_PIN, HIGH);
      } else if(received == 'g'){
        digitalWrite(GREEN_PIN, HIGH);
        digitalWrite(RED_PIN, LOW);
      } else if(received == 'b'){
        digitalWrite(GREEN_PIN, HIGH);
        digitalWrite(RED_PIN, HIGH);
      } else if(received == 'n'){
        digitalWrite(GREEN_PIN, LOW);
        digitalWrite(RED_PIN, LOW);
      }
  }
}
