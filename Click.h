#ifndef CLICK
#define CLICK
#include <Arduino.h>

class Click{
  public:
    Click(int piezo);
    void setTau(int value);
    void updateTauSync(long deltaTauSync);
    void updateTauTempo(long deltaTauTempo);
    void initializeClick();
    void updateClick();
    void resetClick(); // to be called at run-time to set new tempo

    int tau; // tactum in ms: 1/8
    long expectedClick; // currentExpectedClick (absolute value)
    int barPosition; // current position within the 2 bar length (0-15)
    int layerOf[16] = {3,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0}; //
    
  private:
    
    int _piezoPin; // pin for the piezo
    int _threshold; // gate threshold for the onset to be detected (special for metronome reset)
    int _bounceTime; // ms to wait for the new onset after a the last one to stabilize (special for metronome reset)
    long _currentMillis; // place to store millis() to avoid recalling
    long _nextClick; // next scheduled click
    long _nextHalfClick; // point between two scheduled clicks
    long _deltaTauSync; // delta for the synchronizing process
    long _deltaTauAlign; // deltaTau to add for compensate tempo change
    bool _hasAlreadyUpdated;
    long _midiTau; // value for midi
    long _nextMidiClock;
    int _midiClockSent;
};

#endif
