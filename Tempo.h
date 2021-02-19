#ifndef TEMPO
#define TEMPO
#include <Arduino.h>

class Tempo{
  public:
    Tempo(int& t);
    void setLTempo(int index, float value);
    void setAlpha(float value);
    
    
  private:
    void _setTau(int value);
    
    int* _tau; // pointer to metronome tau
    float _lTempo[16]; // likelihood of observing beat
    float _sigmaTempo; // standard deviation
    int _deltaTauTempo;
    float _thetaTempo; // threshold
    float _alpha; // scaling factor
};

#endif
