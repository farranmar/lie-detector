#include "lie_detector.h"
#define SENDER;

// what percent their heart rate/skin has to increase by to be considered a lie
// should this be different for hr vs skin??
const double thresh_percent = 0.15;

// FSM variables
static double baseSkin, baseHr, threshSkin, threshHr, testSkin, testHr;
static int greenLed, redLed, qSampleCount;

void setup() {
  Serial.begin(9600);
  // Serial.begin(115200);
  while (!Serial);

  // set up wdt (for both sender and recevier)
  // clear and enable WDT
  NVIC_DisableIRQ(WDT_IRQn);
  NVIC_ClearPendingIRQ(WDT_IRQn);
  NVIC_SetPriority(WDT_IRQn, 0);
  NVIC_EnableIRQ(WDT_IRQn);
  // configure and enable WDT GCLK:
  GCLK->GENDIV.reg = GCLK_GENDIV_DIV(4) | GCLK_GENDIV_ID(5);
  while (GCLK->STATUS.bit.SYNCBUSY);
  // set GCLK->GENCTRL.reg and GCLK->CLKCTRL.reg
  GCLK->GENCTRL.reg = GCLK_GENCTRL_DIVSEL | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC(3) | GCLK_GENCTRL_ID(5);
  while (GCLK->STATUS.bit.SYNCBUSY);
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_GEN(5) | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_ID(3);
  // while (GCLK->STATUS.bit.SYNCBUSY);
  // configure and enable WDT to have a period of 16384 (I think like 16 seconds at 1Hz?):
  WDT->CONFIG.reg = WDT_CONFIG_PER(0xB);
  WDT->EWCTRL.reg = WDT_EWCTRL_EWOFFSET_8;
  WDT->CTRL.bit.ENABLE = 1;
  while (WDT->STATUS.bit.SYNCBUSY);
  WDT->INTENSET.bit.EW = 1;


  #ifdef SENDER // sender setup
  // initialize fsm variables
  baseSkin = 0;
  baseHr = 0;
  threshSkin = 0;
  threshHr = 0;
  baseBut = 0;
  qBut = 0;
  testSkin = 0;
  testHr = 0;
  greenLed = 0;
  redLed = 0;
  cumulativeSkin = 0;
  cumulativeHr = 0;
  qSampleCount = 0;
  // pulseSensor.analogInput(HR_PIN);
  // pulseSensor.setSerial(Serial);
  // pulseSensor.setThreshold(THRESHOLD);
  pinMode(BASE_BUT_PIN, INPUT);
  pinMode(Q_BUT_PIN, INPUT);

  // Set up interrupts for buttons
  attachInterrupt(digitalPinToInterrupt(BASE_BUT_PIN), base_isr, RISING);
  attachInterrupt(digitalPinToInterrupt(Q_BUT_PIN), q_isr, RISING);

  // set up uart
  pinMode(uartOutPin, OUTPUT);
  digitalWrite(uartOutPin, HIGH);

  // error checking pulseSensor
  // if (!pulseSensor.begin()) {
  //   for (;;) {
  //     digitalWrite(LED_BUILTIN, LOW);
  //     delay(50);
  //     Serial.println("Pulse Sensor Initialization Failed!!!!!");
  //     digitalWrite(LED_BUILTIN, HIGH);
  //     delay(50);
  //   }
  // }
  #else // receiver setup
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);

  // set up uart
  pinMode(uartInPin, INPUT);
  attachInterrupt(uartInPin, uartReceive, CHANGE);
  #endif

  // run tests if applicable
  #if defined(TESTING) 
  testAllTests();
  #elif defined(WDT_TESTING)
  testWDT();
  #endif
}

// sends a message to the receiver arduino to display the appropriate leds
void displayLeds(int greenLed, int redLed) {
  if(!greenLed and !redLed){ uartSend('n'); }
  else if (greenLed and !redLed) { uartSend('r'); }
  else if (!greenLed and redLed) { uartSend('g'); }
  else { uartSend('b'); }
}

