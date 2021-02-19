#include "Onset.h"

Onset::Onset(int &t, int piezo, int bounce, int thr){
  _tau = &t;
  _piezoPin = piezo;
  _bounceTime = bounce;
  _threshold = thr;
}

void Onset::setBounceTime(int bounce){
  _bounceTime = bounce; 
}

void Onset::setThreshold(int thr){
  _threshold = thr;
}

void Onset::initializeOnset(){
  for(int i=0;i<onsetsLength;i++){
    onsets[i]=0;
    syncDone[i]=false;
  }
  _nextAllowedOnset = 0;
  last = 0;
  next = 0;
}

bool Onset::updateOnset(){
  _currentMillis = millis();
  // update the index of last relevant onset
  while(onsets[last] < (_currentMillis - (*_tau * 16))){
    syncDone[last] = false;
    if(last == next){
      break;
    }
    last = (last + 1) % onsetsLength;
  }

  // checks for new onset
  if(analogRead(_piezoPin) > _threshold){
    long currentTime = _currentMillis;
    // set the new onset
    if(currentTime > _nextAllowedOnset){
      onsets[next] = currentTime;
      Serial.println("<-onset");
      next = (next + 1) % onsetsLength;
      _nextAllowedOnset = currentTime + _bounceTime;
      return true;
    }
  }
  return false;
}
