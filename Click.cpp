#include "Click.h"

Click::Click(int piezo, int key){
  _piezoPin = piezo;
  _nextClick = 0;
  _bounceTime = 150;
  _threshold = 20;
  _aKey = key;
}

void Click::setTau(int value){
  tau = value;
}

void Click::updateTauSync(long deltaTauSync){
  // wait for the halfclick and then updates the nextClick and nextHalfClick
  _deltaTauSync = deltaTauSync;
}

void Click::updateTauTempo(long deltaTauTempo){
  tau = tau + deltaTauTempo;
}

void Click::startClick(){
}

void Click::stopClick(){}

void Click::updateClick(){
  if(digitalRead(_aKey)==0){
    Serial.println("Reset");
    delay(300);
    _resetClick();
  }
  
  _currentMillis = millis();
  if(_currentMillis > _nextClick){
    _nextClick = _currentMillis + tau;
    Serial.print("Click ");
    Serial.println(_barPosition);
    tone(14, 440, 50);
  }
  
  if(_currentMillis > _nextHalfClick){
    // updates nextClick with the deltaTau for the synchronizing
    if(_deltaTauSync > 0){
      _nextClick = _nextClick + _deltaTauSync;
      _deltaTauSync = 0;
    }
    // updates nextHalfClick
    _nextHalfClick = _nextClick + (tau / 2);
    Serial.println(_barPosition);
    Serial.println("-----");
    _barPosition = (_barPosition + 1) % 16;
    Serial.println(_barPosition);
  }
}

void Click::_resetClick(){
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
  Serial.println(60000 / tau);
  // reset the click
  _nextClick = hits[3] + (2 * tau);
  _nextHalfClick = hits[3] + (3 * tau);
  _barPosition = 0;
}
