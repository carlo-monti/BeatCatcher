#include "Sync.h"

Sync::Sync(Onset &onsets, Click &clk){
  _ons = &onsets;
  _clk = &clk;
}

void Sync::initializeSync(){
  _thetaSync = 0.80;
  _beta = 1;
  _sigmaSync = _clk->tau / 5;
}

void Sync::setLSync(int index, float value){
  _lSync[index] = value;
}

void Sync::setBeta(float value){
  _beta = value;
}

void Sync::updateSync(){
  
  long deltaTauSync = 0;
  long error = (_ons->onsets[_ons->next - 1] - _clk->expectedClick); // current onset - expected onset (tn - E[tn])
  int twoSigmaSquared = -2 * pow(_sigmaSync,2); // calculate -2 * sigma^2
  float gaussian = exp(pow(error,2) / twoSigmaSquared); // calculate gaussian
  float accuracy = gaussian * _lSync[_clk->barPosition]; // calculate accuracy
  //Serial.print("Sigma: ");Serial.print(_sigma);Serial.print(" Threshold: ");Serial.print(_thetaSync);Serial.print(" Error: ");Serial.print(error);Serial.print(" Gaussian: ");Serial.print(gaussian);Serial.print(" lSync: ");Serial.print(_lSync[_clk->barPosition]);Serial.print(" Accuracy: ");Serial.println(accuracy);
  
  if(accuracy > _thetaSync){ // just sync
    deltaTauSync = ((gaussian + _beta) / (_beta + 1)) * gaussian * _lSync[_clk->barPosition] * error;
    if(deltaTauSync != 0){
      _clk->updateTauSync(deltaTauSync);
      Serial.print("SYNC just sinc.");
    }
    if(accuracy >= _thetaSync + 0.1){ // sync + change parameters
      _thetaSync = _thetaSync + (0.3 * (accuracy - _thetaSync - 0.1));
      Serial.print("SYNC syncs "); Serial.println(deltaTauSync);
      Serial.print("SYNC new threshold "); Serial.println(_thetaSync);
      _sigmaSync = max(_clk->tau / 20,_sigmaSync * (1 + ((0.7 * _lSync[_clk->barPosition]) - accuracy))); // calculate sigma from current tau
      Serial.print("SYNC new sigmaSync ");Serial.println(_sigmaSync);
    }
  }else{ // no sync
    _thetaSync = 0.6 * _thetaSync;
    Serial.print("SYNC NO");
    Serial.print("SYNC new threshold "); Serial.println(_thetaSync);
    _sigmaSync = max(_clk->tau / 20,_sigmaSync * (1 + ((0.7 * _lSync[_clk->barPosition]) - accuracy))); // calculate sigma from current tau
    Serial.print("SYNC sigmaSync ");Serial.println(_sigmaSync);
  }
  
}
