#include "Tempo.h"

Tempo::Tempo(int& t){
  _tau = &t;
}

void Tempo::setLTempo(int index, float value){
  _lTempo[index] = value;
}

void Tempo::setAlpha(float value){
  _alpha = value;
}

void Tempo::_setTau(int value){
  *_tau = value;
}
