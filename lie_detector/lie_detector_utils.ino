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

#if defined(TESTING)
// if testing, assume the heart rate is constantly 60 and the skin value is constantly 5
bool sampleData(){
    cumulativeHr += 60;
    cumulativeSkin += 5;
    return true;
}
#elif defined(WDT_TESTING)
// if testing wdt, delay for a long period (30 sec) to simulate a system hang
// wdt should trigger system shutdown after about 16 seconds
bool sampleData(){
    Serial.println("about to simulate hang (if a failure message does not print in one minute, wdt test passed)");
    delay(30000);
    Serial.println("wdt test failed :(");
}
#else
// get the current heart rate and skin values and add them to the global variables: cumulativeHr, cumulativeSkin
bool sampleData(){
    if(pulseSensor.sawNewSample()){
      if(--samplesUntilReport == (byte) 0){
        samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
        pulseSensor.outputSample();
        int bpm = pulseSensor.getBeatsPerMinute();
        int skin = analogRead(SKIN_PIN);
        if(bpm != 0 and skin > 1){
          cumulativeHr += bpm;
          cumulativeSkin += skin;
          return true;
        }
      }
    }
    return false;
}
#endif