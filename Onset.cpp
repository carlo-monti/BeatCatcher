#include "Onset.h"

Onset::Onset(Click *clk, int piezo0, int piezo1){
  _clk = clk;
  _piezo0 = piezo0;
  _piezo1 = piezo1;
  _bounceTime0 = 100;
  _bounceTime1 = 100;
  _threshold0 = 40;
  _threshold1 = 40;
}

void Onset::setBounceTime(bool piezo, int bounce){
  if(piezo){
    _bounceTime1 = bounce; 
  }else{
    _bounceTime0 = bounce;       
  }
}

void Onset::setThreshold(bool piezo, int thr){
  if(piezo){
    _threshold1 = thr; 
  }else{
    _threshold0 = thr;       
  }
}

void Onset::initializeOnset(){
  for(int i=0;i<onsetsLength;i++){
    onsets[i]=0;
    is16th[i]=0;
  }
  _nextAllowedOnsetKick = 0;
  _nextAllowedOnsetSnare = 0;
  last = 0;
  next = 0;
}

bool Onset::updateOnset(){
  
  _currentMillis = millis();
  // update the index of last relevant onset
  while(onsets[last] < (_currentMillis - (_clk->tau * 8))){
    is16th[last] = 0;
    if(last == next){
      break;
    }
    last = (last + 1) % onsetsLength;
  }
  if(analogRead(_piezo0) > _threshold0){ // checks for new onset for kick
    // set the new onset
    if(_currentMillis > _nextAllowedOnsetKick){
      onsets[next] = _currentMillis;
      isSnare[next] = 0;
      currentOnset = next;
      next = (next + 1) % onsetsLength;
      _nextAllowedOnsetKick = _currentMillis + _bounceTime0;
      return true;
    }
  }else if(analogRead(_piezo1) > _threshold1){ // checks for new onset for snare
    // set the new onset
    if(_currentMillis > _nextAllowedOnsetSnare){
      onsets[next] = _currentMillis;
      isSnare[next] = 1;
      currentOnset = next;
      next = (next + 1) % onsetsLength;
      _nextAllowedOnsetSnare = _currentMillis + _bounceTime1;
      return true;
    }
  }
  return false;
}
