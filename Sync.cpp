#include "Sync.h"

Sync::Sync(Onset* onsets, Click* clk){
  
  _ons = onsets;
  _clk = clk;
  _averageFactor = 0.1;
  
  _lSyncKick[0],_lSyncKick[8] = 1;            // 1
  _lSyncKick[1],_lSyncKick[9] = 0.1;          // and
  _lSyncKick[2],_lSyncKick[10] = 1;           // 2
  _lSyncKick[3],_lSyncKick[11] = 0.1;         // and
  _lSyncKick[4],_lSyncKick[12] = 1;           // 3  
  _lSyncKick[5],_lSyncKick[13] = 0.1;         // and
  _lSyncKick[6],_lSyncKick[14] = 1;           // 4
  _lSyncKick[7],_lSyncKick[15] = 0.1;         // and

  _lSyncSnare[0],_lSyncSnare[8] = 1;          // 1
  _lSyncSnare[1],_lSyncSnare[9] = 0.1;        // and
  _lSyncSnare[2],_lSyncSnare[10] = 1;         // 2
  _lSyncSnare[3],_lSyncSnare[11] = 0.1;       // and
  _lSyncSnare[4],_lSyncSnare[12] = 1;         // 3
  _lSyncSnare[5],_lSyncSnare[13] = 0.1;       // and
  _lSyncSnare[6],_lSyncSnare[14] = 1;         // 4
  _lSyncSnare[7],_lSyncSnare[15] = 0.1;       // and
}

void Sync::initializeSync(){
  _thetaSync = 0.80;
  _beta = 0.6;
  lastMillisForPrint = 0; // xxx
  _sigmaSync = _clk->tau / 20;
  for(int i=0; i<16; i++){
    _accuracyOfLastSync[i] = 0;
  }
  _lastAccuracyOfCurrentLayer = 0;
  _lastBarPosition16th = 0;
  _hasKick = 0;
  _hasSnare = 0;
  _lastRecovery = 0;
  for(int i=0; i<16; i++){
    _averageKickGroove[i] = 0;
    _averageSnareGroove[i] = 0;
    _storedKickGroove[i] = 0;
    _storedSnareGroove[i] = 0;
    _currentKickGroove[i] = 0;
    _currentSnareGroove[i] = 0;
  }
}

void Sync::setLSync(bool type, int index, float value){
  if(type){
    _lSyncSnare[index] = value;
  }else{
    _lSyncKick[index] = value;
  }
}

void Sync::updateSync(){
  int thisLayer = _clk->layerOf[_clk->barPosition]; // layer of this onset
  long deltaTauSync = 0;
  long error = (_ons->onsets[_ons->currentOnset] - _clk->expectedClick); // current onset - expected onset (tn - E[tn])
  float accuracy;
  float gaussian;
  float lSync;
  
  if(_ons->isSnare[_ons->currentOnset]){
    /* per stampa */ onsetKickPerStampa = 0;onsetSnarePerStampa = 1; /* per stampa*/
    lSync = _lSyncSnare[_clk->barPosition];
    _hasSnare = true;
  }else{
    /* per stampa */ onsetKickPerStampa = 1;onsetSnarePerStampa = 0; /* per stampa*/
    lSync = _lSyncKick[_clk->barPosition];
    _hasKick = true;
  }

  int thirdOfTau = (_clk->tau) / 3;
  
  if(_sigmaSync < thirdOfTau && abs(error) > thirdOfTau){ // notch for 16th
    _ons->is16th[_ons->currentOnset] = false;
    //Serial.println("On notch");
    return;
  }
  
  int twoSigmaSquared = -2 * pow(_sigmaSync,2); // calculate -2 * sigma^2
  gaussian = exp(pow(error,2) / twoSigmaSquared); // calculate gaussian
  accuracy = gaussian * lSync;
  if(accuracy > _thetaSync){
    deltaTauSync = ((gaussian + _beta) / (_beta + 1)) * accuracy * error;
    if(thisLayer < _currentHigherLayer){
      if(accuracy > _lastAccuracyOfCurrentLayer){
        _accuracyOfLastSync[_clk->barPosition] = accuracy;
        _clk->setDeltaTauSync(deltaTauSync);
      }
    }else{
      if(accuracy > _accuracyOfLastSync[_clk->barPosition]){ // sync
        _accuracyOfLastSync[_clk->barPosition] = accuracy;
        _clk->setDeltaTauSync(deltaTauSync);
      }
      if(accuracy >= _thetaSync + 0.1 && thisLayer >= _currentHigherLayer){ // change parameters
        // higher threshold, narrow window
        _thetaSync = _thetaSync + (0.3 * (accuracy - _thetaSync - 0.1));
        _sigmaSync = max((_clk->tau) / 20,_sigmaSync * 
                  (_narrowWindowRatio + ((0.7 * lSync) - accuracy))); // calculate sigma from current tau
      }
    }
  }else{
    if(thisLayer >= _currentHigherLayer){ // no sync but change parameters
      // lower threshold, expand window
      _thetaSync = 0.6 * _thetaSync;
      _sigmaSync = max((_clk->tau) / 20,_sigmaSync * 
                  (_expandWindowRatio + ((0.7 * lSync) - accuracy))); // calculate sigma from current tau
    }
  }
}

