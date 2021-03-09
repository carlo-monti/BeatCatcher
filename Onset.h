#ifndef ONSET
#define ONSET
#define NUMBER_OF_ONSETS 800
#include <Arduino.h>
#include "Click.h"

class Onset{
  public:
  
    Onset(Click *clk, int piezo0, int piezo1);
    void setThreshold(bool piezo, int thr);
    void setBounceTime(bool piezo, int bounce);
    void initializeOnset(); // to be called in setup
    bool updateOnset(); // updates the onset array
    
    long onsets[NUMBER_OF_ONSETS]; // array of detected onsets
    bool is16th[NUMBER_OF_ONSETS]; // index n is true if the n onset is considered a 16th
    bool isSnare[NUMBER_OF_ONSETS]; // index n is true if is a snare onset
    const int onsetsLength = NUMBER_OF_ONSETS; // length of the onset array
    int currentOnset; // index of the current onset detected
    int next; // index of next onset to be detected
    int last; // index of last onset of the 2 bars
    
  private:

    Click* _clk; // pointer to click object
    int _piezo0; // pin for the piezo
    int _piezo1; // pin for the piezo
    int _threshold0; // gate threshold for the onset to be detected
    int _bounceTime0; // ms to wait for the new onset after a the last one (to stabilize)
    int _threshold1; // gate threshold for the onset to be detected
    int _bounceTime1; // ms to wait for the new onset after a the last one (to stabilize)
    long _nextAllowedOnsetKick; // time of the next allowed onset (to be used with bounceTime); 
    long _nextAllowedOnsetSnare; // time of the next allowed onset (to be used with bounceTime); 
    long _currentMillis; // place to store millis() to avoid recalling
};

#endif
