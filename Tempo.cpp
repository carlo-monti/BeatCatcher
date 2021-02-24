#include "Tempo.h"

Tempo::Tempo(Onset &onsets, Click &clk){
  _ons = &onsets;
  _clk = &clk;
}

void Tempo::initializeTempo(){
  _thetaTempo = 0.80;
  _alpha = 1;
  _sigmaTempo = _clk->tau / 8;
}

void Tempo::setLTempo(int index, float value){
  _lTempo[index] = value;
}

void Tempo::setAlpha(float value){
  _alpha = value;
}

void Tempo::updateTempo(){
  long currentOnset = _ons->onsets[_ons->next - 1]; // load the last onset
  float accuracyWin = 0; // accuracy of the winner interval
  long errorWin = 0; // error of the winner interval
  int vWin = 0; // v of the winner interval
  long deltaTauTempo = 0;
 // Serial.println("--------------------");
  // read every onset from last to next-2
  int i=_ons->last;
  while(i != _ons->next - 1){ 
    _interOnsetInterval = (currentOnset - _ons->onsets[i]); // calculate IOI for current onset and previous onset (tn - tn-k)
    _v = round(_interOnsetInterval / _clk->tau); // IOI in tatum intervals v(k)
    _error = _interOnsetInterval - (_clk->tau * _v); // error = IOI - v(k) * tau
    _twoSigmaSquared = -2 * pow(_sigmaTempo,2); // calculate -2 * sigma^2
    _gaussian = exp(pow(_error,2) / _twoSigmaSquared); // calculate _gaussian
    _currentAccuracy = _gaussian * _lTempo[_v]; // g(en,k)*Ltempo(v(k))
    //Serial.print("_error ");Serial.println(_error);
    //Serial.print("_v ");Serial.println(_v);
    if(_currentAccuracy > accuracyWin){
      accuracyWin = _currentAccuracy;
      errorWin = _error;
      vWin = _v;
    }  
    i = (i+1) % _ons->onsetsLength;
  }
//  Serial.print("accuracyWin ");Serial.println(accuracyWin);
//  Serial.print("errorWin ");Serial.println(errorWin);
//  Serial.print("L[vWin] ");Serial.println(_lTempo[vWin]);
//  Serial.print("vWin ");Serial.println(vWin);
  deltaTauTempo = _alpha * accuracyWin * (errorWin / vWin);
  
  if(accuracyWin > _thetaTempo){ // Update tempo + change parameters (raise threshold)
    if(deltaTauTempo != 0){
      _clk->updateTauTempo(deltaTauTempo);
      //Serial.print("TEMPO updating "); Serial.println(deltaTauTempo);
    }
    if(accuracyWin >= _thetaTempo + 0.1){
      _thetaTempo = _thetaTempo + (0.3 * (accuracyWin - _thetaTempo - 0.1));
      //Serial.print("TEMPO update + new threshold "); Serial.println(_thetaTempo);
    }
  }else{ // just change parameters (lower threshold)
    //Serial.println("TEMPO just change parameters "); 
    _thetaTempo = 0.6 * _thetaTempo;
  }
  _sigmaTempo = max(_clk->tau / 20,_sigmaTempo * (1 + ((0.7 * _lTempo[vWin]) - accuracyWin))); // calculate sigma from current tau
  //Serial.print("TEMPO sigmaTempo ");Serial.println(_sigmaTempo);
}
