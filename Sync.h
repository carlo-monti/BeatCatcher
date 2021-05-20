#ifndef SYNC_H
#define SYNC_H
#include <Arduino.h>
#include "Click.h"
#include "Onset.h"

class Sync{
  public:
    Sync(Onset* onsets, Click* clk);
    void initializeSync();
    void setSwingMode(bool is128);
    void setTwoBarsLength(bool isTwoBars);
    void updateSync();
    void updateLayerAndGroove();
    
  private:
    Click* _clk;                                  // pointer to click object
    Onset* _ons;                                  // pointer to onset object
    float _lSyncKick44[16];                       // lSync for kick in 4/4
    float _lSyncSnare44[16];                      // lSync for snare in 4/4
    float _lSyncKick128[24];                      // lSync for kick in 12/8
    float _lSyncSnare128[24];                     // lSync for snare in 12/8
    float* _lSyncKick;                                // pointer to lSync array for kick or snare
    float* _lSyncSnare;                                // pointer to lSync array for kick or snare
    int _sigmaSync;                               // width of the window
    float _thetaSync;                             // threshold
    float _beta;                                  // scaling factor
    int _currentHigherLayer;                      // layer the system is currently on
    float _lastAccuracyOfCurrentLayer;            // stores the last accuracy of a beat that has been synced to
    int _lastBarPosition;                         // value needed to detect bar change
    int _lastLayerOfBarPosition;                  // value needed to detect layer change
    float _accuracyOfLastSync[24];
    float _narrowWindowRatio = 1;                 // set a value between 1 and 0.1
    float _expandWindowRatio = 1;
    bool _is128;
    bool _isTwoBars;
    
    long lastMillisForPrint;
    bool onsetKickPerStampa;
    bool onsetSnarePerStampa;

    int _grooveLength;
    int _lastBarPosition16th;
    bool _hasKick;
    bool _hasSnare;
    long _lastRecovery;
    int _averageGrooveThreshold;                    // bars taken for a hit to be stored
    int _averageGrooveStability;                    // bars taken for a hit to be removed (if it has max value)
    int _allowedGrooveDifference;                   // allowed difference between current and stored groove
    int _averageKickGroove[48];
    int _averageSnareGroove[48];
    bool _currentKickGroove[48];
    bool _currentSnareGroove[48];
    bool _storedKickGroove[48];
    bool _storedSnareGroove[48];
    int _howManyHits;
    void _updateGrooveRepresentation();
    bool _checkAndCallRecovery();
};

#endif
