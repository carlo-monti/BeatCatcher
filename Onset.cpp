#include "Onset.h"

Onset::Onset(Click *clk, int piezo0, int piezo1){
  _clk = clk;
  _piezo0 = piezo0;
  _piezo1 = piezo1;
  _bounceTime0 = 50;
  _bounceTime1 = 50;
  _threshold0 = 40;
  _threshold1 = 40;
  _dischargeValue0 = 6;
  _dischargeValue1 = 6;
  _deltaAmount0 = 70;
  _deltaAmount1 = 50;
  setSwingMode(false);
}

long Onset::getCurrentOnset(){
  return onsets[currentOnsetIndex];
}

void Onset::setCurrentOnsetAs16th(bool itIsA16th){
  is16th[currentOnsetIndex] = itIsA16th;
}

void Onset::setSwingMode(bool is128){
  if(is128){
    _twoBarLengthInTatum = 16;
  }else{
    _twoBarLengthInTatum = 24;
  }
  _is128 = is128;
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
  
  long currentMillis = millis();
  
  // update the index of last relevant onset
  while(onsets[last] < (currentMillis - (_clk->tau * _twoBarLengthInTatum))){
    is16th[last] = 0;
    if(last == next){
      break;
    }
    last = (last + 1) % onsetsLength;
  }

  // envelope follower
  int readValue0 = analogRead(_piezo0);
  if(readValue0 > _currentValue0){
    _currentValue0 = readValue0;
  }
  int readValue1 = analogRead(_piezo1);
  if(readValue1 > _currentValue1){
    _currentValue1 = readValue1;
  }

  // store value
  if(currentMillis > _lastUpdate){
    _currentValue0 = max(0,_currentValue0 -= _dischargeValue0);
    _currentValue1 = max(0,_currentValue1 -= _dischargeValue1); 
    _storedValue0 = _currentValue0;
    _storedValue1 = _currentValue1;
    _lastUpdate = currentMillis;
    //Serial.print(1023);Serial.print(",");Serial.print(_currentValue0);Serial.print(",");Serial.println(_currentValue1);
  }
  // check for delta
  if(_currentValue0 > _storedValue0 + _deltaAmount0){
    if(currentMillis > _nextAllowedOnsetKick){
      onsets[next] = currentMillis;
      isSnare[next] = 0;
      currentOnsetIndex = next;
      next = (next + 1) % onsetsLength;
      _nextAllowedOnsetKick = currentMillis + _bounceTime0;
      Serial.println("Kick");
      //tone(21, 140, 50);   
      return true;
    }
  }
  if(_currentValue1 > _storedValue1 + _deltaAmount1){
    if(currentMillis > _nextAllowedOnsetSnare){
      onsets[next] = currentMillis;
      isSnare[next] = 1;
      currentOnsetIndex = next;
      next = (next + 1) % onsetsLength;
      _nextAllowedOnsetSnare = currentMillis + _bounceTime1;
      Serial.println("Snare");
      //tone(22, 140, 50);  
      return true;
    }
  }
  return false;
}
