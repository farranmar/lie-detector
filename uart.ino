#include "lie_detector.h"

/*
 * Manually wait for UART cycle
 */
inline void uartDelay(unsigned long ltime) {
  // delayMicroseconds works in interrupts
  delayMicroseconds(UART_PERIOD_MICROS - (micros() - ltime));
}

/*
 * Send byte via UART
 */
void uartSend(byte B) {
  
  // send start bit
  digitalWrite(uartOutPin, LOW);
  unsigned long lastClockTime = micros(); // record when pin was flipped
  int i = 0;
  byte parity = 0;
  while (i < 8) {
    if ((B & 0x1) == 0x1) {
      uartDelay(lastClockTime);
      digitalWrite(uartOutPin, HIGH);
      lastClockTime = micros();
      parity = parity ^ 0x01;
    } else {
      uartDelay(lastClockTime);
      digitalWrite(uartOutPin, LOW);
      lastClockTime = micros();
    }
    B = B >> 1;
    i += 1;
  }
  uartDelay(lastClockTime);
  // write parity bit;
  if (parity == 1) {
    digitalWrite(uartOutPin, HIGH);
  } else {
    digitalWrite(uartOutPin, LOW);
  }
  uartDelay(micros());
  // back to high
  digitalWrite(uartOutPin, HIGH);
  
}

/*
 * Read a byte from the UART and store in the rBuf
 */
void uartReceive() {
  // delay for 1/3 of the UART period just to get reads towards the middle of bits
  delayMicroseconds(UART_PERIOD_MICROS / 3);
  unsigned long lastClockTime = micros();
  int i = 0;
  byte B = 0;
  byte parity = 0;
  while (i < 8) {
    B = B >> 1;
    uartDelay(lastClockTime);
    int inPinVal = digitalRead(uartInPin);
    lastClockTime = micros();
    if (inPinVal == HIGH) {
      B = B | (0x1 << 7);
      parity = parity ^ 0x01;
    }
    i += 1;
  }
  // Receive parity bit
  uartDelay(lastClockTime);
  int inPinVal = digitalRead(uartInPin);
  lastClockTime = micros();
  // compare computed and received parity
  // if match, put value at end of rBuf
  if(inPinVal == parity){
    if((char) B == 'r' | (char) B == 'g' | (char) B == 'b' | (char) B == 'n'){
      writeToRBuf((char) B);
    }
  }

  // get past this last bit so as not to trigger an early interrupt
  uartDelay(lastClockTime);
}