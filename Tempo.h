#ifndef TEMPO_H
#define TEMPO_H
#include <Arduino.h>
#include "Click.h"
#include "Onset.h"

class Tempo{
  public:
    Tempo(Onset* onsets, Click *clk);
    void initializeTempo();
    void updateTempo();
    void setSwingMode(bool is128);
    
  private:
    Click* _clk;                                  // pointer to click object
    Onset* _ons;                                  // pointer to onset object
    float* _lTempo;                               // pointer to lTempo array
    float _lTempo44[17];                          // likelihood of observing intervals of _v length for 4/4
    float _lTempo128[25];                         // likelihood of observing intervals of _v length for 4/4
    int _sigmaTempo;                              // width of the window
    float _thetaTempo;                            // threshold
    float _alpha;                                 // scaling factor
    bool _is128;
};

#endif
