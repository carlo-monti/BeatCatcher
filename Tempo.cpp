#include "Tempo.h"

Tempo::Tempo(Onset *onsets, Click *clk){
  _ons = onsets;
  _clk = clk;
  _lTempo = _lTempo44;
  _lTempo44[0] = 0;       //
  _lTempo44[1] = 0;       // 1/8
  _lTempo44[2] = 1;       // 1/4  
  _lTempo44[3] = 0;       //
  _lTempo44[4] = 1;       // 2/4
  _lTempo44[5] = 0;       //
  _lTempo44[6] = 0;       // 3/4
  _lTempo44[7] = 0;       //
  _lTempo44[8] = 0.92 ;    // 4/4
  _lTempo44[9] = 0;       //
  _lTempo44[10] = 0;      //
  _lTempo44[11] = 0;      //
  _lTempo44[12] = 0.8;   // 6/4
  _lTempo44[13] = 0;      //
  _lTempo44[14] = 0;      //
  _lTempo44[15] = 0;      //
  _lTempo44[16] = 0.8;    // 8/4

  _lTempo128[0] = 0;       //
  _lTempo128[1] = 0;       //
  _lTempo128[2] = 0;       // 
  _lTempo128[3] = 1;       // 3/8
  _lTempo128[4] = 0;       //
  _lTempo128[5] = 0;       //
  _lTempo128[6] = 1;       // 6/8
  _lTempo128[7] = 0;       //
  _lTempo128[8] = 0;       //
  _lTempo128[9] = 0;       //
  _lTempo128[10] = 0;      //
  _lTempo128[11] = 0;      //
  _lTempo128[12] = 1;      // 12/8
  _lTempo128[13] = 0;      //
  _lTempo128[14] = 0;      //
  _lTempo128[15] = 0;      //
  _lTempo128[16] = 0;      //
  _lTempo128[17] = 0;      //
  _lTempo128[18] = 0.8;    // 18/8
  _lTempo128[19] = 0;      //
  _lTempo128[20] = 0;      //
  _lTempo128[21] = 0;      //
  _lTempo128[22] = 0;      //
  _lTempo128[23] = 0;      //
  _lTempo128[24] = 0.8;    // 24/8
}

void Tempo::initializeTempo(){
  _thetaTempo = 0.80;
  _alpha = 1;
  _sigmaTempo = _clk->tau / 20;
}

void Tempo::setSwingMode(bool is128){
  if(is128){
    _lTempo = _lTempo128;
  }else{
    _lTempo = _lTempo44;
  }
  _is128 = is128;
}

void Tempo::updateTempo(){
  long currentOnset = _ons->getCurrentOnset();                      // load the last onset
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
  
  while(i != _ons->currentOnsetIndex){                                // for every onset...
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
