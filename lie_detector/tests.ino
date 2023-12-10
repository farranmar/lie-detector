// #include "lie_detector.h";
// #include "lie_detector.ino";

// to store all relevant fsm variables
typedef struct {
  double baseSkin;
  double baseHr;
  double threshSkin;
  double threshHr;
  double testSkin;
  double testHr;
  int greenLed;
  int redLed;
  int qSampleCount;
  int cumulativeSkin;
  int cumulativeHr;
  int baseBut;
  int qBut;
} state_vars;

bool testTransition(state startState,
                     state endState,
                     state_vars startStateVars,
                     state_vars endStateVars,
                     bool verbos);

// for printing states
char* s2str(state s) {
  switch(s) {
    case sDISP_LIE_RESULT:
    return "(1) DISP_LIE_RESULT";
    case sTEST_BASELINE:
    return "(2) TEST_BASELINE";
    case sTEST_LIE:
    return "(3) TEST_LIE";
    case sRECORD_LIE:
    return "(4) RECORD_LIE";
    default:
    return "???";
  }
}

/*
 * Given a start state, inputs, and starting values for state variables, tests that
 * updateFSM returns the correct end state and updates the state variables correctly
 * returns true if this is the case (test passed) and false otherwise (test failed)
 * 
 * Need to use "verbos" instead of "verbose" because verbose is apparently a keyword
 */
bool testTransition(state startState,
                     state endState,
                     state_vars startStateVars,
                     state_vars endStateVars,
                     bool verbos) {
  baseSkin = startStateVars.baseSkin;
  baseHr = startStateVars.baseHr;
  threshSkin = startStateVars.threshSkin;
  threshHr = startStateVars.threshHr;
  testSkin = startStateVars.testSkin;
  testHr = startStateVars.testHr;
  greenLed = startStateVars.greenLed;
  redLed = startStateVars.redLed;
  qSampleCount = startStateVars.qSampleCount;
  cumulativeSkin = startStateVars.cumulativeSkin;
  cumulativeHr = startStateVars.cumulativeHr;
  baseBut = startStateVars.baseBut;
  qBut = startStateVars.qBut;
  state resultState = updateFSM(startState);
  bool passedTest = (endState == resultState and
            (doubleEquals(baseSkin, endStateVars.baseSkin) or endStateVars.baseSkin == -1) and
            (doubleEquals(baseHr, endStateVars.baseHr) or endStateVars.baseHr == -1) and
            (doubleEquals(threshSkin, endStateVars.threshSkin) or endStateVars.threshSkin == -1) and
            (doubleEquals(threshHr, endStateVars.threshHr) or endStateVars.threshHr == -1) and
            (doubleEquals(testSkin, endStateVars.testSkin) or endStateVars.testSkin == -1) and
            (doubleEquals(testHr, endStateVars.testHr) or endStateVars.testHr == -1) and
            (greenLed == endStateVars.greenLed or endStateVars.greenLed == -1) and
            (redLed == endStateVars.redLed or endStateVars.redLed == -1) and
            (qSampleCount == endStateVars.qSampleCount or endStateVars.qSampleCount == -1) and
            (cumulativeSkin == endStateVars.cumulativeSkin or endStateVars.cumulativeSkin == -1) and
            (cumulativeHr == endStateVars.cumulativeHr or endStateVars.cumulativeHr == -1) and
            (baseBut == endStateVars.baseBut or endStateVars.baseBut == -1) and
            (qBut == endStateVars.qBut or endStateVars.qBut == -1));
  if (! verbos) {
    return passedTest;
  } else if (passedTest) {
    char sToPrint[200];
    sprintf(sToPrint, "Test from %s to %s PASSED", s2str(startState), s2str(endState));
    Serial.println(sToPrint);
    return true;
  } else {
    char sToPrint[200];
    Serial.println(s2str(startState));
    sprintf(sToPrint, "Test from %s to %s FAILED", s2str(startState), s2str(endState));
    Serial.println(sToPrint);
    sprintf(sToPrint, "End state expected: %s | actual: %s", s2str(endState), s2str(resultState));
    Serial.println(sToPrint);
    sprintf(sToPrint, "          %8s | %8s | %10s | %8s | %8s | %8s | %8s | %8s | %12s | %14s | %12s | %8s | %8s", "baseSkin", "baseHr", "threshSkin", "threshHr", "testSkin", "testHr", "greenLed", "redLed", "qSampleCount", "cumulativeSkin", "cumulativeHr", "baseBut", "qBut");
    Serial.println(sToPrint);
    sprintf(sToPrint, "expected: %8s | %8s | %10s | %8s | %8s | %8s | %8d | %8d | %12d | %14d | %12d | %8d | %8d", String(endStateVars.baseSkin, 5).c_str(), String(endStateVars.baseHr, 5).c_str(), String(endStateVars.threshSkin, 5).c_str(), String(endStateVars.threshHr, 5).c_str(), String(endStateVars.testSkin, 5).c_str(), String(endStateVars.testHr, 5).c_str(), endStateVars.greenLed, endStateVars.redLed, endStateVars.qSampleCount, endStateVars.cumulativeSkin, endStateVars.cumulativeHr, endStateVars.baseBut, endStateVars.qBut);
    Serial.println(sToPrint);
    sprintf(sToPrint, "actual:   %8s | %8s | %10s | %8s | %8s | %8s | %8d | %8d | %12d | %14d | %12d | %8d | %8d", String(baseSkin, 5).c_str(), String(baseHr, 5).c_str(), String(threshSkin, 5).c_str(), String(threshHr, 5).c_str(), String(testSkin, 5).c_str(), String(testHr, 5).c_str(), greenLed, redLed, qSampleCount, cumulativeSkin, cumulativeHr, baseBut, qBut);
    Serial.println(sToPrint);
    return false;
  }
}

