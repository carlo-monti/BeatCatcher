#ifndef SYNC
#define SYNC
#include <Arduino.h>
#include "Onset.h"

class Sync{
  public:
    Sync(Onset &onsets);
    void initializeSync();
    void setLSync(int index, float value);
    void setBeta(float value);
    void setSigmaSync(int value);
    void setThresholdSync(float value);
    void updateSync();
    
  private:
    int* _tau; // pointer to click tau
    Onset* _ons; // pointer to onset object
    float _lSync[16]; // likelihood of observing beat
    float _sigmaSync; // standard deviation
    int _deltaTauSync;
    float _thetaSync; // threshold
    float _beta; // scaling factor
};

#endif
