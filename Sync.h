#ifndef SYNC
#define SYNC
#include <Arduino.h>
#include "Onset.h"

class Sync{
  public:
    Sync(Onset &onsets, Click &clk);
    void initializeSync();
    void setLSync(int index, float value);
    void setBeta(float value);
    void setThreshold(float value);
    void updateSync();
    
  private:
    Click* _clk; // pointer to click object
    Onset* _ons; // pointer to onset object
    float _lSync[16] = {1,0.4,1,0.4,1,0.4,1,0.4,1,0.4,1,0.4,1,0.4,1,0.4}; // likelihood of observing beat
    int _sigmaSync; 
    float _thetaSync; // threshold
    float _beta; // scaling factor
};

#endif