/*
 * REPLACE THE FOLLOWING 6 LINES WITH YOUR TEST CASES
 */
const state testStatesIn[11] = {(state) 1, (state) 1, (state) 1, (state) 2, (state) 2, (state) 2, (state) 3, (state) 3, (state) 3, (state) 4, (state) 4};

const state testStatesOut[11] = {(state) 1, (state) 2, (state) 3, (state) 2, (state) 2, (state) 1, (state) 3, (state) 3, (state) 4, (state) 1, (state) 1};

const state_vars testVarsIn[11] = {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0}, {-1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 1, 0}, {4, 58, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 1}, {-1, -1, -1, -1, -1, -1, -1, -1, 6, 17, 124, 0, -1}, {-1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, -1, -1}, {-1, -1, -1, -1, -1, -1, -1, -1, 10, 60, 700, 1, -1}, {-1, -1, -1, -1, -1, -1, -1, -1, 3, 12, 42, -1, 0}, {-1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, -1, -1}, {-1, -1, -1, -1, -1, -1, -1, -1, 4, 12, 212, -1, 1}, {5, 60, 5.75, 69, 6, 72, -1, -1, -1, -1, -1, -1, -1}, {5, 60, 5.75, 69, 5, 67, -1, -1, -1, -1, -1, -1, -1}};

const state_vars testVarsOut[11] = {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1, -1, 1, 1, 1, 5, 60, 0, 0}, {-1, -1, -1, -1, -1, -1, 1, 1, 1, 5, 60, 0, 0}, {-1, -1, -1, -1, -1, -1, 1, 1, 7, 22, 184, -1, -1}, {-1, -1, -1, -1, -1, -1, 1, 1, 1, 5, 60, -1, -1}, {6, 70, 6.9, 80.5, -1, -1, 0, 0, -1, -1, -1, 0, 0}, {-1, -1, -1, -1, -1, -1, 1, 1, 4, 17, 102, -1, -1}, {-1, -1, -1, -1, -1, -1, 1, 1, 1, 5, 60, -1, -1}, {-1, -1, -1, -1, 3, 53, 0, 0, -1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1, -1, 0, 1, -1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1, -1, 1, 0, -1, -1, -1, -1, -1}};

const int numTests = 11;


// two doubles are equal if they differ by < 0.003
int doubleEquals(double a, double b) {
    long long ai = a * 1000;
    long long bi = b * 1000;
    return abs(ai - bi) < 3;
}

bool testWDT() {
  testTransition(testStatesIn[1], testStatesOut[1], testVarsIn[1], testVarsOut[1], true);
}

/*
 * Runs through all the test cases defined above
 */
bool testAllTests() {
  for (int i = 0; i < numTests; i++) {
    Serial.print("Running test ");
    Serial.println(i);
    if (!testTransition(testStatesIn[i], testStatesOut[i], testVarsIn[i], testVarsOut[i], true)) {
      return false;
    }
    Serial.println();
  }
  Serial.println("All tests passed!");
  return true;
}