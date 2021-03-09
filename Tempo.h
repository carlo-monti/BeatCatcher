#ifndef TEMPO
#define TEMPO
#include <Arduino.h>
#include "Onset.h"

class Tempo{
  public:
    Tempo(Onset *onsets, Click *clk);
    void initializeTempo();
    void setLTempo(int index, float value);
    void setAlpha(float value);
    void updateTempo();
    
  private:
    Click* _clk; // pointer to click object
    Onset* _ons; // pointer to onset object
    float _lTempo[17] = {0,0,1,0,1,0,0,0,0.92,0,0,0,0.68,0,0,0,0.8}; // likelihood of observing intervals of _v length
    int _sigmaTempo; // width of the window
    float _thetaTempo; // threshold
    float _alpha; // scaling factor

    // parameters for calculating the tempo tracking table
    float _interOnsetInterval; // actually a int but defined float for the need of division without truncating
    int _v;
    long _error;
    int _twoSigmaSquared;
    float _gaussian;
    float _currentAccuracy;
};

#endif
