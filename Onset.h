#ifndef ONSET
#define ONSET
#define BOUNCE_DEFAULT 150
#define THRESHOLD_DEFAULT 20
#define NUMBER_OF_ONSETS 800
#include <Arduino.h>

class Onset{
  public:
  
    Onset(int& t, int piezo, int bounce=BOUNCE_DEFAULT, int thr=THRESHOLD_DEFAULT);
    void setThreshold(int thr);
    void setBounceTime(int bounce);
    void initializeOnset(); // to be called in setup
    bool updateOnset(); // updates the onset array
    
    long onsets[NUMBER_OF_ONSETS]; // array of detected onsets
    bool syncDone[NUMBER_OF_ONSETS]; // index n is true if the n onset has already been used to synchronize
    const int onsetsLength = NUMBER_OF_ONSETS; // length of the onset array
    int next; // index of next onset to be detected
    int last; // index of last onset of the 2 bars
    
  private:
    
    int* _tau; // pointer to metronome tau
    int _piezoPin; // pin for the piezo
    int _threshold; // gate threshold for the onset to be detected
    int _bounceTime; // ms to wait for the new onset after a the last one (to stabilize)
    long _nextAllowedOnset; // time of the next allowed onset (to be used with bounceTime); 
    long _currentMillis; // place to store millis() to avoid recalling
};

#endif
