#ifndef SYNC_H
#define SYNC_H
#include <Arduino.h>
#include "Click.h"
#include "Onset.h"

class Sync{
  public:
    Sync(Onset* onsets, Click* clk);
    void initializeSync();
    void setLSync(bool type, int index, float value);
    void setBeta(float value);
    void setThreshold(float value);
    void updateSync();
    void updateLayerAndGroove();
    
  private:
    Click* _clk;                                  // pointer to click object
    Onset* _ons;                                  // pointer to onset object
    float _lSyncKick[16];                         // likelihood of observing onset for kick
    float _lSyncSnare[16];                        // likelihood of observing onset for snare
    int _sigmaSync;                               // width of the window
    float _thetaSync;                             // threshold
    float _beta;                                  // scaling factor
    int _currentHigherLayer;                      // layer the system is currently on
    float _lastAccuracyOfCurrentLayer;            // stores the last accuracy of a beat that has been synced to
    int _lastBarPosition;                         // needed to detect bar change
    float _accuracyOfLastSync[16];
    float _narrowWindowRatio = 1;                 // set a value between 1 and 0.1
    float _expandWindowRatio = 1;
    
    long lastMillisForPrint;
    bool onsetKickPerStampa;
    bool onsetSnarePerStampa;
    
    int _lastBarPosition16th;
    bool _hasKick;
    bool _hasSnare;
    long _lastRecovery;
    float _averageFactor;
    const int _allowedDifference = 14;
    float _averageKickGroove[16];
    float _averageSnareGroove[16];
    bool _currentKickGroove[16];
    bool _currentSnareGroove[16];
    bool _storedKickGroove[16];
    bool _storedSnareGroove[16];
};

#endif
