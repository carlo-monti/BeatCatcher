#ifndef ONSET_H
#define ONSET_H
#define NUMBER_OF_ONSETS 800
#include <Arduino.h>
#include "Click.h"

class Onset{
  public:
  
    Onset(Click* clk, int piezo0, int piezo1);
    void initializeOnset(); // to be called in setup
    bool updateOnset(); // updates the onset array
    long getCurrentOnset();
    void setCurrentOnsetAs16th(bool is16th);
    void setSwingMode(bool is128);
    
    long onsets[NUMBER_OF_ONSETS];                          // array of detected onsets
    long peak[NUMBER_OF_ONSETS];                            // array for peak value of detected onsets
    bool is16th[NUMBER_OF_ONSETS];                          // index n is true if the n onset is considered a 16th
    bool isSnare[NUMBER_OF_ONSETS];                         // index n is true if is a snare onset
    const int onsetsLength = NUMBER_OF_ONSETS;              // length of the onset array
    int currentOnsetIndex;                                  // index of the current onset detected
    int next;                                               // index of next onset to be detected
    int last;                                               // index of last onset of the 2 bars
    
  private:

    void scanChannels(int selectedPiezo);
    Click* _clk;                                            // pointer to click object
    bool _is128;
    int _piezo0;                                            // pin for the piezo
    int _piezo1;                                            // pin for the piezo
    int _threshold0;                            // gate threshold for the onset to be detected
    int _bounceTime0; // ms to wait for the new onset after a the last one (to stabilize)
    int _threshold1; // gate threshold for the onset to be detected
    int _bounceTime1; // ms to wait for the new onset after a the last one (to stabilize)
    long _nextAllowedOnsetKick; // time of the next allowed onset (to be used with bounceTime); 
    long _nextAllowedOnsetSnare; // time of the next allowed onset (to be used with bounceTime); 
    int _twoBarLengthInTatum;
    long _lastUpdate;
    int _storedValue0;
    int _storedValue1;
    int _currentValue0;
    int _currentValue1;
    int _deltaAmount0;
    int _deltaAmount1;
    int _dischargeValue0;
    int _dischargeValue1;    
};

#endif