void loop() {
  // pet wdt
  WDT->CLEAR.reg = WDT_CLEAR_CLEAR(0xA5);
  #ifdef SENDER // sender loop
  static state CURRENT_STATE = sDISP_LIE_RESULT;
  Serial.println(qBut);
  CURRENT_STATE = updateFSM(CURRENT_STATE);
  delay(10);
  #else // receiver loop
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
  #endif
}

// @TODO: set up WDT and pet on each fsm update
state updateFSM(state curState) {
  state nextState;
  switch(curState) {
  case sDISP_LIE_RESULT:
    if (baseBut == 0 and qBut == 0) { // transition 1-1
      Serial.println("1-1");
      displayLeds(greenLed, redLed);
      nextState = sDISP_LIE_RESULT;
    } else if (baseBut == 1 and qBut == 0){ // transition 1-2
      Serial.println("1-2");
      displayLeds(1,1);
      resetButtons();
      cumulativeHr = 0;
      cumulativeSkin = 0;
      sampleData();
      qSampleCount = 1;
      greenLed = 1;
      redLed = 1;
      nextState = sTEST_BASELINE;
    } else if(baseBut == 0 and qBut == 1 and baseHr != 0 and baseSkin != 0){ // transition 1-3
      Serial.println("1-3");
      displayLeds(1,1);
      resetButtons();
      cumulativeHr = 0;
      cumulativeSkin = 0;
      sampleData();
      qSampleCount = 1;
      greenLed = 1;
      redLed = 1;
      nextState = sTEST_LIE;
    } else {
      nextState = sDISP_LIE_RESULT;
    }
    break;
  case sTEST_BASELINE:
    if (baseBut == 0 or qSampleCount <= 0) { // transition 2-2
      Serial.println("2-2");
      displayLeds(1,1);
      greenLed = 1;
      redLed = 1;
      sampleData();
      qSampleCount += 1;
      nextState = sTEST_BASELINE;
    } else if (baseBut == 1 and qSampleCount > 0) { // transition 2-1
      Serial.println("2-1");
      displayLeds(0,0);
      greenLed = 0;
      redLed = 0;
      resetButtons();
      baseHr = 1.0 * cumulativeHr / qSampleCount;
      threshHr = baseHr * (1.0 + thresh_percent);
      baseSkin = 1.0 * cumulativeSkin / qSampleCount;
      threshSkin = baseSkin * (1.0 + thresh_percent);
      nextState = sDISP_LIE_RESULT;
    } else {
      nextState = sTEST_BASELINE;
    }
    break;
  case sTEST_LIE:
    if(qBut == 0 or qSampleCount <= 0) { // transition 3-3
      Serial.println("3-3");
      displayLeds(1,1);
      greenLed = 1;
      redLed = 1;
      sampleData();
      qSampleCount += 1;
      nextState = sTEST_LIE;
    } else if (qBut == 1 and qSampleCount > 0) { // transition 3-4
      Serial.println("3-4");
      displayLeds(0,0);
      greenLed = 0;
      redLed = 0;
      resetButtons();
      testHr = 1.0 * cumulativeHr / qSampleCount;
      testSkin = 1.0 * cumulativeSkin / qSampleCount;
      nextState = sRECORD_LIE;
    } else {
      nextState = sTEST_LIE;
    }
    break;
  case sRECORD_LIE:
    if(testHr > threshHr and testSkin > threshSkin) { // transition 4-1 (a)
      Serial.println("4-1 a");
      displayLeds(0,1);
      greenLed = 0;
      redLed = 1;
      nextState = sDISP_LIE_RESULT;
    } else { // transition 4-1 (b)
      Serial.println("4-1 b");
      displayLeds(1,0);
      greenLed = 1;
      redLed = 0;
      nextState = sDISP_LIE_RESULT;
    }
    break;
  }
  return nextState;
}