void Sync::updateLayerAndGroove(){
  long currentMillis = millis();
  if(_lastBarPosition != _clk->barPosition){                // whenever bar position changes do:
    int pastLayer = _clk->layerOf[_lastBarPosition];        // layer of last bar position
    if(_accuracyOfLastSync[_lastBarPosition] == 0){         // if last bar position has a miss decrease current higher layer
      if(pastLayer == _currentHigherLayer){
        _currentHigherLayer = max(0,_currentHigherLayer - 1);
      }
    }else{ // if onset detected and synced
      if(pastLayer >= _currentHigherLayer){
        _lastAccuracyOfCurrentLayer = _accuracyOfLastSync[_lastBarPosition]; // sets the current accuracy as global accuracy for the layer
        _currentHigherLayer = pastLayer;
      }
    }
    _accuracyOfLastSync[_lastBarPosition] = 0; // reset the accuracy value of the sync onset for last bar position
    _lastBarPosition = _clk->barPosition;
  }
  
  if(_lastBarPosition16th != _clk->barPosition16th){    
    _currentKickGroove[_lastBarPosition16th] = _hasKick;
    _currentSnareGroove[_lastBarPosition16th] = _hasSnare;
    if(_hasKick){
      _averageKickGroove[_lastBarPosition16th] = min(1,_averageKickGroove[_lastBarPosition16th] + _averageFactor);
    }else{
      _averageKickGroove[_lastBarPosition16th] = max(0,_averageKickGroove[_lastBarPosition16th] - _averageFactor);
    }
    for(int i=0; i<16; i++){
      _storedKickGroove[i] = _averageKickGroove[i] > 0.5;
    }
    if(_hasSnare){
      _averageSnareGroove[_lastBarPosition16th] = min(1,_averageSnareGroove[_lastBarPosition16th] + _averageFactor);
    }else{
      _averageSnareGroove[_lastBarPosition16th] = max(0,_averageSnareGroove[_lastBarPosition16th] - _averageFactor);
    }
    for(int i=0; i<16; i++){
      _storedSnareGroove[i] = _averageSnareGroove[i] > 0.5;
    }
    _hasKick = false;
    _hasSnare = false;
    _lastBarPosition16th = _clk->barPosition16th;

    int checkIfAfterKick = 0;
    int checkIfAfterSnare = 0;
    int checkIfBeforeKick = 0;
    int checkIfBeforeSnare = 0;
    int countZerosKick = 0;
    int countZerosSnare = 0;
    if((currentMillis - _lastRecovery) > (_clk->tau) * 64){
      for (int i = 0; i < 16; i++){
        if (_storedKickGroove[(i + 1) % 16] == _currentKickGroove[i]){ 
          checkIfAfterKick++;
        }
        if (_storedKickGroove[(i + 15) % 16] == _currentKickGroove[i]){ 
          checkIfBeforeKick++;
        }
        countZerosKick = countZerosKick + _currentKickGroove[i];
      }
      for (int i = 0; i < 16; i++){
        if (_storedSnareGroove[(i + 1) % 16] == _currentSnareGroove[i]){ 
          checkIfAfterSnare++;
        }
        if (_storedSnareGroove[(i + 15) % 16] == _currentSnareGroove[i]){ 
          checkIfBeforeSnare++;
        }
        countZerosSnare = countZerosSnare + _currentKickGroove[i];
      }
      if(countZerosKick == 0 || countZerosSnare == 0){
      }else{
        if(checkIfAfterKick > _allowedDifference && checkIfAfterSnare > _allowedDifference){
          _clk->setDeltaTauRecovery(-(_clk->tau)/2);
          _lastRecovery = currentMillis;
        }else if(checkIfBeforeKick > _allowedDifference && checkIfBeforeSnare > _allowedDifference){
          _clk->setDeltaTauRecovery((_clk->tau)/2);
          _lastRecovery = currentMillis;
        }
      }
    }
  }
  /* solo per stampa -------------- 
  long curr = millis();
  if(curr > lastMillisForPrint){
    int thisLayer = _clk->layerOf[_clk->barPosition]; // layer of this onset
    long deltaTauSync = 0;
    long error = (curr - *_expectedClick); // current onset - expected onset (tn - E[tn])
    float accuracy;
    float gaussian;
    if(error >= ((_clk->tau) / 3)){ // notch for 16th
      gaussian = 0;
    }else{
      int twoSigmaSquared = -2 * pow(_sigmaSync,2); // calculate -2 * sigma^2
      gaussian = exp(pow(error,2) / twoSigmaSquared); // calculate gaussian
    }
    Serial.print(curr);Serial.print(",");Serial.print(onsetKickPerStampa);Serial.print(",");Serial.print(onsetSnarePerStampa);Serial.print(",");Serial.print(gaussian);Serial.print(",");Serial.println(_thetaSync);
    lastMillisForPrint = curr;
    onsetKickPerStampa = false;
    onsetSnarePerStampa = false;
  }
  /* per stampa --------------------*/
}
