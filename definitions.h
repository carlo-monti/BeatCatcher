#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "Click.h"
#include "Onset.h"
#include "Sync.h"
#include "Tempo.h"

#define blueLed 22
#define greenLed 21
#define redLed 20
#define internalLed 13
#define bKey 2 // 
#define aKey 3 // 
#define dKey 4 // 
#define cKey 5 // 
#define mode 6 // mode switch
#define piezo0 A16 //
#define piezo1 A14 //
#define piezo2 A15 //
#define pot A17 //
#define buzzer 14 //

void setupPinsAndParameters(){
  pinMode(blueLed,OUTPUT);
  pinMode(greenLed,OUTPUT);
  pinMode(redLed,OUTPUT);
  pinMode(internalLed,OUTPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(piezo0,INPUT);
  pinMode(piezo1,INPUT);
  pinMode(piezo2,INPUT);
  pinMode(aKey,INPUT_PULLUP);
  pinMode(bKey,INPUT_PULLUP);
  pinMode(cKey,INPUT_PULLUP);
  pinMode(dKey,INPUT_PULLUP);
  pinMode(mode,INPUT_PULLUP);
  //analogReadRes(10); // reduce analog read resolution
  //analogReadAveraging(2); // reduce analog read averaging
}

#endif
