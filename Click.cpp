#include "Click.h"
#include "Sync.h"
#include "Tempo.h"

Click::Click(int piezo){
  _piezoPin = piezo;                                // pin number for the control piezo
  _bounceTime = 250;                                // rest time to settle before a new hit can be detected
  _threshold = 40;                                  // threshold to overcome for hit detection
  setSwingMode(false);
  _latencyBuffer = 3;
  _layerOf44[0] = _layerOf44[8] = 3;                       // layer value for every bar position for 4/4
  _layerOf44[1] = _layerOf44[9] = 0;                       // ...
  _layerOf44[2] = _layerOf44[10] = 1;
  _layerOf44[3] = _layerOf44[11] = 0;
  _layerOf44[4] = _layerOf44[12] = 2;
  _layerOf44[5] = _layerOf44[13] = 0;
  _layerOf44[6] = _layerOf44[14] = 1;
  _layerOf44[7] = _layerOf44[15] = 0;

  _layerOf128[0] = _layerOf128[12] = 3;                       // layer value for every bar position for 12/8
  _layerOf128[1] = _layerOf128[13] = 0;                       // ...
  _layerOf128[2] = _layerOf128[14] = 0;
  _layerOf128[3] = _layerOf128[15] = 1;
  _layerOf128[4] = _layerOf128[16] = 0;
  _layerOf128[5] = _layerOf128[17] = 0;
  _layerOf128[6] = _layerOf128[18] = 2;
  _layerOf128[7] = _layerOf128[19] = 0;
  _layerOf128[8] = _layerOf128[20] = 0;
  _layerOf128[9] = _layerOf128[21] = 1;
  _layerOf128[10] = _layerOf128[22] = 0;
  _layerOf128[11] = _layerOf128[23] = 0;
}

void Click::initializeClick(){
  for(int i=0; i<_grooveLength; i++){                          // reset deltaTauLatency
    _deltaTauLatency[i] = 0;
    _deltaTauRecovery[i] = 0;                            // reset deltaTauRecovery
    _lastLatency[i] = 0;
  }
  _deltaTauSync = 0;                                // reset deltaTauSync
  resetClick();
}

void Click::addSyncObject(Sync *syn){               // adds a pointer to Sync obj
  _syn = syn;                     
}

void Click::addTempoObject(Tempo *tmp){             // adds a pointer to Tempo obj
  _tmp = tmp;
}

long Click::getCurrentLayer(){
  return _layerOf[barPosition];
}

void Click::setDeltaTauSync(long deltaTauSync){
  _deltaTauSync = deltaTauSync;
}

void Click::setSwingMode(bool is128){
  if(is128){
    _layerOf = _layerOf128;
    _grooveLength = 24;
  }else{
    _layerOf = _layerOf44;
    _grooveLength = 16;
  }
  _is128 = is128;       
}

void Click::setDeltaTauTempo(long deltaTauTempo){  
  tau += deltaTauTempo;    
  //Serial.print("Bpm ");Serial.print(60000 / (2 * tau));Serial.print(" Tau ");Serial.println(tau);
    // update tau and
  if(deltaTauTempo > 0){
    _lastLatency[(barPosition)] += 2;
  }else{
    _lastLatency[(barPosition)] -= 2;
  }
  int factor = 0;
  for(int i=_latencyBuffer; i>=0; i--){
    factor += _lastLatency[(barPosition + _grooveLength - i) % _grooveLength];      
  }
  long deltaTauLatencyAmount = deltaTauTempo + deltaTauTempo * factor;                   // 
  _deltaTauLatency[(barPosition + 1) % _grooveLength] += deltaTauLatencyAmount;               // set deltaTauLatency factor
}

void Click::setDeltaTauRecovery(long deltaTauRecovery){        
  long deltaTauRecoveryFactor = deltaTauRecovery;
  _deltaTauRecovery[(barPosition + 1) % _grooveLength] +=  deltaTauRecoveryFactor;          // set deltaTauRecovery factor 
}

