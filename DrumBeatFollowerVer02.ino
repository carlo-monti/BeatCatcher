#include "definitions.h"

Click clk = Click(piezo0);
Onset ons = Onset(&clk, piezo0, piezo1);
Sync syn = Sync(&ons, &clk);
Tempo tmp = Tempo(&ons, &clk);

void setup() {
  Serial.begin(92600);
  setupPinsAndParameters(); // from definitions.h
  clk.initializeClick();
  ons.initializeOnset();
  syn.initializeSync();
  tmp.initializeTempo();
}

void loop() {
  
  if(digitalRead(aKey)==0){
    /*Serial.print(millis()); Serial.print(",");Serial.println(analogRead(piezo0));*/
    Serial.println("Reset");
    delay(300);
    clk.resetClick();
  }
  clk.updateClick();
  syn.updateLayer();
  if(ons.updateOnset()){
    syn.updateSync();
    tmp.updateTempo();
  }
}
