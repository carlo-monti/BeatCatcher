#include "Sync.h"

Sync::Sync(Onset &onsets, Click &clk){
  _ons = &onsets;
  _clk = &clk;
}

void Sync::initializeSync(){
  _thetaSync = 0.80;
  _beta = 0;
  _sigmaSync = _clk->tau / 5;
  for(int i=0; i<16; i++){
    _accuracyOfLastSync[i] = 0;
  }
  _lastAccuracyOfCurrentLayer = 0;
}

void Sync::setLSync(bool type, int index, float value){
  if(type){
    _lSyncSnare[index] = value;
  }else{
    _lSyncKick[index] = value;
  }
}

void Sync::setBeta(float value){
  _beta = value;
}

void Sync::updateSync(){
  int thisLayer = _clk->layerOf[_clk->barPosition]; // layer of this onset
  long deltaTauSync = 0;
  long error = (_ons->onsets[_ons->currentOnset] - _clk->expectedClick); // current onset - expected onset (tn - E[tn])
  float accuracy;
  float gaussian;
  float lSync;
  if(_ons->isSnare[_ons->currentOnset]){
    Serial.println("SYNC Snare -----------");
    lSync = _lSyncSnare[_clk->barPosition];
  }else{
    Serial.println("SYNC Kick -----------");
    lSync = _lSyncKick[_clk->barPosition];
  }
  if(error > (_clk->tau / 3)){ // notch for 16th
    _ons->is16th[_ons->currentOnset] = 1;
    Serial.println("On notch");
    return;
  }
  int twoSigmaSquared = -2 * pow(_sigmaSync,2); // calculate -2 * sigma^2
  gaussian = exp(pow(error,2) / twoSigmaSquared); // calculate gaussian
  accuracy = gaussian * lSync;
  //Serial.print("SYNC -> Sigma: ");Serial.print(_sigmaSync);Serial.print(" Threshold: ");Serial.print(_thetaSync);Serial.print(" Error: ");Serial.print(error);Serial.print(" Gaussian: ");Serial.print(gaussian);Serial.print(" lSync: ");Serial.print(_lSync[_clk->barPosition]);Serial.print(" Accuracy: ");Serial.println(accuracy);

  if(accuracy > _thetaSync){
    deltaTauSync = ((gaussian + _beta) / (_beta + 1)) * accuracy * error;
    if(thisLayer < _currentHigherLayer){
      if(accuracy > _lastAccuracyOfCurrentLayer){
        _accuracyOfLastSync[_clk->barPosition] = accuracy;
        _clk->updateTauSync(deltaTauSync);
        Serial.print("syncs: "); Serial.println(deltaTauSync);
      }
    }else{
      if(accuracy > _accuracyOfLastSync[_clk->barPosition]){ // sync
        _accuracyOfLastSync[_clk->barPosition] = accuracy;
        _clk->updateTauSync(deltaTauSync);
        Serial.print("syncs: "); Serial.print(deltaTauSync);Serial.print(" Error: ");Serial.print(error);
      }
      if(accuracy >= _thetaSync + 0.1 && thisLayer >= _currentHigherLayer){ // change parameters
        // higher threshold, narrow window
        _thetaSync = _thetaSync + (0.3 * (accuracy - _thetaSync - 0.1));
        _sigmaSync = max(_clk->tau / 30,_sigmaSync * (_narrowWindowRatio + ((0.7 * lSync) - accuracy))); // calculate sigma from current tau
        Serial.print(" new threshold: "); Serial.print(_thetaSync); Serial.print(" new sigmaSync: ");Serial.print(_sigmaSync);
      }
      Serial.println();
    }
  }else{
    if(thisLayer >= _currentHigherLayer){ // no sync but change parameters
      // lower threshold, expand window
      _thetaSync = 0.6 * _thetaSync;
      _sigmaSync = max(_clk->tau / 30,_sigmaSync * (_expandWindowRatio + ((0.7 * lSync) - accuracy))); // calculate sigma from current tau
      Serial.print(" new threshold: "); Serial.print(_thetaSync);Serial.print(" sigmaSync: ");Serial.println(_sigmaSync);
    }else{
      Serial.print(" no layer change ");Serial.println(_currentHigherLayer);
    }
  }
}

void Sync::updateLayer(){
  if(_lastBarPosition != _clk->barPosition){ // whenever bar position changes do:
    int pastLayer = _clk->layerOf[_lastBarPosition]; // layer of last bar position
    if(_accuracyOfLastSync[_lastBarPosition] == 0){ // if no onset detected in last bar position decrease current higher layer
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
    //Serial.print("------ NEW bar position: ");Serial.print(_clk->barPosition);Serial.print(" layer: ");Serial.println(_currentHigherLayer);
  }
}
