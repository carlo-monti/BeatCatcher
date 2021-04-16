#include "Tempo.h"

Tempo::Tempo(Onset *onsets, Click *clk){
  _ons = onsets;
  _clk = clk;
  
  _lTempo[0] = 0;       //
  _lTempo[1] = 0;       // 1/8
  _lTempo[2] = 1;       // 1/4  
  _lTempo[3] = 0;       //
  _lTempo[4] = 1;       // 2/4
  _lTempo[5] = 0;       //
  _lTempo[6] = 0;       //
  _lTempo[7] = 0;       //
  _lTempo[8] = 0.92;    // 4/4
  _lTempo[9] = 0;       //
  _lTempo[10] = 0;      //
  _lTempo[11] = 0;      //
  _lTempo[12] = 0.68;   // 6/4
  _lTempo[13] = 0;      //
  _lTempo[14] = 0;      //
  _lTempo[15] = 0;      //
  _lTempo[16] = 0.8;    // 8/4
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
  long currentOnset = _ons->onsets[_ons->currentOnset];             // load the last onset
  float accuracyWin = 0;                                            // accuracy of the winner interval
  long errorWin = 0;                                                // error of the winner interval
  int vWin = 0;                                                     // v of the winner interval
  long deltaTauTempo = 0;
  float gaussian = 0;
  float interOnsetInterval;                                         // actually a int but defined float for the need of division without truncating
  int v;
  long error;
  float currentAccuracy;
  int i=_ons->last;
  
  while(i != _ons->currentOnset){                                     // for every onset...
    if(!_ons->is16th[i]){                                             // that is not a 16th...
      interOnsetInterval = (currentOnset - _ons->onsets[i]);          // calculate IOI for current onset and previous onset (tn - tn-k)
      v = round(interOnsetInterval / _clk->tau);                      // IOI in tatum intervals v(k)
      error = interOnsetInterval - (_clk->tau * v);                   // error = IOI - v(k) * tau
      gaussian = exp(pow(error,2) / float(-2 * pow(_sigmaTempo,2)));  // calculate gaussian
      currentAccuracy = gaussian * _lTempo[v];                        // g(en,k)*Ltempo(v(k))
      if(currentAccuracy > accuracyWin){                              // check if it is the winner
        accuracyWin = currentAccuracy;
        errorWin = error;
        vWin = v;
      }  
    }
    i = (i+1) % _ons->onsetsLength;
  }
  deltaTauTempo = _alpha * accuracyWin * (errorWin / vWin);
  
  if(accuracyWin >= _thetaTempo){                                   // Update tempo + change parameters (raise threshold)
    if(deltaTauTempo != 0){
      _clk->setDeltaTauTempo(deltaTauTempo);
    }
    if(accuracyWin >= _thetaTempo + 0.1){
      _thetaTempo = _thetaTempo + (0.3 * (accuracyWin - _thetaTempo - 0.1));
    }
  }else{                                                            // just change parameters (lower threshold)
    _thetaTempo = 0.6 * _thetaTempo;
  }
  _sigmaTempo = max(_clk->tau / 20,_sigmaTempo * (1 + ((0.7 * _lTempo[vWin]) - accuracyWin))); // calculate sigma from current tau
}
