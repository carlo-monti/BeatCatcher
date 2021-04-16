#include "definitions.h"

#include "Click.h"
#include "Onset.h"
#include "Sync.h"
#include "Tempo.h"

Click clk = Click(piezo0);
Onset ons = Onset(&clk, piezo0, piezo1);
Sync syn = Sync(&ons, &clk);
Tempo tmp = Tempo(&ons, &clk);

void setup() {
  Serial.begin(9600);
  setupPinsAndParameters();                 // from definitions.h
  clk.addSyncObject(&syn);                  // add pointer from Click to Sync obj
  clk.addTempoObject(&tmp);                 // add pointer from Click to Tempo obj
  clk.initializeClick();                    // initialize all the obj
  ons.initializeOnset();
  syn.initializeSync();
  tmp.initializeTempo();
}

void loop() {
  if(digitalRead(aKey)==0){
    Serial.println("Reset");
    delay(300);
    syn.initializeSync();
    clk.resetClick();
  }
  clk.updateClick();                        // update the Click obj
  if(ons.updateOnset()){                    // if there is an onset do:
    syn.updateSync();                       //    check for sync
    tmp.updateTempo();                      //    check for tempo change
  }
  syn.updateLayerAndGroove();               // update the sync obj for layer and groove
}
