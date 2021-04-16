#include "Click.h"
#include "Sync.h"
#include "Tempo.h"

Click::Click(int piezo){
  _piezoPin = piezo;
  _bounceTime = 250;
  _threshold = 40;
  for(int i=0; i<16; i++){
    _deltaTauLatency[i] = 0;
  }
  layerOf[0], layerOf[8] = 3;
  layerOf[1], layerOf[9] = 0;
  layerOf[2], layerOf[10] = 1;
  layerOf[3], layerOf[11] = 0;
  layerOf[4], layerOf[12] = 2;
  layerOf[5], layerOf[13] = 0;
  layerOf[6], layerOf[14] = 1;
  layerOf[7], layerOf[15] = 0;
}

void Click::initializeClick(){
  for(int i=0; i<16; i++){
    _deltaTauLatency[i] = 0;
  }
  _deltaTauSync = 0;
  _deltaTauRecovery = 0;
  resetClick();
}

void Click::addSyncObject(Sync *syn){
  _syn = syn;
}

void Click::addTempoObject(Tempo *tmp){
  _tmp = tmp;
}

void Click::setTau(int value){
  tau = value;
}

void Click::setDeltaTauSync(long deltaTauSync){
  _deltaTauSync = deltaTauSync;
}

void Click::setDeltaTauTempo(long deltaTauTempo){
  tau = tau + deltaTauTempo;
  _deltaTauLatency[(barPosition + 1) % 16] = _deltaTauLatency[(barPosition + 1) % 16] + (deltaTauTempo * 3);
  _deltaTauLatency[(barPosition + 2) % 16] = _deltaTauLatency[(barPosition + 2) % 16] + (deltaTauTempo * 3);
  _deltaTauLatency[(barPosition + 3) % 16] = _deltaTauLatency[(barPosition + 3) % 16] + (deltaTauTempo * 3);
}

void Click::setDeltaTauRecovery(long deltaTauRecovery){
  _deltaTauRecovery = deltaTauRecovery;
  Serial.print("recovery ");Serial.println(deltaTauRecovery);
}

void Click::updateClick(){
  long currentMillis = millis();

  if(currentMillis >= _nextClick){
    long nextStep = tau + _deltaTauLatency[barPosition];
    _deltaTauLatency[barPosition] = 0;
    _nextClick = currentMillis + nextStep;
    _nextBarPositionChange = currentMillis + (nextStep/2);
    _next16thChange = currentMillis + (nextStep/4);
    _midiTau = round((_nextBarPositionChange - currentMillis) / 6.0);
    //Serial.println("MIDI Clock First"); // xxx Send midi clock
    _midiClockSent = 1;
    _nextMidiClock = currentMillis + _midiTau;
    
    if(barPosition % 8 == 0){
      tone(14, 140, 50);
    }else if(barPosition % 2 == 0){
      tone(14, 140, 10);  
    }
  }
  
  if(currentMillis >= _nextBarPositionChange){
    _nextClick = _nextClick + _deltaTauSync + _deltaTauRecovery;
    _deltaTauSync = 0;
    _deltaTauRecovery = 0;
    _nextBarPositionChange = _nextClick + (tau/2);
    _next16thChange = currentMillis + ((_nextClick - currentMillis) / 2); 
    _midiTau = round((_nextClick - currentMillis) / 6.0);
    //Serial.println("MIDI Clock Half"); // xxx Send midi clock
    _midiClockSent = 1;
    _nextMidiClock = currentMillis + _midiTau;
    expectedClick = _nextClick;
    barPosition = (barPosition + 1) % 16;
  }
    
  if(currentMillis >= _next16thChange){
    _next16thChange = min(_nextBarPositionChange, _nextClick) + tau/2;
    barPosition16th = (barPosition16th + 1) % 16;
  }

  if(currentMillis >= _nextMidiClock && _midiClockSent < 5){
    //Serial.println("MIDI Clock"); // xxx Send midi clock
    _nextMidiClock = currentMillis + _midiTau;
    _midiClockSent++;
  }
}

void Click::resetClick(){
  Serial.println("reset");
  //send stop MidiClock msg xxx
  long hits[4] = {0,0,0,0};
  int hitsDetected = 0;
  long nextAllowedOnset = 0;
  while(hitsDetected < 4){
    long currentMillis = millis();
    if(analogRead(_piezoPin) > _threshold){
      if(currentMillis > nextAllowedOnset){
        Serial.println("onD");
        hits[hitsDetected] = currentMillis;
        hitsDetected++;
        nextAllowedOnset = currentMillis + _bounceTime;
      }
    }
  }
  
  // sets the tempo as the average of the 4 hits
  tau = int((hits[1]-hits[0]) + (hits[2]-hits[1]) + (hits[3]-hits[2])) / 6;
  //Serial.print("Bpm ");Serial.print(60000 / (2 * tau));Serial.print(" Tau ");Serial.println(tau);
  _nextClick = hits[3] + (3 * tau);
  _nextBarPositionChange = _nextClick - (tau / 2);
  _next16thChange = _nextBarPositionChange - (tau / 4);
  _midiTau = tau / 24;
  expectedClick = hits[3] + (2 * tau);
  _nextMidiClock = expectedClick + _midiTau;
  _syn->initializeSync();
  //_tmp->initializeTempo();
  barPosition = 0;
  barPosition16th = 0;
  delay(2 * tau);
  //send startMidiClock msg xxx
  //Serial.println("MIDI Clock first");
} 
