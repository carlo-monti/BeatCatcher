#include "Click.h"

Click::Click(int piezo){
  _piezoPin = piezo;
  _nextClick = 0;
  _bounceTime = 150;
  _threshold = 15;
}

void Click::initializeClick(){
  tau = 2000;
  resetClick();
}

void Click::setTau(int value){
  tau = value;
}

void Click::updateTauSync(long deltaTauSync){
  _deltaTauSync = deltaTauSync;
}

void Click::updateTauTempo(long deltaTauTempo){
  tau = tau + deltaTauTempo;
  Serial.print("new tau "); Serial.print(tau);Serial.print(" bpm=");Serial.print(round(60000.0/(2*tau)));Serial.println("-------------------");
}

void Click::updateClick(){
  
  _currentMillis = millis();
  if(_currentMillis > _nextClick){
    _nextClick = _currentMillis + tau;
    //Serial.print("Click ");Serial.println(barPosition);
    if(barPosition == 0 || barPosition == 8){
      tone(22, 300, 50);
    }else{
      tone(14, 140, 10);  
    }
  }
  
  if(_currentMillis > _nextHalfClick){
    if(_deltaTauSync != 0){
      _nextClick = _nextClick + _deltaTauSync;
      _deltaTauSync = 0;
    }
    _midiTau = (_nextClick - _currentMillis) / 6;
    _nextHalfClick = _nextClick + (tau / 2);
    expectedClick = _nextClick;
    barPosition = (barPosition + 1) % 16;
  }

  if(_currentMillis > _nextMidiClick){
    //SendMidiClock
    //Serial.println("MIDI Clock");
    //Serial.println(_midiTau);
    _nextMidiClick = _nextMidiClick + _midiTau;
  }
}

void Click::resetClick(){
  //send stop MidiClock msg
  long hits[4] = {0,0,0,0};
  int hitsDetected = 0;
  long nextAllowedOnset = 0;
  // detect 4 hits 
  while(hitsDetected < 4){
    _currentMillis = millis();
    if(analogRead(_piezoPin) > _threshold){
      if(_currentMillis > nextAllowedOnset){
        hits[hitsDetected] = _currentMillis;
        hitsDetected++;
        nextAllowedOnset = _currentMillis + _bounceTime;
      }
    }
  }
  
  // sets the tempo as the average of the 4 hits
  tau = int((hits[1]-hits[0]) + (hits[2]-hits[1]) + (hits[3]-hits[2])) / 6;
  Serial.print("Bpm ");
  Serial.println(60000 / (2 * tau));
  Serial.print("Tau");
  Serial.println(tau);
  // reset the click
  _nextClick = hits[3] + (2 * tau);
  _nextHalfClick = hits[3] + (3 * tau);
  _midiTau = tau / 24;
  _nextMidiClick = _nextClick + _midiTau;
  barPosition = 0;
  // send startMidiClock msg
}
