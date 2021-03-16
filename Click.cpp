#include "Click.h"

Click::Click(int piezo){
  _piezoPin = piezo;
  _nextClick = 0;
  _bounceTime = 250;
  _threshold = 40;
  _deltaTauSync = 0;
  _deltaTauAlign = 0;
}

void Click::initializeClick(){
  tau = 500;
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
  _deltaTauAlign = deltaTauTempo * 2;
  //Serial.print("new tau "); Serial.print(tau);Serial.print(" bpm=");Serial.println(round(60000.0/(2*tau)));
}

void Click::updateClick(){
  _currentMillis = millis();
  
  if(_currentMillis >= _nextClick){
    //Serial.print("<---");Serial.println(barPosition);
    _nextClick = _currentMillis + tau + _deltaTauAlign;
    _deltaTauAlign = 0;
    //Serial.println("MIDI Clock first");
    _midiTau = (_nextHalfClick - _currentMillis) / 6;
    _nextMidiClock = _nextMidiClock + _midiTau;
    _midiClockSent = 0;
    if(barPosition == 0 || barPosition == 8){
      tone(22, 300, 50);
    }else{
      tone(14, 140, 10);  
    }
  }else if(_currentMillis >= _nextHalfClick){
    _nextClick = _nextClick + _deltaTauSync;
    _deltaTauSync = 0;
    //Serial.println("MIDI Clock half");
    _midiTau = (_nextClick - _currentMillis) / 6;
    _nextMidiClock = _nextMidiClock + _midiTau;
    _midiClockSent = 0;
    _nextHalfClick = _nextClick + (tau / 2);
    expectedClick = _nextClick;
    barPosition = (barPosition + 1) % 16;
  }else if(_currentMillis >= _nextMidiClock){
    if(_midiClockSent < 5){
      //send MIDI CLock
      //Serial.println("MIDI Clock");
      //Serial.println(_midiTau);
      _nextMidiClock = _currentMillis + _midiTau;
      _midiClockSent++;
    }
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
        //Serial.println(hitsDetected);
        hitsDetected++;
        nextAllowedOnset = _currentMillis + _bounceTime;
      }
    }
  }
  
  // sets the tempo as the average of the 4 hits
  tau = int((hits[1]-hits[0]) + (hits[2]-hits[1]) + (hits[3]-hits[2])) / 6;
  //Serial.print("Bpm ");Serial.print(60000 / (2 * tau));Serial.print(" Tau ");Serial.println(tau);
  // reset the click
  _nextClick = hits[3] + (3 * tau);
  _nextHalfClick = _nextClick - (tau / 2);
  _midiTau = tau / 24;
  _nextMidiClock = _nextClick + _midiTau;
  barPosition = 0;
  _deltaTauAlign = 0;
  delay(tau);
  // send startMidiClock msg
}
