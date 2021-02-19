#include "Sync.h"

Sync::Sync(Onset &onsets){
  _ons = &onsets;
}

void Sync::initializeSync(){
}

void Sync::setLSync(int index, float value){
  _lSync[index] = value;
}

void Sync::setBeta(float value){
  _beta = value;
}

void Sync::updateSync(){
  
}
