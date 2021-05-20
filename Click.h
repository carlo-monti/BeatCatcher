#ifndef CLICK_H
#define CLICK_H
#include <Arduino.h>

class Sync;                                                             // forward declaration
class Tempo;                                                            // forward declaration

class Click{
  public:
    Click(int piezo);
    void initializeClick();
    void updateClick();
    void resetClick();
    void setTau(int value);
    void setDeltaTauSync(long deltaTauSync);
    void setDeltaTauTempo(long deltaTauTempo);
    void setDeltaTauRecovery(long deltaTauRecovery);
    void setSwingMode(bool is128);
    void addSyncObject(Sync *syn);
    void addTempoObject(Tempo *tmp);
    long getCurrentLayer();
    
    long tau;                                                             // tactum in ms (value of 1/8 note)
    long expectedBeat;                                                    // currentExpectedBeat position
    int barPosition;                                                      // current position within the 2 bar length (0-15)
    int barPosition16th;                                                  // current position within the bar in 16th
    
  private:
    Sync* _syn;                                                           // pointer to sync object
    Tempo* _tmp;                                                          // pointer to sync object
    int* _layerOf;   
    bool _is128;
    int _grooveLength;
    int _piezoPin;                                                        // pin for the piezo
    int _threshold;                                                       // gate threshold for the onset to be detected
    int _bounceTime;                                                      // ms to wait for the new onset after a the last one to stabilize
    long _nextClick;                                                      // next scheduled click
    long _nextBarPositionChange;                                          // point where the bar position number changes
    long _next16thChange;                                                 // point where the position number for 16th changes
    long _deltaTauSync;                                                   // delta for the synchronizing process
    long _deltaTauLatency[24];                                            // delta for compensate tempo change
    int _lastLatency[24];                                                // last latency values
    int _latencyBuffer;                                                   //      
    long _deltaTauRecovery[24];                                           // delta for the recovery process
    long _midiTau;                                                        // distance between two MIDI Clocks
    long _nextMidiClock;                                                  // next scheduled MIDI Clock
    int _midiClockSent;                                                   // number of MIDI Clocks sent
    int _layerOf44[16];                                                   // layer value for 4/4
    int _layerOf128[24];                                                  // layer value for 12/8
    void _startClickReset(long firstHit);
};

#endif
