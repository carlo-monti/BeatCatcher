#include "definitions.h"

Click clk = Click(piezo2,aKey);
Onset ons = Onset(clk.tau, piezo1);
Sync syn = Sync(ons);
//Tempo tmp = Tempo(clk.tau);

void setup() {
  Serial.begin(9600);
  setupPinsAndParameters();
  ons.initializeOnset();
  clk.setTau(1000);
}

void loop() {
  ons.updateOnset();
  //clk.updateClick();
}

void printOnsetsArrayc(){
  Serial.println("----------");
  for(int i=0;i<20;i++){
    if(i==ons.last){
      Serial.println("last ");
    }
    if(i==ons.next){
      Serial.println("next ");
    }
    Serial.println(ons.onsets[i]);
  }
} // debug
