#ifndef TEMPO_H
#define TEMPO_H
#include <Arduino.h>
#include "Click.h"
#include "Onset.h"

class Tempo{
  public:
    Tempo(Onset* onsets, Click *clk);
    void initializeTempo();
    void setLTempo(int index, float value);
    void setAlpha(float value);
    void updateTempo();
    
  private:
    Click* _clk;                                  // pointer to click object
    Onset* _ons;                                  // pointer to onset object
    float _lTempo[17];                            // likelihood of observing intervals of _v length
    int _sigmaTempo;                              // width of the window
    float _thetaTempo;                            // threshold
    float _alpha;                                 // scaling factor
};

#endif