void Click::updateClick(){                                                      // update the click state:
  long currentMillis = millis();                                                // set current time

  /*if(analogRead(_piezoPin)>0){                                                  // if the pad has a hit sta
    _startClickReset(currentMillis);                                              // start reset
  }  */                     
                                                        
  if(currentMillis >= _nextClick){                                              // if it is time for new click do:
    long nextStep = tau + _deltaTauLatency[barPosition] 
      + _deltaTauRecovery[barPosition];                                         // calculate time distance of the next step
    _deltaTauLatency[barPosition] = 0;                                          // reset deltaTauLatency for current position
    
    _lastLatency[(barPosition + _grooveLength - _latencyBuffer)  % _grooveLength] = 0;
    
    _deltaTauRecovery[barPosition] = 0;                                         // reset deltaTauRecovery for current position
    _nextClick = currentMillis + nextStep;                                      // set position next click
    _nextBarPositionChange = currentMillis + (nextStep/2);                      // set position of next bar position change
    //Serial.println("dcc");
    _next16thChange = currentMillis + (nextStep/4);                             // set position of next 16th position change
    _midiTau = (_nextClick - currentMillis) / 12;                   // calculate tau for MIDI Clock
    //Serial.println("MIDI Clock First"); // send MIDI Clock msg xxx
    Serial1.write(248);
    _midiClockSent = 1;                                                         // count the msg sent 
    _nextMidiClock = currentMillis + _midiTau;                                  // set position of next MIDI Clock

    if(_is128){
      if(barPosition % 12 == 0){                                                // if a new bar has begun do:
        tone(14, 140, 50);                                                        // flash a led
      }else if(barPosition % 3 == 0){
        tone(14, 140, 10);  
      }
    }else{
      if(barPosition % 8 == 0){                                                   // if a new bar has begun do:
        tone(14, 140, 50);                                                        // flash a led
      }else if(barPosition % 2 == 0){
        tone(14, 140, 10);  
      }
    }
    
  }
  
  if(currentMillis >= _nextBarPositionChange){                                  // if it is time to change the bar position number do:
    _nextClick = _nextClick + _deltaTauSync;                                    // update value of next click for sync and recovery
    _deltaTauSync = 0;                                                          // reset deltaTauSync
    _nextBarPositionChange = _nextClick + (tau/2);                              // set position of next bar position change
    _next16thChange = currentMillis + ((_nextClick - currentMillis) / 2);       // set position of next 16th position change
    _midiTau = (_nextClick - currentMillis) / 6;                               // calculate tau for MIDI Clock
    //Serial.println("MIDI Clock Half"); // xxx Send midi clock
    Serial1.write(248);
    _midiClockSent = 1;                                                         // count the msg sent 
    _nextMidiClock = currentMillis + _midiTau;                                  // set position of next MIDI Clock
    expectedBeat = _nextClick;                                                 // set next click as expectedBeat
    barPosition = (barPosition + 1) % _grooveLength;                            // set new bar position number
  }
    
  if(currentMillis >= _next16thChange){                                         // if it is time for change the 16th position do:
    _next16thChange = min(_nextBarPositionChange, _nextClick) + tau/2;          // set position of next 16th position change
    barPosition16th = (barPosition16th + 1) % _grooveLength;              // increment position value
  }

  if(currentMillis >= _nextMidiClock && _midiClockSent < 6){                    // if it is time for the next MIDI Clock do:
    Serial1.write(248);
    //Serial.println("MIDI Clock"); // xxx Send midi clock                      // send MIDI Clock msg
    _midiClockSent++;                                                           // count the msg sent 
    _nextMidiClock = currentMillis + _midiTau;                                  // set position of next MIDI Clock
  }
}

