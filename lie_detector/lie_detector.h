#define USE_ARDUINO_INTERRUPTS false
#include <PulseSensorPlayground.h>
// #define TESTING
// #define WDT_TESTING

// pins
const int uartInPin = 5;
const int uartOutPin = 3;
const int GREEN_PIN = 0;
const int RED_PIN = 1;
const int BASE_BUT_PIN = 6; // can also move to 4 if needed
const int Q_BUT_PIN = 7;
const int HR_PIN = A0;
const int SKIN_PIN = A5;

// heart rate read threshold to avoid noise
const int THRESHOLD = 450;

// typedef for fsm states
typedef enum {
  sDISP_LIE_RESULT = 1,
  sTEST_BASELINE = 2,
  sTEST_LIE = 3,
  sRECORD_LIE = 4,
} state;

// fsm variables that we want to be truly global
int cumulativeHr, cumulativeSkin, baseBut, qBut;
PulseSensorPlayground pulseSensor;
byte samplesUntilReport;
const byte SAMPLES_PER_SERIAL_SAMPLE = 10;

// isrs
void q_isr();
void base_isr();

// fsm helpers
bool sampleData();
void resetButtons();
state updateFSM(state curState, long mils, int numButtons, int lastButton);

// testing
bool testAllTests();