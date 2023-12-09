// #define TESTING

// pins
const int uartInPin = 5;
const int uartOutPin = 3;
const int GREEN_PIN = 4;
const int RED_PIN = 5;
const int BASE_BUT_PIN = 6;
const int Q_BUT_PIN = 7;

// receive buffer for uart communication
// we don't need a send buffer because we just send right away
const int rsBufSize = 80;
byte rBuf[rsBufSize];
int rBufStart = 0;
int rBufEnd = 0;

const int UART_PERIOD_MICROS = 104;

// typedef for fsm states
typedef enum {
  sDISP_LIE_RESULT = 1,
  sTEST_BASELINE = 2,
  sTEST_LIE = 3,
  sRECORD_LIE = 4,
} state;

// fsm variables that we want to be truly global
int cumulativeHr, cumulativeSkin, baseBut, qBut;

// uart helper functions
void uartReceive();
void uartSend(byte B);
void writeToRBuf(char c);

// isrs
void q_isr();
void base_isr();

// fsm helpers
void sampleData();
void resetButtons();
state updateFSM(state curState, long mils, int numButtons, int lastButton);

// testing
bool testAllTests();