void Click::resetClick(){
  //Serial.println("reset");
      Serial1.write(252);
    digitalWrite(21,LOW);
        digitalWrite(22,LOW);
  //send stop MidiClock msg xxx
  long hits[4] = {0,0,0,0};
  int hitsDetected = 0;
  long nextAllowedOnset = 0;
  while(hitsDetected < 4){
    long currentMillis = millis();
    if(analogRead(_piezoPin) > _threshold){
      if(currentMillis > nextAllowedOnset){
        Serial.println("onD");
        hits[hitsDetected] = currentMillis;
        hitsDetected++;
        nextAllowedOnset = currentMillis + _bounceTime;
      }
    }
  }

  if(_is128){
    // sets the tempo as the average of the 4 hits
    tau = int((hits[1]-hits[0]) + (hits[2]-hits[1]) + (hits[3]-hits[2])) / 9;
    //Serial.print("Bpm ");Serial.print(60000 / (2 * tau));Serial.print(" Tau ");Serial.println(tau);
    _nextClick = hits[3] + (4 * tau);
    expectedBeat = hits[3] + (3 * tau);
  }else{
    // sets the tempo as the average of the 4 hits
    tau = int((hits[1]-hits[0]) + (hits[2]-hits[1]) + (hits[3]-hits[2])) / 6;
    //Serial.print("Bpm ");Serial.print(60000 / (2 * tau));Serial.print(" Tau ");Serial.println(tau);
    _nextClick = hits[3] + (3 * tau);
    expectedBeat = hits[3] + (2 * tau);
  }
  _nextBarPositionChange = _nextClick - (tau / 2);
  _next16thChange = _nextBarPositionChange - (tau / 4);
  _midiTau = tau / 24;
  _nextMidiClock = expectedBeat + _midiTau;
  _syn->initializeSync();
  _tmp->initializeTempo();
  barPosition = 0;
  barPosition16th = 0;
  delay(2 * tau);
  //send startMidiClock msg xxx
  Serial1.write(250);
  Serial.println("MIDI Clock first");
} 

void Click::_startClickReset(long firstHit){
  Serial.println("reset");
  //send stop MidiClock msg xxx
  long hits[4] = {firstHit,0,0,0};
  int hitsDetected = 1;
  long nextAllowedOnset = 0;
  while(hitsDetected < 4){
    long currentMillis = millis();
    if(analogRead(_piezoPin) > _threshold){
      if(currentMillis > nextAllowedOnset){
        Serial.println("onD");
        hits[hitsDetected] = currentMillis;
        hitsDetected++;
        nextAllowedOnset = currentMillis + _bounceTime;
      }
      Serial.println("NH");
    }

      
  }

  if(_is128){
    // sets the tempo as the average of the 4 hits
    tau = int((hits[1]-hits[0]) + (hits[2]-hits[1]) + (hits[3]-hits[2])) / 9;
    Serial.print("Bpm ");Serial.print(60000 / (2 * tau));Serial.print(" Tau ");Serial.println(tau);
    _nextClick = hits[3] + (4 * tau);
    expectedBeat = hits[3] + (3 * tau);
  }else{
    // sets the tempo as the average of the 4 hits
    tau = int((hits[1]-hits[0]) + (hits[2]-hits[1]) + (hits[3]-hits[2])) / 6;
    Serial.print("Bpm ");Serial.print(60000 / (2 * tau));Serial.print(" Tau ");Serial.println(tau);
    _nextClick = hits[3] + (3 * tau);
    expectedBeat = hits[3] + (2 * tau);
  }
  _nextBarPositionChange = _nextClick - (tau / 2);
  _next16thChange = _nextBarPositionChange - (tau / 4);
  _midiTau = tau / 24;

  _nextMidiClock = expectedBeat + _midiTau;
  _syn->initializeSync();
  _tmp->initializeTempo();
  barPosition = 0;
  barPosition16th = 0;
  delay(2 * tau);
  Serial.println("START MSG"); //XX
  //Serial.println("MIDI Clock first");
} 
